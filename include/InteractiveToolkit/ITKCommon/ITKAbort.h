#pragma once

#include "../common.h"
#include "../EventCore/Event.h"

#include "../Platform/Mutex.h"
#include "../Platform/AutoLock.h"

namespace ITKCommon
{

    class ITKAbort
    {
        // std::recursive_mutex mtx;
        Platform::Mutex mutex;
        bool abort_triggered;
        ITKAbort()
        {
            abort_triggered = false;
        }

    public:
        EventCore::Event<void(const char *file, int line, const char *message)> OnAbort;

        void triggeredAbort(const char *file, int line, const char *format, ...)
        {
            // std::lock_guard<decltype(mtx)> lock(mtx);
            Platform::AutoLock lock(&mutex);

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
            char_buffer.resize(vsnprintf(nullptr, 0, format, args) + 1);
            va_end(args);

            va_start(args, format);
            int len = vsnprintf(char_buffer.data(), char_buffer.size(), format, args);
            va_end(args);

            // print abort reason
            fprintf(stderr, "[%s:%i]\n", file, line);
            fprintf(stderr, "%s\n", char_buffer.data());

            if (OnAbort != nullptr)
                OnAbort(file, line, char_buffer.data());
            exit(-1);
        }

        static ITK_INLINE ITKAbort *Instance()
        {
            static ITKAbort _ITKAbort;
            return &_ITKAbort;
        }
    };


    static inline std::string GenerateDebugStrFnc(const char *file, int line, const char *format, ...){
        
        std::vector<char> char_buffer;
        
        va_list args;

        va_start(args, format);
        char_buffer.resize(vsnprintf(nullptr, 0, format, args) + 1);
        va_end(args);

        va_start(args, format);
        int len = vsnprintf(char_buffer.data(), char_buffer.size(), format, args);
        va_end(args);

        std::vector<char> final_str;

        final_str.resize(snprintf(nullptr,0,"[%s:%i]\n%s", file, line, char_buffer.data()) + 1);

        snprintf(final_str.data(),final_str.size(),
            "[%s:%i]\n%s", 
            file, line, char_buffer.data());

        return final_str.data();
    }
}

#define ITK_ABORT(bool_exp, ...) \
    if (bool_exp)                \
        ITKCommon::ITKAbort::Instance()->triggeredAbort(__FILE__, __LINE__, __VA_ARGS__);

#define ITK_ABORT_FL(file, line, bool_exp, ...) \
    if (bool_exp)                               \
        ITKCommon::ITKAbort::Instance()->triggeredAbort(file, line, __VA_ARGS__);

#define ON_COND_SET_ERRORSTR_RETURN(cond, ret, ...) \
    if (cond) { \
        if (errorStr != nullptr) \
            *errorStr = ITKCommon::GenerateDebugStrFnc(__FILE__, __LINE__,__VA_ARGS__); \
        return ret; \
    }

#define ON_COND_SET_ERRORSTR_RETURN_FL(file,line,cond, ret, ...) \
    if (cond) { \
        if (errorStr != nullptr) \
            *errorStr = ITKCommon::GenerateDebugStrFnc(file,line,__VA_ARGS__); \
        return ret; \
    }

#include "../Platform/Mutex.inl"