#pragma once

#include "../platform_common.h"

#ifndef LLADDR
#define LLADDR
#endif

namespace Platform
{

    namespace Tool
    {

        /// \brief Query the MAC (Media Access Control) From the First Network Device Installed in the Computer
        ///
        /// This function could return an 6 bytes hardware address format.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// // result could be: 0xa0, 0xb1, 0xc2, 0xd3, 0xe4, 0xf5
        /// std::vector<unsigned char> mac = getFirstMacAddress();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        static std::vector<unsigned char> getFirstMacAddress()
        {
#if defined(_WIN32)

            PIP_ADAPTER_INFO AdapterInfo;
            DWORD dwBufLen = sizeof(AdapterInfo);

            std::vector<unsigned char> mac_addr;

            AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));

            // Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen     variable
            if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW)
            {
                free(AdapterInfo);
                AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
            }

            if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR)
            {
                PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to current adapter info
                do
                {
                    //if (pAdapterInfo->AddressLength == 6)
                    {
                        mac_addr.resize(pAdapterInfo->AddressLength);
                        memcpy(mac_addr.data(), pAdapterInfo->Address, pAdapterInfo->AddressLength);

                        printf("[%s:%i]\n", __FILE__, __LINE__);
                        printf("%s: ", pAdapterInfo->IpAddressList.IpAddress.String);
                        for (size_t i = 0; i < mac_addr.size(); i++){
                            if (i>0)
                                printf(":");
                            printf("%02x", mac_addr[i]);
                        }
                        printf("\n");

                        //printf("Found HW Address: ");
                        // printf("%02X:%02X:%02X:%02X:%02X:%02X of: ",
                        //        pAdapterInfo->Address[0], pAdapterInfo->Address[1],
                        //        pAdapterInfo->Address[2], pAdapterInfo->Address[3],
                        //        pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
                        // printf("%s", pAdapterInfo->IpAddressList.IpAddress.String);
                        // printf("\n");
                        break;
                    }

                    pAdapterInfo = pAdapterInfo->Next;
                } while (pAdapterInfo);
            }

            free(AdapterInfo);
            return mac_addr;

#elif defined(__linux__) || defined(__APPLE__)

            std::vector<unsigned char> mac_addr;

            struct ifaddrs *ifap, *ifaptr;
            unsigned char *ptr;

            if (getifaddrs(&ifap) == 0)
            {
                for (ifaptr = ifap; ifaptr != NULL; ifaptr = (ifaptr)->ifa_next)
                {
#if defined(__linux__)
                    if (((ifaptr)->ifa_addr)->sa_family == AF_PACKET)
#elif defined(__APPLE__)
                    if (((ifaptr)->ifa_addr)->sa_family == AF_LINK)
#endif
                    {

                        size_t len = strlen((ifaptr)->ifa_name);

                        if (len > 2)
                            len = 2;

                        if (memcmp((ifaptr)->ifa_name, "lo", len) == 0)
                            continue;


#if defined(__linux__)  
                        struct sockaddr_ll *addr_structure = (struct sockaddr_ll *)(ifaptr)->ifa_addr;
                        //ptr = (unsigned char *)LLADDR(addr_structure->sll_addr);
                        ptr = (unsigned char *)(addr_structure->sll_addr);
                        mac_addr.resize(addr_structure->sll_halen);
                        memcpy(mac_addr.data(), ptr, addr_structure->sll_halen);
#elif defined(__APPLE__)
                        struct sockaddr_dl *addr_structure = (struct sockaddr_dl *)(ifaptr)->ifa_addr;
                        ptr = (unsigned char *)LLADDR(addr_structure);
                        mac_addr.resize(addr_structure->sdl_alen);
                        memcpy(mac_addr.data(), ptr, addr_structure->sdl_alen);
#endif

                        printf("[%s:%i]\n", __FILE__, __LINE__);
                        printf("%s: ", (ifaptr)->ifa_name);
                        for (size_t i = 0; i < mac_addr.size(); i++){
                            if (i>0)
                                printf(":");
                            printf("%02x", mac_addr[i]);
                        }
                        printf("\n");

                        break;
                    }
                }
                freeifaddrs(ifap);
            }

            return mac_addr;

#endif
        }

    }

}
