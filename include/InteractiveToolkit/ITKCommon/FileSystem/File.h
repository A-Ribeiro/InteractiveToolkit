#pragma once

#include "../../common.h"
#include "../../Platform/platform_common.h"
#include "../Date.h"
#include "../../EventCore/ExecuteOnScopeEnd.h"
#include "../Path.h"

#include "../../Platform/Core/ObjectBuffer.h"

#if defined(_WIN32)
#pragma warning( push )
#pragma warning( disable : 4996)
#endif

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

            // Will try to resolve the path with the OS.
            // If it fails, it will return the File representation 
            // of this path without fill the statx information
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
        

            FILE *fopen(const char* mode, std::string *errorStr = NULL){
                if (!isFile){
                    if (errorStr != NULL)
                        *errorStr = "The path is not a file";
                    return NULL;
                }
                return File::fopen(full_path.c_str(), mode, errorStr);
            }

            bool readContentToObjectBuffer(Platform::ObjectBuffer *output, std::string *errorStr = NULL) {
                FILE *file = fopen("rb", errorStr);
                if (!file)
                    return false;
                EventCore::ExecuteOnScopeEnd _close_source([=](){
                    fclose(file);
                });

                if (fseek(file, 0, SEEK_END) != 0){
                    if (errorStr != NULL)
                        *errorStr = strerror(errno);
                    return false;
                }

                int64_t _ftell = (int64_t)ftell(file);
                if (_ftell == -1){
                    if (errorStr != NULL)
                        *errorStr = strerror(errno);
                    return -1;
                }

                output->setSize(_ftell);

                if (fseek(file, 0, SEEK_SET) != 0){
                    if (errorStr != NULL)
                        *errorStr = strerror(errno);
                    return false;
                }

                int64_t readed_size = 0;
                int64_t offset = 0;
                
                while( offset < output->size &&
                    ( readed_size = (int64_t)fread( &output->data[offset], 
                        sizeof(uint8_t), (size_t)(output->size - offset),
                        file) ) > 0 ) {
                    offset += readed_size;
                }

                return true;
            }

            bool readContentToVector(std::vector<uint8_t> *output, std::string *errorStr = NULL) {
                FILE *file = fopen("rb", errorStr);
                if (!file)
                    return false;
                EventCore::ExecuteOnScopeEnd _close_source([=](){
                    fclose(file);
                });

                if (fseek(file, 0, SEEK_END) != 0){
                    if (errorStr != NULL)
                        *errorStr = strerror(errno);
                    return false;
                }

                int64_t _ftell = (int64_t)ftell(file);
                if (_ftell == -1){
                    if (errorStr != NULL)
                        *errorStr = strerror(errno);
                    return -1;
                }

                output->resize((size_t)_ftell);

                if (fseek(file, 0, SEEK_SET) != 0){
                    if (errorStr != NULL)
                        *errorStr = strerror(errno);
                    return false;
                }

                int64_t readed_size = 0;
                int64_t offset = 0;
                
                while( offset < (int64_t)output->size() &&
                    ( readed_size = (int64_t)fread( &output->at(offset),
                        sizeof(uint8_t), (size_t)((int64_t)output->size() - offset),
                        file) ) > 0 ) {
                    offset += readed_size;
                }

                return true;
            }

            // read the file size in bytes using fopen call
            // returns -1 in case of any error
            int64_t readContentSizeSafe(std::string *errorStr = NULL) {
                FILE *file = fopen("rb", errorStr);
                if (!file)
                    return -1;
                EventCore::ExecuteOnScopeEnd _close_source([=](){
                    fclose(file);
                });
                if (fseek(file, 0, SEEK_END) != 0){
                    if (errorStr != NULL)
                        *errorStr = strerror(errno);
                    return -1;
                }
                int64_t _ftell = (int64_t)ftell(file);
                if (_ftell == -1){
                    if (errorStr != NULL)
                        *errorStr = strerror(errno);
                    return -1;
                }
                return _ftell;
            }

            bool readContentToMemory(int64_t read_offset, uint8_t *output, int64_t output_size, std::string *errorStr = NULL) {
                FILE *file = fopen("rb", errorStr);
                if (!file)
                    return false;
                EventCore::ExecuteOnScopeEnd _close_source([=](){
                    fclose(file);
                });

                if (fseek(file, read_offset, SEEK_SET) != 0){
                    if (errorStr != NULL)
                        *errorStr = strerror(errno);
                    return false;
                }

                int64_t readed_size = 0;
                int64_t offset = 0;
                
                while( offset < output_size &&
                    ( readed_size = (int64_t)fread( &output[offset],
                        sizeof(uint8_t), (size_t)(output_size - offset),
                        file) ) > 0 ) {
                    offset += readed_size;
                }

                if (offset < output_size){
                    if (errorStr != NULL)
                        *errorStr = "Output buffer not filled completely";
                    return false;
                }

                return true;
            }

            bool writeContentFromObjectBuffer(const Platform::ObjectBuffer *input, bool append = false, std::string *errorStr = NULL) {
                return writeContentFromBinary(input->data, (int64_t)input->size, append, errorStr );
            }

            bool writeContentFromVector(const std::vector<uint8_t> *input, bool append = false, std::string *errorStr = NULL) {
                return writeContentFromBinary(input->data(), (int64_t)input->size(), append, errorStr );
            }

            bool writeContentFromBinary(const uint8_t *input, const int64_t _size, bool append = false, std::string *errorStr = NULL) {
                return File::WriteContentFromBinary(this->full_path.c_str(), input, _size, append, errorStr);
            }
            

            static bool WriteContentFromObjectBuffer(const char* filename, const Platform::ObjectBuffer *input, bool append = false, std::string *errorStr = NULL) {
                return WriteContentFromBinary(filename, input->data, (int64_t)input->size, append, errorStr );
            }

            static bool WriteContentFromVector(const char* filename, const std::vector<uint8_t> *input, bool append = false, std::string *errorStr = NULL) {
                return WriteContentFromBinary(filename, input->data(), (int64_t)input->size(), append, errorStr );
            }

            static bool WriteContentFromBinary(const char* filename, const uint8_t *input, const int64_t _size, bool append = false, std::string *errorStr = NULL) {
                FILE *file;
                if (append)
                    file = File::fopen(filename, "ab", errorStr);
                else
                    file = File::fopen(filename, "wb", errorStr);
                if (!file)
                    return false;
                EventCore::ExecuteOnScopeEnd _close_source([=](){
                    fclose(file);
                });
                if (_size > 0){
                    int64_t written_size = 0;
                    int64_t offset = 0;
                    while( offset < _size &&
                        ( written_size = (int64_t)fwrite( &input[offset],
                            sizeof(uint8_t), (size_t)(_size - offset), 
                            file) ) > 0 ) {
                        offset += written_size;
                    }
                }
                return true;
            }


            static bool touch(const char* filename, std::string *errorStr = NULL) {
                FILE *fileTouched = File::fopen(filename, "ab", errorStr);
                if (!fileTouched)
                    return false;
                return File::fclose(fileTouched, errorStr);
            }

            // work on windows or linux
            // mode from std::fopen
            // must call fclose if is != NULL
            static FILE * fopen(const char* filename, const char* mode, std::string *errorStr = NULL){
#if defined(_WIN32)
                FILE * result = _wfopen( ITKCommon::StringUtil::string_to_WString(filename).c_str(), ITKCommon::StringUtil::string_to_WString(mode).c_str() );
#elif defined(__linux__) || defined(__APPLE__)
                FILE * result = ::fopen(filename, mode);
#endif
                if (errorStr != NULL && !result)
                    *errorStr = strerror(errno);
                return result;
            }

            static bool fclose(FILE *file, std::string *errorStr = NULL) {
                if (::fclose(file) != 0){
                    if (errorStr != NULL)
                        *errorStr = strerror(errno);
                    return false;
                }
                return true;
            }

            static bool rename(const char* src_filename, const char* dst_filename, std::string *errorStr = NULL) {
#if defined(_WIN32)
                std::wstring _wstr_src = ITKCommon::StringUtil::string_to_WString(src_filename);
                std::wstring _wstr_dst = ITKCommon::StringUtil::string_to_WString(dst_filename);
                if (MoveFileW(_wstr_src.c_str(), _wstr_dst.c_str()) == FALSE) {
                    if (errorStr != NULL)
                        *errorStr = ITKPlatformUtil::getLastErrorMessage();
                    return false;
                }
                return true;
#elif defined(__linux__) || defined(__APPLE__)
                if (::rename(src_filename, dst_filename) != 0) {
                    if (errorStr != NULL)
                        *errorStr = strerror(errno);
                    return false;
                }
                return true;
#endif
            }

            static bool move(const char* src_filename, const char* dst_filename, std::string *errorStr = NULL) {
                return File::rename(src_filename, dst_filename, errorStr);
            }

            static bool copy(const char* src_filename, const char* dst_filename, std::string *errorStr = NULL) {
                FILE* source = File::fopen(src_filename, "rb", errorStr);
                if (!source)
                    return false;
                EventCore::ExecuteOnScopeEnd _close_source([=](){
                    fclose(source);
                });

                FILE* dest = File::fopen(dst_filename, "wb", errorStr);
                if (!dest)
                    return false;
                EventCore::ExecuteOnScopeEnd _close_dest([=](){
                    fclose(dest);
                });

                char buf[BUFSIZ];
                size_t size;
                while (size = fread(buf, 1, BUFSIZ, source)) {
                    fwrite(buf, 1, size, dest);
                }
                
                return true;
            }

            static bool remove(const char* filename, std::string *errorStr = NULL) {
#if defined(_WIN32)
                std::wstring _wstr = ITKCommon::StringUtil::string_to_WString(filename);
                if (DeleteFileW(_wstr.c_str()) == FALSE) {
                    if (errorStr != NULL)
                        *errorStr = ITKPlatformUtil::getLastErrorMessage();
                    return false;
                }
                return true;
#elif defined(__linux__) || defined(__APPLE__)
                if (::remove(filename) != 0) {
                    if (errorStr != NULL)
                        *errorStr = strerror(errno);
                    return false;
                }
                return true;
#endif
            }

        
        };

    }
}

#if defined(_WIN32)
#pragma warning( pop )
#endif
