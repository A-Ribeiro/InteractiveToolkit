#pragma once

#include "../../common.h"

#include "SocketUtils.h"

#include <regex>

#if defined(_WIN32)

// #elif defined(__linux__) || defined(__APPLE__)
#else

#include <netdb.h>
#include <arpa/inet.h>

#endif

#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

namespace Platform
{
    namespace SocketTools
    {

        static inline std::string urlEncode(const std::string &value)
        {
            std::vector<char> output;
            for (char c : value)
            {
                if (c == ' ')
                    output.push_back('+');
                else if (std::isalnum((uint8_t)c) || c == '-' || c == '_' || c == '.' || c == '~')
                    output.push_back(c);
                else
                {
                    char buf[4];
                    std::snprintf(buf, sizeof(buf), "%%%02X", (uint8_t)c);
                    output.insert(output.end(), buf, buf + 3);
                }
            }
            return std::string(output.begin(), output.end());
        }

        static inline std::string urlDecode(const std::string &value)
        {
            std::vector<char> decoded;
            for (size_t i = 0; i < value.length(); ++i)
            {
                if (value[i] == '%')
                {
                    if (i + 2 < value.length())
                    {
                        // Convert hex string to character
                        std::string hex = value.substr(i + 1, 2);
                        int ch;
                        if (sscanf(hex.c_str(), "%02x", &ch) == 1)
                            decoded.push_back((char)ch);
                        else
                            decoded.push_back('?');
                        i += 2;
                    }
                    else
                        decoded.push_back('?');
                }
                else if (value[i] == '+')
                    // Convert '+' to space (common in query strings)
                    decoded.push_back(' ');
                else
                    decoded.push_back(value[i]);
            }
            return std::string(decoded.begin(), decoded.end());
        }

        static inline uint32_t ipv4_address_to_nl(const std::string &ip_address)
        {
            uint32_t result;

            if (ip_address.size() == 0 || ip_address.compare("INADDR_ANY") == 0)
                result = htonl(INADDR_ANY);
            else if (ip_address.compare("INADDR_LOOPBACK") == 0)
                result = htonl(INADDR_LOOPBACK);
            else
                result = inet_addr(ip_address.c_str());

            return result;
        }

        static inline struct sockaddr_in mountAddress(const std::string &ip = "127.0.0.1", uint16_t port = NetworkConstants::PUBLIC_PORT_START)
        {
            struct sockaddr_in result = {0};

            result.sin_family = AF_INET;
            result.sin_addr.s_addr = ipv4_address_to_nl(ip);
            result.sin_port = htons(port);

            return result;
        }

        static inline bool isIPv4Address(const std::string &str)
        {
            // Simple regex for IPv4: 4 groups of 1-3 digits separated by dots
            std::regex ipv4_pattern(
                "^(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})$");

            std::smatch matches;
            if (!std::regex_match(str, matches, ipv4_pattern))
                return false;

            for (size_t i = 1; i < matches.size(); ++i)
            {
                int octet = std::stoi(matches[i].str());
                if (octet < 0 || octet > 255)
                    return false;
            }

            return true;
        }

        struct URL
        {
            std::string protocol; // http, https
            std::string hostname; // domain or IP
            uint16_t port;        // port number
            std::string path;     // path after hostname
            bool valid;
            bool ssl;

            URL() : port(0), path("/"), valid(false), ssl(false) {}

            URL(std::string protocol, // http, https
                std::string hostname, // domain or IP
                std::string path,     // path after hostname
                int port = -1)        // port number
            {
                this->protocol = protocol;
                this->hostname = hostname;
                this->path = path;

                this->ssl = (protocol != "http");

                if (port != -1)
                    this->port = (uint16_t)port;
                else
                {
                    if (this->protocol == "https")
                        this->port = 443;
                    else if (this->protocol == "http")
                        this->port = 80;
                    else
                        this->port = 443;
                }

                this->valid = true;
            }

            std::string toString() const
            {
                if (protocol.empty())
                {
                    if (port == 443)
                        return ITKCommon::PrintfToStdString("%s%s",
                                                            hostname.c_str(),
                                                            urlEncode(path).c_str());
                    else
                        return ITKCommon::PrintfToStdString("%s:%d%s",
                                                            hostname.c_str(),
                                                            port,
                                                            urlEncode(path).c_str());
                }
                else
                {
                    if (port == 443 && protocol == "https")
                        return ITKCommon::PrintfToStdString("%s://%s%s",
                                                            protocol.c_str(),
                                                            hostname.c_str(),
                                                            urlEncode(path).c_str());
                    else if (port == 80 && protocol == "http")
                        return ITKCommon::PrintfToStdString("%s://%s%s",
                                                            protocol.c_str(),
                                                            hostname.c_str(),
                                                            urlEncode(path).c_str());
                    else
                        return ITKCommon::PrintfToStdString("%s://%s:%d%s",
                                                            protocol.c_str(),
                                                            hostname.c_str(),
                                                            port,
                                                            urlEncode(path).c_str());
                }
            }

            static inline URL parse(const std::string &url)
            {
                URL result;

                // URL format: [protocol://]hostname[:port][/path]
                std::regex url_pattern("^(?:([^:]+)://)?([^:/]+)(?::(\\d+))?(/.*)?$");

                std::smatch matches;

                if (!std::regex_match(url, matches, url_pattern))
                    return result;

                result.protocol = matches[1].str();
                result.hostname = matches[2].str();

                // Default protocol to https if not specified
                if (result.protocol.empty())
                    result.protocol = "https";

                // Parse port
                if (matches[3].matched)
                    result.port = std::stoi(matches[3].str());
                else
                {
                    // Default ports based on protocol
                    if (result.protocol == "https")
                        result.port = 443;
                    else if (result.protocol == "http")
                        result.port = 80;
                    else
                        result.port = 443;
                }

                // Determine SSL based on protocol
                result.ssl = (result.protocol != "http");

                // Parse path (defaults to "/" if not specified)
                if (matches[4].matched)
                    result.path = urlDecode(matches[4].str());

                result.valid = true;
                
                return result;
            }
        };

        static inline std::string resolveHostname(const std::string &hostname_or_ip)
        {
            // If it's already an IPv4 address, return as-is
            if (isIPv4Address(hostname_or_ip))
                return hostname_or_ip;

            // Use gethostbyname to resolve hostname to IP
            struct hostent *host = gethostbyname(hostname_or_ip.c_str());
            if (host == nullptr)
            {
                printf("Failed to resolve hostname: %s\n", hostname_or_ip.c_str());
                return "";
            }

            // Convert to dotted decimal notation
            struct in_addr **addr_list = (struct in_addr **)host->h_addr_list;
            if (addr_list[0] != nullptr)
            {
                return inet_ntoa(*addr_list[0]);
            }

            return "";
        }
    }
}

#if defined(_WIN32)
#pragma warning(pop)
#endif
