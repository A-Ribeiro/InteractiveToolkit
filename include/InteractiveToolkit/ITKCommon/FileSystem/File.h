#pragma once

#include "../../common.h"
#include "../../Platform/platform_common.h"
#include "../Date.h"

namespace ITKCommon
{
    namespace FileSystem
    {

        class File
        {
        public:
            std::string base_path;
            std::string full_path;
            std::string name;
            bool isDirectory;
            bool isFile;

            Date creationTime;
            Date lastWriteTime;

            uint64_t size;

            File()
            {
                isDirectory = false;
                isFile = false;
                size = UINT64_C(0);
            }

            static File FromPath(const std::string &_path)
            {
                File result;

                result.full_path = ITKCommon::Path::getAbsolutePath(_path);
#if defined(_WIN32)
                ITKCommon::StringUtil::replaceAll(&result.full_path, "\\", "/");
#endif
                if (ITKCommon::StringUtil::endsWith(result.full_path, "/"))
                    result.full_path = result.full_path.substr(0, result.full_path.length() - 1);
                {
                    auto tokens = ITKCommon::StringUtil::tokenizer(result.full_path, "/");
                    if (tokens.size() > 0)
                        result.name = tokens[tokens.size() - 1];
                }
                result.base_path = result.full_path.substr(0, result.full_path.length() - result.name.length());

                // result.isDirectory = false;
                // result.isFile = false;

#if defined(_WIN32)
                std::wstring path_w;
                if (StringUtil::endsWith(result.full_path, "/"))
                    path_w = StringUtil::string_to_WString(result.full_path.substr(0, result.full_path.length() - 1));
                else
                    path_w = StringUtil::string_to_WString(result.full_path);
                WIN32_FIND_DATAW findfiledata;
                HANDLE hFind = FindFirstFileW((LPCWSTR)path_w.c_str(), &findfiledata);
                if (hFind != INVALID_HANDLE_VALUE)
                {
                    result.isDirectory = (findfiledata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                    result.isFile = !result.isDirectory;
                    // fileInfo.name = ITKCommon::StringUtil::wString_to_String(findfiledata.cFileName);
                    // fileInfo.full_path = fileInfo.base_path + fileInfo.name;
                    if (result.isDirectory)
                        result.full_path += "/";

                    // date processing
                    SYSTEMTIME stUTC; //, stLocal;
                    FileTimeToSystemTime(&findfiledata.ftLastWriteTime, &stUTC);
                    // SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
                    result.lastWriteTime = Date::FromSystemTime_win32(stUTC);

                    FileTimeToSystemTime(&findfiledata.ftCreationTime, &stUTC);
                    // SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
                    result.creationTime = Date::FromSystemTime_win32(stUTC);

                    result.size =
                        ((uint64_t)findfiledata.nFileSizeHigh << 32) | (uint64_t)findfiledata.nFileSizeLow & UINT64_C(0xffffffff);

                    FindClose(hFind);
                }
#elif defined(__APPLE__) || defined(__linux__)

                // stat directory
                {
                    struct statx sb;
                    int dirfd = AT_FDCWD;
                    // unsigned int mask = STATX_ALL;
                    unsigned int mask = STATX_MODE | STATX_MTIME | STATX_SIZE | STATX_BTIME;
                    int flags = AT_SYMLINK_NOFOLLOW;
                    bool stat_success = statx(dirfd, result.full_path.c_str(), flags, mask, &sb) == 0;
                    if (stat_success)
                    {
                        result.isDirectory = sb.stx_mode & S_IFDIR;
                        result.isFile = !result.isDirectory;

                        if (result.isDirectory)
                            result.full_path += "/";

                        // date processing
                        result.lastWriteTime = Date::FromUnixTimestampUTC(
                            sb.stx_mtime.tv_sec,
                            sb.stx_mtime.tv_nsec);

                        result.creationTime = Date::FromUnixTimestampUTC(
                            sb.stx_btime.tv_sec,
                            sb.stx_btime.tv_nsec);

                        result.size = (uint64_t)sb.stx_size;
                    }
                }
#endif

                if (result.isDirectory && !ITKCommon::StringUtil::endsWith(result.full_path, "/"))
                    result.full_path += "/";

                return result;
            }
        };

    }
}
