#pragma once

#include "../common.h"

namespace ITKCommon
{

#if defined(_WIN32)

    namespace Path
    {

        static ITK_INLINE std::string getWorkingPath()
        {
            auto ptr = _getcwd(NULL, 0);
            if (!ptr)
                return "";
            std::string result = ptr;
            free(ptr);
            return result;
        }

        static bool setWorkingPath(const std::string &path)
        {
            return _chdir(path.c_str()) == 0;
        }

        static std::string getExecutablePath(const std::string &arg0)
        {
            size_t path_directory_index = arg0.find_last_of('\\');
            return arg0.substr(0, path_directory_index);
        }
    }

#elif defined(__APPLE__) || defined(__linux__)

    namespace Path
    {

        static ITK_INLINE std::string getWorkingPath()
        {
            auto ptr = getcwd(NULL, 0);
            if (!ptr)
                return "";
            std::string result = ptr;
            free(ptr);
            return result;
        }

        static ITK_INLINE bool setWorkingPath(const std::string &path)
        {
            return chdir(path.c_str()) == 0;
        }

        static ITK_INLINE std::string getExecutablePath(const std::string &arg0)
        {
            size_t path_directory_index = arg0.find_last_of('/');
            auto str = arg0.substr(0, path_directory_index);
            auto ptr = realpath(str.c_str(), NULL);
            if (!ptr)
                return "";
            str = ptr;
            free(ptr);
            return str;
        }
    }

#else

#error Platform not supported...

#endif

}