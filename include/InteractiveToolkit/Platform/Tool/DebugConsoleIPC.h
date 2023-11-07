#pragma once

#include "../IPC/LowLatencyQueueIPC.h"

namespace Platform
{

    namespace Tool
    {

        class DebugConsoleIPC
        {

            std::vector<char> char_buffer;
            uint32_t mode;

        public:
            Platform::IPC::LowLatencyQueueIPC queue;

            DebugConsoleIPC(uint32_t mode = Platform::IPC::QueueIPC_WRITE, bool blocking = false) : queue("debug_console", mode, 1024, sizeof(char) * 64, blocking)
            {
                this->mode = mode;
            }

            void runReadLoop()
            {
                if ((mode & Platform::IPC::QueueIPC_READ) == 0)
                    return;

                ::printf("read main loop\n");
                fflush(stdin);

                ObjectBuffer buffer;

                while (true)
                {
                    while (queue.read(&buffer))
                    {
                        if (buffer.size > 0){
                            buffer.data[buffer.size-1] = 0;
                            ::printf("%s", (char *)buffer.data);
                            fflush(stdin);
                        }
                    }
                    if (queue.isSignaled())
                        return;
                    Platform::Sleep::millis(1);
                }
            }

            const char *printf(const char *format, ...)
            {

                if ((mode & Platform::IPC::QueueIPC_WRITE) == 0)
                    return "";

                va_list args;

                va_start(args, format);
                char_buffer.resize(vsnprintf(NULL, 0, format, args) + 1);
                va_end(args);

                va_start(args, format);
                int len = vsnprintf(&char_buffer[0], char_buffer.size(), format, args);
                va_end(args);

                if (char_buffer.size() == 0)
                    return "";

                queue.write((uint8_t *)&char_buffer[0], (uint32_t)char_buffer.size(), false);

                return &char_buffer[0];
            }
        };

    }

}