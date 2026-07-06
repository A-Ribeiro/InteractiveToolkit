#pragma once

#include "../common.h"

namespace ITKCommon
{
    namespace ByteUtils
    {
        static ITK_INLINE double byteToKilo(uint64_t byte)
        {
            return (double)byte / 1024.0;
        }
        static ITK_INLINE double byteToMega(uint64_t byte)
        {
            return (double)byte / 1048576.0;
        }
        static ITK_INLINE double byteToGiga(uint64_t byte)
        {
            return (double)byte / 1073741824.0;
        }
        static ITK_INLINE double byteToTera(uint64_t byte)
        {
            return byteToGiga(byte) / 1024.0;
        }
        static ITK_INLINE double byteToPeta(uint64_t byte)
        {
            return byteToGiga(byte) / 1048576.0;
        }
        static ITK_INLINE double byteToExtra(uint64_t byte)
        {
            return byteToGiga(byte) / 1073741824.0;
        }

        static ITK_INLINE std::string byteSmartPrint(uint64_t byte, const char *unit)
        {
            uint64_t check_zero = byte;

            check_zero >>= 10;
            if (!check_zero)
                return ITKCommon::PrintfToStdString("%g %s", std::round((double)byte * 100.0) / 100.0, unit);
            check_zero >>= 10;
            if (!check_zero)
                return ITKCommon::PrintfToStdString("%g K%s", std::round(byteToKilo(byte) * 100.0) / 100.0, unit);
            check_zero >>= 10;
            if (!check_zero)
                return ITKCommon::PrintfToStdString("%g M%s", std::round(byteToMega(byte) * 100.0) / 100.0, unit);
            check_zero >>= 10;
            if (!check_zero)
                return ITKCommon::PrintfToStdString("%g G%s", std::round(byteToGiga(byte) * 100.0) / 100.0, unit);
            check_zero >>= 10;
            if (!check_zero)
                return ITKCommon::PrintfToStdString("%g T%s", std::round(byteToTera(byte) * 100.0) / 100.0, unit);
            check_zero >>= 10;
            if (!check_zero)
                return ITKCommon::PrintfToStdString("%g P%s", std::round(byteToPeta(byte) * 100.0) / 100.0, unit);
            return ITKCommon::PrintfToStdString("%g E%s", std::round(byteToExtra(byte) * 100.0) / 100.0, unit);
        }

        static ITK_INLINE double bitToKilo(uint64_t byte)
        {
            return (double)byte / 1000.0;
        }
        static ITK_INLINE double bitToMega(uint64_t byte)
        {
            return (double)byte / 1000000.0;
        }
        static ITK_INLINE double bitToGiga(uint64_t byte)
        {
            return (double)byte / 1000000000.0;
        }
        static ITK_INLINE double bitToTera(uint64_t byte)
        {
            return bitToGiga(byte) / 1000.0;
        }
        static ITK_INLINE double bitToPeta(uint64_t byte)
        {
            return bitToGiga(byte) / 1000000.0;
        }
        static ITK_INLINE double bitToExtra(uint64_t byte)
        {
            return bitToGiga(byte) / 1000000000.0;
        }

        static ITK_INLINE std::string bitSmartPrint(uint64_t bit, const char *unit)
        {
            uint64_t check_zero = bit;

            check_zero /= 1000;
            if (!check_zero)
                return ITKCommon::PrintfToStdString("%g %s", std::round((double)bit * 100.0) / 100.0, unit);
            check_zero /= 1000;
            if (!check_zero)
                return ITKCommon::PrintfToStdString("%g k%s", std::round(bitToKilo(bit) * 100.0) / 100.0, unit);
            check_zero /= 1000;
            if (!check_zero)
                return ITKCommon::PrintfToStdString("%g m%s", std::round(bitToMega(bit) * 100.0) / 100.0, unit);
            check_zero /= 1000;
            if (!check_zero)
                return ITKCommon::PrintfToStdString("%g g%s", std::round(bitToGiga(bit) * 100.0) / 100.0, unit);
            check_zero /= 1000;
            if (!check_zero)
                return ITKCommon::PrintfToStdString("%g t%s", std::round(bitToTera(bit) * 100.0) / 100.0, unit);
            check_zero /= 1000;
            if (!check_zero)
                return ITKCommon::PrintfToStdString("%g p%s", std::round(bitToPeta(bit) * 100.0) / 100.0, unit);
            return ITKCommon::PrintfToStdString("%g e%s", std::round(bitToExtra(bit) * 100.0) / 100.0, unit);
        }
    }

}