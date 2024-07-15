#pragma once

#include "../common.h"
#include "./StringUtil.h"
#include "ITKAbort.h"

//
// Windows path defs
//
#if defined(_WIN32)

#define SHGetFolderPath_Custom SHGetFolderPathW
#ifndef PathAppend
#define PathAppend PathAppendW
#endif
#ifndef CSIDL_PERSONAL
#define CSIDL_PERSONAL 0x0005 // My Documents
#endif

#define INITGUID

typedef GUID KNOWNFOLDERID;
#define REFKNOWNFOLDERID const KNOWNFOLDERID &

static HRESULT(_stdcall *SHGetKnownFolderPath_Custom)
(
    REFKNOWNFOLDERID rfid,
    DWORD dwFlags,
    HANDLE hToken,
    PWSTR *ppszPath);

#ifndef __out_ecount
#define __out_ecount(v)
#define __inout_ecount(v)
#endif

EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
SHGetFolderPathW(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, __out_ecount(MAX_PATH) LPWSTR pszPath);

#ifndef LWSTDAPI_
#define LWSTDAPI_(type) EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#endif

#if defined(__MINGW32__)
LWSTDAPI_(WINBOOL)
PathAppendW(LPWSTR pszPath, LPCWSTR pMore);
#else
LWSTDAPI_(BOOL)
PathAppendW(__inout_ecount(MAX_PATH) LPWSTR pszPath, LPCWSTR pMore);
#endif

static bool useKnownFolders(void)
{
    static bool result = false;
    static bool count = 0;
    HINSTANCE shell32;
    if (count)
        return result;
    shell32 = LoadLibrary(TEXT("shell32.dll"));
    if (shell32)
    {
        // FARPROC DllGetVersion;
        int(__stdcall * DllGetVersion)(DLLVERSIONINFO * pdvi);
        DLLVERSIONINFO version;
        version.cbSize = sizeof(DLLVERSIONINFO);
        // DllGetVersion = GetProcAddress(shell32, TEXT("DllGetVersion"));
        SHGetKnownFolderPath_Custom = (HRESULT(_stdcall *)(
            REFKNOWNFOLDERID rfid,
            DWORD dwFlags,
            HANDLE hToken,
            PWSTR * ppszPath)) GetProcAddress(shell32, "SHGetKnownFolderPath");
        DllGetVersion = (int(__stdcall *)(DLLVERSIONINFO * pdvi)) GetProcAddress(shell32, "DllGetVersion");
        if (!DllGetVersion)
            result = false;
        else
        { // GetProcAddress succeeded
            result = true;
            DllGetVersion(&version);
            if (version.dwMajorVersion >= 6)
            {
                if (version.dwMajorVersion == 6 && version.dwMinorVersion == 0)
                {
                    if (version.dwBuildNumber < 6000)
                    {
                        result = false;
                    }
                }
            }
            else
            { // version.dwMajorVersion < 6
                result = false;
            }
        }
        FreeLibrary(shell32);
    }
    else
    {
        result = false;
    }
    count = true;
    return result;
}

#ifdef INITGUID
#define DEFINE_KNOWN_FOLDER(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const GUID DECLSPEC_SELECTANY name = {l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}}
#else
#define DEFINE_KNOWN_FOLDER(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const GUID name
#endif // INITGUID

// {4C5C32FF-BB9D-43b0-B5B4-2D72E54EAAA4}
DEFINE_KNOWN_FOLDER(FOLDERID_SavedGames, 0x4c5c32ff, 0xbb9d, 0x43b0, 0xb5, 0xb4, 0x2d, 0x72, 0xe5, 0x4e, 0xaa, 0xa4);

// {FDD39AD0-238F-46AF-ADB4-6C85480369C7}
DEFINE_KNOWN_FOLDER(FOLDERID_Documents, 0xFDD39AD0, 0x238F, 0x46AF, 0xAD, 0xB4, 0x6C, 0x85, 0x48, 0x03, 0x69, 0xC7);

#define _TRUNCATE ((size_t)-1)

#endif

namespace ITKCommon
{

#if defined(_WIN32)
        static constexpr char PATH_SEPARATOR[] = "\\";
#elif defined(__APPLE__) || defined(__linux__)
        static constexpr char PATH_SEPARATOR[] = "/";
#endif

    class Path
    {

#if defined(_WIN32)
        /*
         * This function sets the string to either %USERPROFILE%\My Documents\My Games\<baseDirName>\<gameDirName>
         * (Windows XP or older) or to %USERPROFILE\Saved Games\<baseDirName>\<gameDirName> (Vista and newer).
         */
        static void getSaveGameDir(char *dest, size_t size, const wchar_t *baseDirName, const wchar_t *gameDirName)
        {
            WCHAR szPath[MAX_PATH];
            if (useKnownFolders())
            { // Vista+ has a "Saved Games" known folder
                PWSTR wdest;
                SHGetKnownFolderPath_Custom(FOLDERID_SavedGames, 0, NULL, &wdest);
                // copy to szpath
                // wsprintfW(szPath, TEXT("%s"), wdest);
                wsprintfW(szPath, L"%s", wdest);
                CoTaskMemFree(wdest);
            }
            else
            { // use My Documents\My Games\<baseDirName>
                SHGetFolderPath_Custom(NULL, CSIDL_PERSONAL, NULL, 0, szPath);
                // PathAppendW(szPath, TEXT("My Games"));
                PathAppendW(szPath, L"My Games");
                CreateDirectoryW(szPath, NULL);
            }
            PathAppendW(szPath, baseDirName);
            CreateDirectoryW(szPath, NULL);
            PathAppendW(szPath, gameDirName);
            CreateDirectoryW(szPath, NULL);

            memset(dest, 0, size);

#if __GNUC__
            wcstombs(dest, szPath, size);
#else
            size_t convertedChars;
            size_t inputLength = wcslen(szPath);
            wcstombs_s(&convertedChars, dest, size, szPath, inputLength);
#endif
        }

        static void getDocumentsDir(char *dest, size_t size, const wchar_t *baseDirName, const wchar_t *gameDirName)
        {
            WCHAR szPath[MAX_PATH];
            if (useKnownFolders())
            { // Vista+ has a "Saved Games" known folder
                PWSTR wdest;
                SHGetKnownFolderPath_Custom(FOLDERID_Documents, 0, NULL, &wdest);
                // copy to szpath
                // wsprintfW(szPath, TEXT("%s"), wdest);
                wsprintfW(szPath, L"%s", wdest);
                CoTaskMemFree(wdest);
            }
            else
            { // use My Documents\My Games\<baseDirName>
                SHGetFolderPath_Custom(NULL, CSIDL_PERSONAL, NULL, 0, szPath);
                // PathAppendW(szPath, L"My Games");
                // CreateDirectoryW(szPath, NULL);
            }
            PathAppendW(szPath, baseDirName);
            CreateDirectoryW(szPath, NULL);
            PathAppendW(szPath, gameDirName);
            CreateDirectoryW(szPath, NULL);

            memset(dest, 0, size);

#if __GNUC__
            wcstombs(dest, szPath, size);
#else
            size_t convertedChars;
            size_t inputLength = wcslen(szPath);
            wcstombs_s(&convertedChars, dest, size, szPath, inputLength);
#endif
        }

#elif defined(__APPLE__) || defined(__linux__)

        static std::string unixHomeDirectory()
        {
            size_t bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
            if (bufsize == -1)
                bufsize = 0x4000; // = all zeroes with the 14th bit set (1 << 14)

            char *buf = (char *)malloc(bufsize);

            ITK_ABORT(buf == NULL, "malloc\n");

            passwd pwd;
            passwd *result;

            int s = getpwuid_r(getuid(), &pwd, buf, bufsize, &result);

            ITK_ABORT(result == NULL, "getpwuid_r errorcode: %i\n", s);
            std::string resultstr = std::string(result->pw_dir);

            free(buf);

            return resultstr;
        }

        static bool unixIsDirectory(const char *path)
        {
            if (strcmp(path, ".") == 0 || strcmp(path, "..") == 0)
                return false;
            struct stat st;
            bool pathExists = (stat(path, &st) == 0);
            return pathExists && S_ISDIR(st.st_mode);
        }

        static bool unixIsFile(const char *path)
        {
            struct stat st;
            bool pathExists = (stat(path, &st) == 0);
            return pathExists && S_ISREG(st.st_mode);
        }

        static bool unixCreateDirectory(const char *path)
        {
            return (mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0);
        }

        static void unixCheckPathCreation(const char *path)
        {
            // LOGI("Checking path (%s)",path);
            if (!unixIsDirectory(path))
            {
                // LOGI("  Path does not exists...");
                if (unixCreateDirectory(path))
                {
                    // LOGI("  Created!!!");
                }
                else
                {
                    // LOGI("  Error to create!!!");
                }
            }
            else
            {
                // LOGI("  Path exists!!!");
            }
        }

        static bool unixRemovePath(const char *directory_name)
        {
            DIR *dp;
            struct dirent *ep;
            char p_buf[512] = {0};

            dp = opendir(directory_name);

            while ((ep = readdir(dp)) != NULL)
            {
                snprintf(p_buf, 512, "%s/%s", directory_name, ep->d_name);
                if (unixIsDirectory(p_buf))
                    unixRemovePath(p_buf);
                else
                    unlink(p_buf);
            }

            closedir(dp);
            rmdir(directory_name);

            return true;
        }

        static void unixCleanUpPath(const char *path)
        {
            // LOGI("Cleaning up path (%s)",path);
            if (unixRemovePath(path))
            {
                // LOGI("  Path removed...");
                unixCheckPathCreation(path);
            }
            else
            {
                // LOGI("  Cannot remove path!!!");
            }
        }

#endif

    public:

        /// \brief Get the current executable path.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroPlatform/aRibeiroPlatform.h>
        /// using namespace aRibeiro;
        ///
        /// std::string current_work_directory = PlatformPath::getWorkingPath();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return the current path
        ///
        static ITK_INLINE std::string getWorkingPath()
        {
#if defined(_WIN32)

            auto ptr = _wgetcwd(NULL, 0);
            if (!ptr)
                return "";
            std::string result = StringUtil::wString_to_String(ptr);
            free(ptr);
            return result;

#elif defined(__APPLE__) || defined(__linux__)

            auto ptr = getcwd(NULL, 0);
            if (!ptr)
                return "";
            std::string result = ptr;
            free(ptr);
            return result;

#endif
        }

        /// \brief Set the current executable path.
        ///
        /// It helps to set the current relative path, that all commands related to FILE manipulation work with.
        ///
        /// Can be used in the main function.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroPlatform/aRibeiroPlatform.h>
        /// using namespace aRibeiro;
        ///
        /// int main(int argc, char *argv[]) {
        ///     PlatformPath::setWorkingPath( PlatformPath::getExecutablePath( argv[0] ) );
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param path the new path to set
        /// \return true if its OK
        ///
        static bool setWorkingPath(const std::string &path)
        {
#if defined(_WIN32)
            std::wstring path_w = StringUtil::string_to_WString(path);
            return _wchdir(path_w.c_str()) == 0;
#elif defined(__APPLE__) || defined(__linux__)
            return chdir(path.c_str()) == 0;
#endif
        }

        /// \brief Compute the executable path from the application first argument.
        ///
        /// Can be used in the main function.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroPlatform/aRibeiroPlatform.h>
        /// using namespace aRibeiro;
        ///
        /// int main(int argc, char *argv[]) {
        ///     PlatformPath::setWorkingPath( PlatformPath::getExecutablePath( std::string( argv[0] ) ) );
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param arg0 argument 0 from the application main procedure
        /// \return the computed path
        ///
        static std::string getExecutablePath(const std::string &arg0)
        {
#if defined(_WIN32)
            size_t path_directory_index = arg0.find_last_of('\\');
            return arg0.substr(0, path_directory_index);
#elif defined(__APPLE__) || defined(__linux__)
            size_t path_directory_index = arg0.find_last_of('/');
            auto str = arg0.substr(0, path_directory_index);
            auto ptr = realpath(str.c_str(), NULL);
            if (!ptr)
                return "";
            str = ptr;
            free(ptr);
            return str;
#endif
        }

        /// \brief Get and create the savegame path based to a root folder and game name.
        ///
        /// In windows, the code gets the default save game folder windows configure in their SDK.
        ///
        /// In windows, the path is commonly created at:
        /// C:\Users\username\Saved Games\[rootFolder]\[gameName]
        ///
        /// In linux and similar systems, it gets the user home directory as base and creates a hidden folder starting with a dot.
        ///
        /// In unix, the path is commonly created at:
        /// /home/username/.[rootFolder]/[gameName]
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroPlatform/aRibeiroPlatform.h>
        /// using namespace aRibeiro;
        ///
        /// std::string path_to_save_files = PlatformPath::getSaveGamePath( "Company Root Folder", "Game Name" );
        /// ...
        /// // Save some data on the path
        /// FILE *out = fopen( ( path_to_save_files + PlatformPATH_SEPARATOR + "out.cfg" ).c_str(), "wb" );
        /// if ( out ) {
        ///     ...
        ///     fclose(out);
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param rootFolder base path used to group several games
        /// \param gameName the game name
        /// \return true if its OK
        ///
        static std::string getSaveGamePath(const std::string &rootFolder, const std::string &gameName)
        {
#if defined(_WIN32)
            std::wstring base = StringUtil::string_to_WString(rootFolder);
            std::wstring game = StringUtil::string_to_WString(gameName);

            char resultChar[4096];

            getSaveGameDir(resultChar, 4096, base.c_str(), game.c_str());

            return std::string(resultChar);

#elif defined(__APPLE__) || defined(__linux__)

            std::string home = unixHomeDirectory();

            home += std::string("/.") + rootFolder;
            unixCheckPathCreation(home.c_str());

            home += std::string("/") + gameName;
            unixCheckPathCreation(home.c_str());

            return home;
#endif
        }

        /// \brief Get and create the application path based to a root folder and app name.
        ///
        /// In windows, the code gets the default documents folder.
        ///
        /// In windows, the path is commonly created at:
        /// C:\Users\username\Documents\[rootFolder]\[appName]
        ///
        /// In linux and similar systems, it gets the user home directory as base and creates a hidden folder starting with a dot.
        ///
        /// In unix, the path is commonly created at:
        /// /home/username/.[rootFolder]/[appName]
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroPlatform/aRibeiroPlatform.h>
        /// using namespace aRibeiro;
        ///
        /// std::string path_to_save_files = PlatformPath::getDocumentsPath( "Company Root Folder", "App Name" );
        /// ...
        /// // Save some data on the path
        /// FILE *out = fopen( ( path_to_save_files + PlatformPATH_SEPARATOR + "out.cfg" ).c_str(), "wb" );
        /// if ( out ) {
        ///     ...
        ///     fclose(out);
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param rootFolder base path used to group several apps
        /// \param appName the app name
        /// \return true if its OK
        ///
        static std::string getDocumentsPath(const std::string &rootFolder, const std::string &appName)
        {
#if defined(_WIN32)
            std::wstring base = StringUtil::string_to_WString(rootFolder);
            std::wstring app = StringUtil::string_to_WString(appName);

            char resultChar[4096];

            getDocumentsDir(resultChar, 4096, base.c_str(), app.c_str());

            return std::string(resultChar);
#elif defined(__APPLE__) || defined(__linux__)

            std::string home = unixHomeDirectory();

            home += std::string("/.") + rootFolder;
            unixCheckPathCreation(home.c_str());

            home += std::string( "/" ) + appName;
            unixCheckPathCreation(home.c_str());

            return home;
#endif
        }

        /// \brief Check if the path is a directory
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroPlatform/aRibeiroPlatform.h>
        /// using namespace aRibeiro;
        ///
        /// if ( PlatformPath::isDirectory( "directory to test" ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param path the path to test
        /// \return true if it is a directory
        ///
        static bool isDirectory(const std::string &path_)
        {
#if defined(_WIN32)
            std::wstring path_w;
            if (StringUtil::endsWith(path_, "\\") || StringUtil::endsWith(path_, "/"))
                path_w = StringUtil::string_to_WString(path_.substr(0, path_.length() - 1));
            else
                path_w = StringUtil::string_to_WString(path_);
            WIN32_FIND_DATAW findfiledata;
            HANDLE hFind = FindFirstFileW((LPCWSTR)path_w.c_str(), &findfiledata);
            if (hFind != INVALID_HANDLE_VALUE)
            {
                bool is_dir = (findfiledata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                FindClose(hFind);
                return is_dir;
            }
            else
                return false;
            //return PathIsDirectoryW(path_w.c_str()) == TRUE;
#elif defined(__APPLE__) || defined(__linux__)
            return unixIsDirectory(path_.c_str());
#endif
        }

        /// \brief Check if the path is a file
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroPlatform/aRibeiroPlatform.h>
        /// using namespace aRibeiro;
        ///
        /// if ( PlatformPath::isFile( "file to test" ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param path the path to test
        /// \return true if it is a file
        ///
        static bool isFile(const std::string & path_)
        {
#if defined(_WIN32)
            std::wstring path_w;
            if (StringUtil::endsWith(path_, "\\") || StringUtil::endsWith(path_, "/"))
                path_w = StringUtil::string_to_WString(path_.substr(0, path_.length() - 1));
            else
                path_w = StringUtil::string_to_WString(path_);
            WIN32_FIND_DATAW findfiledata;
            HANDLE hFind = FindFirstFileW((LPCWSTR)path_w.c_str(), &findfiledata);
            if (hFind != INVALID_HANDLE_VALUE)
            {
                bool is_file = (findfiledata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
                FindClose(hFind);
                return is_file;
            }
            else
                return false;
            //return PathFileExistsW(path_w.c_str()) && !isDirectory(path_);
#elif defined(__APPLE__) || defined(__linux__)
            return unixIsFile(path_.c_str());
#endif
        }

        /// \brief Split the path+file string into path file extension
        ///
        /// Make easy work with path input from main argument
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroPlatform/aRibeiroPlatform.h>
        /// using namespace aRibeiro;
        ///
        /// int main(int argc, char *argv[]) {
        ///     if ( argc < 2 )
        ///         return -1;
        ///     std::string filepath = std::string(argv[1]);
        ///
        ///     std::string folder, filename, filename_wo_ext, fileext;
        ///     PlatformPath::splitPathString(filepath, &folder, &filename, &filename_wo_ext, &fileext);
        ///
        ///     fprintf(stdout, "Folder: %s\n", folder.c_str());
        ///     fprintf(stdout, "Filename: %s\n", filename.c_str());
        ///     fprintf(stdout, "Filename W/O ext: %s\n", filename_wo_ext.c_str());
        ///     fprintf(stdout, "Filename ext: %s\n", fileext.c_str());
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _input the arg string from main parameter
        /// \param outFolder returns folder path
        /// \param outFilename returns filename with extension
        /// \param outFileWOExt returns filename without extension
        /// \param outFileExt returns extension from filename
        static void splitPathString(const std::string &_input, std::string *outFolder, std::string *outFilename, std::string *outFileWOExt, std::string *outFileExt)
        {
            // copy the parameter to use replace afterwards
            std::string input = _input;

            //
            // normalize path separator
            //
            std::replace(input.begin(), input.end(), '\\', ITKCommon::PATH_SEPARATOR[0]);
            std::replace(input.begin(), input.end(), '/', ITKCommon::PATH_SEPARATOR[0]);

            std::string folder, filename, filename_wo_ext, ext;
            size_t path_directory_index = input.find_last_of(ITKCommon::PATH_SEPARATOR[0]);
            if (path_directory_index == -1)
            {
                folder = ".";
                filename = input;
            }
            else
            {
                folder = input.substr(0, path_directory_index);
                filename = input.substr(path_directory_index + 1, input.size() - 1 - path_directory_index);
            }

            path_directory_index = filename.find_last_of('.');
            if (path_directory_index == -1)
            {
                filename_wo_ext = filename;
                ext = "";
            }
            else
            {
                filename_wo_ext = filename.substr(0, path_directory_index);
                ext = filename.substr(path_directory_index + 1, filename.size() - 1 - path_directory_index);
            }

            *outFolder = folder;
            *outFilename = filename;
            *outFileWOExt = filename_wo_ext;
            *outFileExt = ext;
        }

        // Always returns the path without the final slash.
        static std::string getAbsolutePath(const std::string &path_)
        {
#if defined(_WIN32)
            WCHAR fullFilename[MAX_PATH];
            std::wstring path_w = StringUtil::string_to_WString(path_);
            if (GetFullPathNameW(path_w.c_str(), MAX_PATH, fullFilename, nullptr) > 0) {
                std::string result = StringUtil::wString_to_String(fullFilename);
                if (StringUtil::endsWith(result, "\\"))
                    result = result.substr(0, result.length()-1);
                return result;
            }

            // return the path without resolve the absolute path
            char resolved_path[MAX_PATH*4];
            snprintf(resolved_path, MAX_PATH, "%s", path_.c_str());
            while (strlen(resolved_path) > 0 && 
                (resolved_path[strlen(resolved_path) - 1] == '/' ||
                resolved_path[strlen(resolved_path) - 1] == '\\') )
                resolved_path[strlen(resolved_path) - 1] = '\0';
            return resolved_path;

#elif defined(__APPLE__) || defined(__linux__)
            char resolved_path[PATH_MAX];
            if (realpath(path_.c_str(), resolved_path) != NULL) {
                if (strlen(resolved_path) > 0 && resolved_path[strlen(resolved_path) - 1] == '/')
                    resolved_path[strlen(resolved_path) - 1] = '\0';
                return resolved_path;
            }

            // return the path without resolve the absolute path
            snprintf(resolved_path, PATH_MAX, "%s", path_.c_str());
            while (strlen(resolved_path) > 0 && resolved_path[strlen(resolved_path) - 1] == '/')
                resolved_path[strlen(resolved_path) - 1] = '\0';
            return resolved_path;
#endif
        }
    };

}