#pragma once

//#include "platform_common.h"
#include "../common.h"
#include "../ITKCommon/StringUtil.h"
#include "Mutex.h"
#include "AutoLock.h"
#include "Thread.h"
#include "Sleep.h"
#include "../ITKCommon/Path.h"
#include "IPC/LowLatencyQueueIPC.h"

#if defined(_WIN32)
#include "Core/WindowsPipe.h"
#elif defined(__linux__) || defined(__APPLE__)
#include "Core/UnixPipe.h"
#endif

#if defined(_WIN32)
#pragma warning( push )
#pragma warning( disable : 4996)
#endif

#include <InteractiveToolkit/EventCore/ExecuteOnScopeEnd.h>

namespace Platform
{

    class Process
    {

        Platform::Mutex mutex;
        std::string lpApplicationName;
        std::string commandLine;

        bool process_created;

        std::string pid_str;

        int exit_code;

        int force_horrible_terminate_after_ms;

#if defined(_WIN32)
        STARTUPINFO startupInfo;
        PROCESS_INFORMATION processInformation;
#elif defined(__linux__) || defined(__APPLE__)

        pid_t created_pid;

#endif

    public:
        static bool ApplicationExists(const std::string &_lpApplicationName)
        {
#if defined(_WIN32)
            return ITKCommon::Path::isFile(_lpApplicationName);
#elif defined(__linux__) || defined(__APPLE__)
            std::string app_name = _lpApplicationName;
            if (!ITKCommon::Path::isFile(app_name))
            {
                // search executable in path variable
                char *dup = strdup(getenv("PATH"));
                EventCore::ExecuteOnScopeEnd _exec_on_scope_end([dup](){
                    free(dup);
                });
                char *s = dup;
                char *p = NULL;
                do
                {
                    p = strchr(s, ':');
                    if (p != NULL)
                    {
                        p[0] = 0;
                    }
                    std::string exe_path = std::string(s) + ITKCommon::PATH_SEPARATOR + app_name;
                    if (ITKCommon::Path::isFile(exe_path))
                    {
                        // check can execute
                        struct stat st;
                        if (stat(exe_path.c_str(), &st) >= 0)
                        {
                            if ((st.st_mode & S_IEXEC) != 0)
                            {
                                printf("[Process:ApplicationExists] Executable found at: %s\n", exe_path.c_str());
                                app_name = exe_path;
                                return true;
                                break;
                            }
                        }
                    }
                    s = p + 1;
                } while (p != NULL);
                //free(dup);
            }
            else
                return true;

            return false;
#endif
        }

        Process(const std::string &_lpApplicationName, const std::vector<std::string> &vector_argv, int _force_horrible_terminate_after_ms = 5000
#if defined(_WIN32)
                        ,
                        WindowsPipe *pipe_stdin = NULL, WindowsPipe *pipe_stdout = NULL, WindowsPipe *pipe_stderr = NULL
#elif defined(__linux__) || defined(__APPLE__)
                        ,
                        UnixPipe *pipe_stdin = NULL, UnixPipe *pipe_stdout = NULL, UnixPipe *pipe_stderr = NULL
#endif
        )
        {
            exit_code = 0;
            pid_str = "";
            process_created = false;
            lpApplicationName = _lpApplicationName;
            commandLine = ITKCommon::StringUtil::argvToCmd(vector_argv);
            force_horrible_terminate_after_ms = _force_horrible_terminate_after_ms;

#if defined(_WIN32)

            ITKCommon::StringUtil::replaceAll(&lpApplicationName, "/", ITKCommon::PATH_SEPARATOR);

            if (lpApplicationName.find(".") == std::string::npos)
                lpApplicationName += ".exe";

            /*
            TCHAR buffer[MAX_PATH];
            TCHAR** lppPart = { NULL };
            DWORD retval = GetFullPathName(
                lpApplicationName.c_str(),
                MAX_PATH,
                buffer,
                lppPart
            );
            if (retval)
                lpApplicationName = buffer;
            */

            if (commandLine.length() > 0)
                commandLine = "\"" + lpApplicationName + "\"" + " " + commandLine;
            else
                commandLine = "\"" + lpApplicationName + "\"";

            // set the size of the structures
            ZeroMemory(&startupInfo, sizeof(startupInfo));
            startupInfo.cb = sizeof(startupInfo);
            ZeroMemory(&processInformation, sizeof(processInformation));

            startupInfo.hStdInput = NULL;  // read from pipe
            startupInfo.hStdOutput = NULL; // write to pipe
            startupInfo.hStdError = NULL;  // write to pipe

            if (pipe_stdin != NULL)
                startupInfo.hStdInput = pipe_stdin->read_fd; // read from pipe
            if (pipe_stdout != NULL)
                startupInfo.hStdOutput = pipe_stdout->write_fd; // write to pipe
            if (pipe_stderr != NULL)
                startupInfo.hStdError = pipe_stderr->write_fd; // write to pipe

            startupInfo.dwFlags |= STARTF_USESTDHANDLES;

            // start the program up
            process_created = CreateProcess(NULL,                   //(LPCTSTR)lpApplicationName.c_str(),   // the path
                                            (LPSTR)&commandLine[0], // Command line
                                            NULL,                   // Process handle not inheritable
                                            NULL,                   // Thread handle not inheritable
                                            TRUE,                   // Set handle inheritance to FALSE
                                            0,                      // CREATE_NEW_PROCESS_GROUP, // No creation flags // CREATE_NEW_CONSOLE |
                                            NULL,                   // Use parent's environment block
                                            NULL,                   // Use parent's starting directory
                                            &startupInfo,           // Pointer to STARTUPINFO structure
                                            &processInformation     // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
            );

            if (isCreated())
            {
                char aux[64];
                snprintf(aux, 64, "p%u", processInformation.dwProcessId);
                pid_str = aux;

                if (pipe_stdin != NULL)
                    pipe_stdin->closeReadFD(); // read from pipe
                if (pipe_stdout != NULL)
                    pipe_stdout->closeWriteFD(); // write to pipe
                if (pipe_stderr != NULL)
                    pipe_stderr->closeWriteFD(); // write to pipe
            }

#elif defined(__linux__) || defined(__APPLE__)

            // resolve app name before fork
            if (!ITKCommon::Path::isFile(lpApplicationName))
            {
                // search executable in path variable
                char *dup = strdup(getenv("PATH"));
                char *s = dup;
                char *p = NULL;
                do
                {
                    p = strchr(s, ':');
                    if (p != NULL)
                    {
                        p[0] = 0;
                    }
                    std::string exe_path = std::string(s) + ITKCommon::PATH_SEPARATOR + lpApplicationName;
                    if (ITKCommon::Path::isFile(exe_path))
                    {
                        // check can execute
                        struct stat st;
                        if (stat(exe_path.c_str(), &st) >= 0)
                        {
                            if ((st.st_mode & S_IEXEC) != 0)
                            {
                                printf("[Process] Executable found at: %s\n", exe_path.c_str());
                                lpApplicationName = exe_path;
                                break;
                            }
                        }
                    }
                    s = p + 1;
                } while (p != NULL);
                free(dup);
            }

            created_pid = fork();
            if (created_pid == 0)
            {
                // child process
                // set pipes
                if (pipe_stdin == NULL)
                    SinkStdFD(STDIN_FILENO);
                else
                {
                    pipe_stdin->aliasReadAs(STDIN_FILENO);
                    pipe_stdin->close();
                }

                if (pipe_stdout == NULL)
                    SinkStdFD(STDOUT_FILENO);
                else
                {
                    pipe_stdout->aliasWriteAs(STDOUT_FILENO);
                    pipe_stdout->close();
                }

                if (pipe_stderr == NULL)
                    SinkStdFD(STDERR_FILENO);
                else
                {
                    pipe_stderr->aliasReadAs(STDERR_FILENO);
                    pipe_stderr->close();
                }

                // execve replaces the current process memory with the new shell executable

                std::vector<std::string> vector_argv_local = vector_argv;
                std::vector<char *> argv(vector_argv.size() + 2);
                argv[vector_argv.size() + 2 - 1] = NULL;
                for (size_t i = 0; i < vector_argv.size(); i++)
                {
                    //printf("[Process] argv[%i] = %s\n", (int)i, vector_argv_local[i].data());
                    argv[i + 1] = vector_argv_local[i].data();
                }
                // printf("[Process] %s %s\n", lpApplicationName.c_str(), commandLine.c_str());

                argv[0] = lpApplicationName.data();
                // char* const envp[] = { NULL,NULL };
                // printf("Will execute: %s\n",lpApplicationName.c_str());
                execve(lpApplicationName.c_str(), argv.data(), environ); // envp);

                // exit(0);
                perror((std::string("Error to execute: ") + lpApplicationName).c_str());

                kill(getpid(), SIGKILL); // SIGABRT);//SIGKILL);
                // exit(127);
            }
            else
            {
                // close unused host side pipe writter/reader
                if (pipe_stdin != NULL)
                    pipe_stdin->closeReadFD();

                if (pipe_stdout != NULL)
                    pipe_stdout->closeWriteFD();

                if (pipe_stderr != NULL)
                    pipe_stderr->closeWriteFD();
            }

            process_created = created_pid > 0;

            printf("process_created %i\n", process_created);

#endif
        }

        bool waitExit(int *_exit_code, uint32_t timeout_ms)
        {
            Platform::AutoLock autoLock(&mutex);

            if (isCreated())
            {

#if defined(_WIN32)
                if (WaitForSingleObject(processInformation.hProcess, timeout_ms) == WAIT_TIMEOUT)
                {
                    return false;
                }
                DWORD code;
                if (GetExitCodeProcess(processInformation.hProcess, &code))
                {
                    exit_code = (int)code;
                    *_exit_code = exit_code;
                    return true;
                }
#elif defined(__linux__) || defined(__APPLE__)

                int status;
                bool timeout = false;

                Platform::Time timer;
                int64_t timer_acc = 0;
                timer.update();
                while (waitpid(created_pid, &status, WNOHANG) != created_pid)
                {
                    timer.update();
                    timer_acc += timer.deltaTimeMicro;
                    if (timer_acc > (int64_t)timeout_ms * (int64_t)1000)
                    {
                        timeout = true;
                        break;
                    }
                    Platform::Sleep::millis(50);
                }

                if (timeout)
                {
                    return false;
                }
                else if (WIFEXITED(status))
                {
                    exit_code = WEXITSTATUS(status);
                    *_exit_code = exit_code;

                    // pid_str = "";
                    process_created = false;
                    return true;
                }
                else
                {
                    exit_code = -1; // exit with error (signal, for example)
                    *_exit_code = exit_code;

                    // pid_str = "";
                    process_created = false;
                    return true;
                }

#endif
            }

            return false;
        }

        int getExitCode()
        {
            return exit_code;
        }

        bool isRunning()
        {
            Platform::AutoLock autoLock(&mutex);
            if (isCreated())
            {
#if defined(_WIN32)
                bool isTerminated = WaitForSingleObject(processInformation.hProcess, 0) == WAIT_OBJECT_0;
                if (isTerminated)
                {
                    DWORD code;
                    if (GetExitCodeProcess(processInformation.hProcess, &code))
                    {
                        exit_code = (int)code;
                    }

                    CloseHandle(processInformation.hProcess);
                    CloseHandle(processInformation.hThread);

                    // pid_str = "";
                    process_created = false;
                }
                return !isTerminated;
#elif defined(__linux__) || defined(__APPLE__)
                int status;
                // waitpid clear the defunct child processes...
                bool isTerminated = waitpid(created_pid, &status, WNOHANG) == created_pid;
                if (isTerminated)
                {
                    if (WIFEXITED(status))
                        exit_code = WEXITSTATUS(status);
                    else if (WIFSIGNALED(status))
                        exit_code = -1; // WTERMSIG(chld_state)

                    // pid_str = "";
                    process_created = false;
                }
                return !isTerminated;
#endif
            }

            return false;
        }

        bool isCreated()
        {
            Platform::AutoLock autoLock(&mutex);
            return process_created;
        }

        void horribleForceTermination()
        {
            Platform::AutoLock autoLock(&mutex);
            if (isCreated())
            {
                bool isRunningAux = isRunning();
                exit_code = 0;
                pid_str = "";
                process_created = false;
#if defined(_WIN32)
                if (isRunningAux)
                    TerminateProcess(processInformation.hProcess, exit_code);
                CloseHandle(processInformation.hProcess);
                CloseHandle(processInformation.hThread);
#elif defined(__linux__) || defined(__APPLE__)

                kill(created_pid, SIGKILL);
                // clear defunct
                int status;
                waitpid(created_pid, &status, 0);

#endif
            }
        }

        int signal(int sig = SIGINT)
        {
            Platform::AutoLock autoLock(&mutex);

            if (isCreated())
            {
#if defined(_WIN32)

                // check the process signal queue
                Platform::IPC::LowLatencyQueueIPC queue(pid_str.c_str(), Platform::IPC::QueueIPC_WRITE, 16, sizeof(int));
                queue.write((uint8_t *)&sig, sizeof(int), false);

                if (WaitForSingleObject(processInformation.hProcess, force_horrible_terminate_after_ms) == WAIT_TIMEOUT)
                {
                    // if the process did not finish in the time
                    //   Forcibly terminate the process...
                    TerminateProcess(processInformation.hProcess, 0);
                }

                DWORD code;
                GetExitCodeProcess(processInformation.hProcess, &code);

                CloseHandle(processInformation.hProcess);
                CloseHandle(processInformation.hThread);

                // ARIBEIRO_ABORT(code != 0, "Child process terminated with code: %u", code);
                exit_code = code;

                pid_str = "";
                process_created = false;

#elif defined(__linux__) || defined(__APPLE__)

                // SIGTERM or a SIGKILL
                kill(created_pid, sig);

                int status;
                bool timeout = false;

                Platform::Time timer;
                int64_t timer_acc = 0;
                timer.update();
                while (waitpid(created_pid, &status, WNOHANG) != created_pid)
                {
                    timer.update();
                    timer_acc += timer.deltaTimeMicro;
                    if (timer_acc > (int64_t)force_horrible_terminate_after_ms * (int64_t)1000)
                    {
                        printf("[Process] FORCE KILL CHILDREN TIMEOUT\n");
                        kill(created_pid, SIGKILL);
                        // clear defunct
                        waitpid(created_pid, &status, 0);
                        timeout = true;
                        break;
                    }
                    Platform::Sleep::millis(50);
                }

                if (timeout)
                {
                    exit_code = 0;
                }
                else if (WIFEXITED(status))
                {
                    exit_code = WEXITSTATUS(status);
                }
                else
                    exit_code = -1; // exit with error (signal, for example)

                pid_str = "";
                process_created = false;
#endif
            }

            return exit_code;
        }

        ~Process()
        {
            signal(SIGINT);
        }
    };

}

#if defined(_WIN32)
#pragma warning( pop )
#endif
