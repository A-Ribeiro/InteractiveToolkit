#pragma once

#include "../common.h"
#include "../AlgorithmCore/PatternMatch/array_index_of.h"

// #include <locale>
// #include <codecvt>
#include "3rdparty/utf8proc/header-only/utf8proc.h"
#include "3rdparty/utf8proc/header-only/utf16proc.inl"

#define UTF32_STR(utf32_literal) ITKCommon::StringUtil::utf32_to_utf8(utf32_literal).c_str()

namespace ITKCommon
{
    /// \brief Common string operation for use with the font renderer and generic string tests
    ///
    /// \author Alessandro Ribeiro
    ///
    class StringUtil
    {
        // private copy constructores, to avoid copy...
        StringUtil(const StringUtil &v) {}
        void operator=(const StringUtil &v) {}

    public:
        StringUtil() {}

        std::vector<char> char_buffer;     ///< Result of the use of the #StringUtil::printf method
        std::vector<wchar_t> wchar_buffer; ///< Result of the use of the #StringUtil::wprintf method

        /// \brief Get the RAW char string pointer
        ///
        /// \author Alessandro Ribeiro
        /// \return pointer to the result of the #StringUtil::printf method
        ///
        ITK_INLINE const char *char_ptr() const
        {
            if (char_buffer.size() == 0)
                return NULL;
            return &char_buffer[0];
        }

        /// \brief Get the RAW wchar_t string pointer
        ///
        /// \author Alessandro Ribeiro
        /// \return pointer to the result of the #StringUtil::wprintf method
        ///
        ITK_INLINE const wchar_t *wchar_ptr() const
        {
            if (wchar_buffer.size() == 0)
                return NULL;
            return &wchar_buffer[0];
        }

        /// \brief Wrapper of the stdio printf to work with std::string type
        ///
        /// It is possible to use it to generate a string that will be rendered as font glyphs.
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float fps;
        /// StringUtil stringUtil;
        ///
        /// const char* output_char_string = stringUtil.printf("fps: %.2f", fps);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param format stdio printf like input string format descriptor
        /// \param ... va_list args
        /// \return char pointer to the generated string
        ///
        ITK_INLINE const char *printf(const char *format, ...)
        {

            va_list args;

            va_start(args, format);
            char_buffer.resize(vsnprintf(NULL, 0, format, args) + 1);
            va_end(args);

            va_start(args, format);
            int len = vsnprintf(&char_buffer[0], char_buffer.size(), format, args);
            va_end(args);

            return char_ptr();
        }

        /// \brief Wrapper of the stdio wprintf to work with std::wstring type
        ///
        /// It is possible to use it to generate a string that will be rendered as font glyphs.
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float fps;
        /// StringUtil stringUtil;
        ///
        /// const wchar_t* output_wchar_string = stringUtil.wprintf(L"fps: %.2f", fps);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param format stdio wprintf like input string format descriptor
        /// \param ... va_list args
        /// \return wchar_t pointer to the generated string
        ///
        ITK_INLINE const wchar_t *wprintf(const wchar_t *format, ...)
        {
            va_list args;

            va_start(args, format);
            wchar_buffer.resize(vswprintf(NULL, 0, format, args) + 1);
            va_end(args);

            va_start(args, format);
            int len = vswprintf(&wchar_buffer[0], wchar_buffer.size(), format, args);
            va_end(args);

            return wchar_ptr();
        }

        /// \brief Convert any char string to wchar_t string
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// std::string input;
        ///
        /// std::wstring output = StringUtil::toWString(input);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param str char string
        /// \return wchar_t string
        ///
        // static ITK_INLINE std::wstring toWString(const std::string &str)
        // {
        //     //return std::wstring(str.begin(), str.end());
        //     using convert_typeX = std::codecvt_utf8<wchar_t>;
        //     std::wstring_convert<convert_typeX, wchar_t> converterX;

        //     return converterX.from_bytes(str);
        // }

        // static ITK_INLINE std::string toString(const std::wstring &wstr)
        // {
        //     //return std::string(wstr.begin(), wstr.end());
        //     using convert_typeX = std::codecvt_utf8<wchar_t>;
        //     std::wstring_convert<convert_typeX, wchar_t> converterX;

        //     return converterX.to_bytes(wstr);
        // }

        static ITK_INLINE std::u32string utf8_to_utf32(const std::string &str)
        {
            // return std::wstring(str.begin(), str.end());

            // using convert_typeX = std::codecvt_utf8<char32_t>;
            // std::wstring_convert<convert_typeX, char32_t> converterX;
            // return converterX.from_bytes(str);
            std::vector<char32_t> output;
            utf8proc_ssize_t input_total_bytes = (utf8proc_ssize_t)str.length();
            if (input_total_bytes == 0)
                return U"";
            const utf8proc_uint8_t *input_const_ptr = (const utf8proc_uint8_t *)&str[0];
            utf8proc_ssize_t total_bytes_readed = 0;

            utf8proc_int32_t readed_char_utf32;
            utf8proc_ssize_t readed_bytes;
            while (total_bytes_readed < input_total_bytes)
            {
                readed_bytes = utf8proc_iterate(
                    input_const_ptr + total_bytes_readed,
                    input_total_bytes - total_bytes_readed,
                    &readed_char_utf32);
                if (readed_bytes <= 0)
                {
                    // error occured on reading UTF8 Data
                    fprintf(stderr, "[ITKCommon::StringUtil] error to read utf-8 data.\n");
                    break;
                }
                total_bytes_readed += readed_bytes;
                // valid codepoint readed
                if (readed_char_utf32 >= 0)
                    output.push_back((char32_t)readed_char_utf32);
            }
            return std::u32string(output.data(), output.size());
        }

        static ITK_INLINE std::string utf32_to_utf8(const std::u32string &str)
        {
            // return std::wstring(str.begin(), str.end());

            // using convert_typeX = std::codecvt_utf8<char32_t>;
            // std::wstring_convert<convert_typeX, char32_t> converterX;
            // return converterX.to_bytes(str);

            std::vector<char> output;

            utf8proc_uint8_t dst[4];
            utf8proc_ssize_t bytes_written;
            for (auto _input_codepoint : str)
            {
                bytes_written = utf8proc_encode_char((utf8proc_int32_t)_input_codepoint, dst);
                if (bytes_written == 0)
                    continue;
                output.insert(output.end(), (char *)dst, (char *)(dst + bytes_written));
            }

            return std::string(output.data(), output.size());
        }

        static ITK_INLINE std::u32string utf16_to_utf32(const std::u16string &str)
        {
            std::vector<char32_t> output;
            size_t input_total_bytes = str.length();
            if (input_total_bytes == 0)
                return U"";
            const uint16_t *input_const_ptr = (const uint16_t *)&str[0];
            size_t total_bytes_readed = 0;

            uint32_t readed_char_utf32;
            size_t readed_bytes;
            while (total_bytes_readed < input_total_bytes)
            {
                readed_bytes = utf16proc_iterate(
                    input_const_ptr + total_bytes_readed,
                    input_total_bytes - total_bytes_readed,
                    &readed_char_utf32);
                if (readed_bytes <= 0)
                {
                    // error occured on reading UTF8 Data
                    fprintf(stderr, "[ITKCommon::StringUtil] error to read utf-16 data.\n");
                    break;
                }
                total_bytes_readed += readed_bytes;
                // valid codepoint readed
                if (readed_char_utf32 != 0)
                    output.push_back((char32_t)readed_char_utf32);
            }
            return std::u32string(output.data(), output.size());
        }

        static ITK_INLINE std::u16string utf32_to_utf16(const std::u32string &str)
        {
            std::vector<char16_t> output;

            uint16_t dst[2];
            size_t elements_written;
            for (auto _input_codepoint : str)
            {
                elements_written = utf16proc_encode_char((uint32_t)_input_codepoint, dst);
                if (elements_written == 0)
                    continue;
                output.insert(output.end(), (char16_t *)dst, (char16_t *)(dst + elements_written));
            }

            return std::u16string(output.data(), output.size());
        }

        static ITK_INLINE std::string wString_to_String(const std::wstring &str)
        {
            if (sizeof(wchar_t) == 2)
            {
                auto u32 = ITKCommon::StringUtil::utf16_to_utf32((char16_t *)str.c_str());
                return ITKCommon::StringUtil::utf32_to_utf8(u32);
            }
            else
                return ITKCommon::StringUtil::utf32_to_utf8((char32_t *)str.c_str());
        }

        static ITK_INLINE std::wstring string_to_WString(const std::string &str)
        {
            if (sizeof(wchar_t) == 2)
            {
                auto u32 = ITKCommon::StringUtil::utf8_to_utf32(str);
                return (wchar_t *)ITKCommon::StringUtil::utf32_to_utf16(u32).c_str();
            }
            else
                return (wchar_t *)ITKCommon::StringUtil::utf8_to_utf32(str).c_str();
        }

        /// \brief test if a string starts with another string
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// std::string input;
        ///
        /// if ( StringUtil::startsWith(input, "prefix") ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param str input string
        /// \param prefix string to check if str starts with
        /// \return true if the str starts with prefix
        ///
        static ITK_INLINE bool startsWith(const std::string &str, const std::string &prefix)
        {
            return ((prefix.size() <= str.size()) &&
                    std::equal(prefix.begin(), prefix.end(), str.begin()));
        }

        /// \brief test if a string ends with another string
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// std::string input;
        ///
        /// if ( StringUtil::endsWith(input, ".png") ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param str input string
        /// \param sufix string to check if str ends with
        /// \return true if the str ends with sufix
        ///
        static ITK_INLINE bool endsWith(const std::string &str, const std::string &sufix)
        {
            return ((sufix.size() <= str.size()) &&
                    std::equal(sufix.begin(), sufix.end(), str.begin() + (str.size() - sufix.size())));
        }

        static ITK_INLINE std::string toLower(const std::string &str)
        {
            // std::string aux = str;
            // std::transform(aux.begin(), aux.end(), aux.begin(), ::tolower);
            // return aux;
            std::u32string _upper = utf8_to_utf32(str);
            for (auto &c : _upper)
                c = utf8proc_tolower(c);
            return utf32_to_utf8(_upper);
        }
        static ITK_INLINE std::string toUpper(const std::string &str)
        {
            // std::string aux = str;
            // std::transform(aux.begin(), aux.end(), aux.begin(), ::toupper);
            // return aux;
            std::u32string _upper = utf8_to_utf32(str);
            for (auto &c : _upper)
                c = utf8proc_toupper(c);
            return utf32_to_utf8(_upper);
        }

        static ITK_INLINE std::string removeAccents(const std::string &str)
        {
            std::u32string u32_str = utf8_to_utf32(str);

            utf8proc_int32_t decomposed[4];
            utf8proc_option_t options = (utf8proc_option_t)(UTF8PROC_DECOMPOSE);
            int boundclass = UTF8PROC_BOUNDCLASS_START;

            for (auto &c : u32_str)
            {
                utf8proc_ssize_t size = utf8proc_decompose_char(c, decomposed, 4, options, &boundclass);
                if (size == 2)
                    c = decomposed[0];
            }

            return utf32_to_utf8(u32_str);
        }

        static ITK_INLINE bool contains(const std::string &str, const std::string &v)
        {
            return strstr(str.c_str(), v.c_str()) != 0;
        }

        static ITK_INLINE std::vector<std::string> tokenizer(const std::string &input, const std::string &delimiter)
        {

            int start = 0;

            const uint8_t *input_str = (const uint8_t *)input.c_str();
            int input_size = (int)input.length();
            const uint8_t *delimiter_str = (const uint8_t *)delimiter.c_str();
            int delimiter_size = (int)delimiter.length();

            std::vector<std::string> result;
            std::string result_str;
            while (start < input_size)
            {
                int index_of_delimiter = AlgorithmCore::PatternMatch::array_index_of(input_str, start, input_size, delimiter_str, delimiter_size);
                if (index_of_delimiter == start)
                {
                    start += delimiter_size;
                    result.push_back("");
                    continue;
                }
                int delta = index_of_delimiter - start;
                result_str.resize(delta);
                // for (int i = start, count = 0; i < index_of_delimiter; i++, count++)
                //     result_str[count] = (char)input_str[i];
                memcpy(&result_str[0], &input_str[start], delta * sizeof(uint8_t));
                result.push_back(result_str);
                start = index_of_delimiter + delimiter_size;
            }

            if (start == input_size){
                result.push_back("");
            }

            if (result.size() == 0)
                result.push_back("");

            return result;
        }

        static ITK_INLINE void replaceAll(std::string *data, const std::string &toSearch, const std::string &replaceStr)
        {
            // Get the first occurrence
            size_t pos = data->find(toSearch);
            // Repeat till end is reached
            while (pos != std::string::npos)
            {
                // Replace this occurrence of Sub String
                data->replace(pos, toSearch.size(), replaceStr);
                // Get the next occurrence from the current position
                pos = data->find(toSearch, pos + replaceStr.size());
            }
        }
        static ITK_INLINE std::string trim(const std::string &_str)
        {
            // return std::regex_replace(str, std::regex("(^[ \n]+)|([ \n]+$)"),"");
            // std::string::const_iterator it = str.begin();
            // while (it != str.end() && std::isspace(*it))
            //     it++;

            // std::string::const_reverse_iterator rit = str.rbegin();
            // while (rit.base() != it && std::isspace(*rit))
            //     rit++;

            // return std::string(it, rit.base());

            std::u32string str_32 = StringUtil::utf8_to_utf32(_str);

            auto it = str_32.begin();
            while (it != str_32.end() && 
                (*it == U' ' || *it == U'\n' || *it == U'\r' || *it == U'\b' || *it == U'\t')
            )
                it++;

            auto rit = str_32.rbegin();
            while (rit.base() != it && 
                (*rit == U' ' || *rit == U'\n' || *rit == U'\r' || *rit == U'\b' || *rit == U'\t')
            )
                rit++;

            return StringUtil::utf32_to_utf8(std::u32string(it, rit.base()));
        }

        static ITK_INLINE std::string unquote_cmd(const std::string &str)
        {
            // return std::string("\"") + std::regex_replace(str, std::regex("[\\\\\"]"), "\\$&") + "\"";
            std::string result = str;
            
            if (result.length() >= 2) {
                if (result[0] == '\"')
                    result = result.substr(1,result.length()-1);
                if (result[result.length()-1] == '\"')
                    result = result.substr(0,result.length()-1);
            }

            StringUtil::replaceAll(&result, "\\\"", "\"");
            StringUtil::replaceAll(&result, "\\\\", "\\");
            //result = std::string("\"") + result + "\"";
            
            return result;
        }

        static ITK_INLINE std::string quote_cmd(const std::string &str)
        {
            // return std::string("\"") + std::regex_replace(str, std::regex("[\\\\\"]"), "\\$&") + "\"";
            std::string result = str;
            StringUtil::replaceAll(&result, "\\", "\\\\");
            StringUtil::replaceAll(&result, "\"", "\\\"");
            result = std::string("\"") + result + "\"";
            return result;
        }

        static ITK_INLINE std::vector<std::string> parseArgv(const std::string &_cmd)
        {
            std::string cmd = trim(_cmd);

            std::vector<std::string> result;
            enum states
            {
                normal,
                enter_string,
                include_next_str,

                enter_string_concatenate,
                include_next_str_concatenate,

                normal_slash
            };
            states state = normal;
            std::string aux = "";
            std::string aux_string_concatenate = "";
            for (int i = 0; i < cmd.size(); i++)
            {
                char chr = cmd[i];
                char next_chr = cmd[i];
                if (i < (int)cmd.size() - 1)
                    next_chr = cmd[i + 1];
                else 
                    next_chr = 0;

                if (state == normal_slash)
                {
                    state = normal;
                    aux += chr;
                }
                else if (state == normal && chr == '\\')
                {
                    state = normal_slash;
                    //aux += chr;
                }
                else if (state == normal && chr == '"')
                {
                    // string opening...
                    aux = trim(aux);
                    if (aux.size() > 0)
                    {
                        //aux += chr;
                        aux_string_concatenate = "";
                        aux_string_concatenate += chr;
                        state = enter_string_concatenate;
                    }
                    else
                    {
                        // result.push_back(aux);
                        state = enter_string;
                        aux = "\"";
                    }
                }
                else if (state == normal && chr == ' ')
                {
                    // one new command
                    aux = trim(aux);
                    if (aux.size() > 0){
                        result.push_back(aux);
                        aux = "";
                    }
                }
                else if ((state == enter_string) && 
                    ((chr == '\\' && next_chr == '"') ||
                    (chr == '\\' && next_chr == '\\')))
                {
                    // skip slash for inner string
                    aux += chr;
                    state = include_next_str;
                }
                else if (state == include_next_str)
                {
                    // back to enter string
                    state = enter_string;
                    aux += chr;
                }
                else if ((state == enter_string_concatenate) && 
                    ((chr == '\\' && next_chr == '"') ||
                    (chr == '\\' && next_chr == '\\')))
                {
                    // skip slash for inner string
                    aux_string_concatenate += chr;
                    state = include_next_str_concatenate;
                }
                else if (state == include_next_str_concatenate)
                {
                    // back to enter string
                    state = enter_string_concatenate;
                    aux_string_concatenate += chr;
                }
                else if (state == enter_string && chr == '"')
                {
                    // exit string
                    state = normal;
                    aux += "\"";
                    result.push_back( StringUtil::unquote_cmd(aux) );
                    aux = "";
                }
                else if (state == enter_string_concatenate && chr == '"')
                {
                    // exit string concatenate
                    //aux += chr;
                    aux_string_concatenate += chr;

                    aux += unquote_cmd(aux_string_concatenate);

                    state = normal;

                    //aux = trim(aux);
                    // result.push_back(aux);
                    // aux = "";
                }
                else if (state == enter_string || state == normal)
                {
                    aux += chr;
                } else if (state == enter_string_concatenate){
                    aux_string_concatenate += chr;
                }
            }

            aux = trim(aux);
            if (aux.size() > 0)
            {
                result.push_back(aux);
                aux = "";
            }

            // ::printf("[StringUtil]Parsing ARGV:\n");
            // for(size_t i=0;i<result.size();i++){
            //     ::printf("    %i -> '%s'\n", (int)i, result[i].c_str());
            // }

            return result;
        }

        static ITK_INLINE std::string argvToCmd(const std::vector<std::string> &argv)
        {
            std::string result;

            for (size_t i = 0; i < argv.size(); i++)
            {
                if (result.size() > 0)
                    result += " ";

                std::string cmd = trim(argv[i]);
                if (cmd.size() == 0) // empty string
                    result += "\"\"";
                else if (cmd.find(" ") != std::string::npos) // found space, need to quote the string
                    result += quote_cmd(cmd);
                else // normal string
                    result += cmd;
            }

            return result;
        }
    };

}
