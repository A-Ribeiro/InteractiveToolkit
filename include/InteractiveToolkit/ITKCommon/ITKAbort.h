#pragma once

#include "../common.h"
#include "../EventCore/Event.h"

namespace ITKCommon
{

    class ITKAbort
    {
        bool abort_triggered;
        ITKAbort()
        {
            abort_triggered = false;
        }

    public:
        EventCore::Event<void(const char *file, int line, const char *message)> OnAbort;

        void triggeredAbort(const char *file, int line, const char *format, ...)
        {
            if (abort_triggered)
            {
                fprintf(stderr, "ERROR: Called abort inside the abort event...\n");

                va_list args;
                fprintf(stderr, "[%s:%i]\n", file, line);
                va_start(args, format);
                vfprintf(stderr, format, args);
                va_end(args);
                fprintf(stderr, "\n");

                exit(-1);
                return;
            }

            abort_triggered = true;

            va_list args;

            std::vector<char> char_buffer;
            va_start(args, format);
            char_buffer.resize(vsnprintf(NULL, 0, format, args) + 1);
            va_end(args);

            va_start(args, format);
            int len = vsnprintf(char_buffer.data(), char_buffer.size(), format, args);
            va_end(args);

            if (OnAbort == nullptr)
            {
                fprintf(stderr, "[%s:%i]\n", file, line);
                fprintf(stderr, "%s\n", char_buffer.data());
            }
            else
                OnAbort(file, line, char_buffer.data());
            exit(-1);
        }

        static ITK_INLINE ITKAbort *Instance()
        {
            static ITKAbort _ITKAbort;
            return &_ITKAbort;
        }
    };

}

#define ITK_ABORT(bool_exp, ...) \
    if (bool_exp)                \
        ITKCommon::ITKAbort::Instance()->triggeredAbort(__FILE__, __LINE__, __VA_ARGS__);

#define ITK_ABORT_FL(file, line, bool_exp, ...) \
    if (bool_exp)                               \
        ITKCommon::ITKAbort::Instance()->triggeredAbort(file, line, __VA_ARGS__);
