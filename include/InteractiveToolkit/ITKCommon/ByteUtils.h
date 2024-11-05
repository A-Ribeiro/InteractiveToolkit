#pragma once

#include "../common.h"

namespace ITKCommon
{
    namespace ByteUtils
	{
		static ITK_INLINE double byteToKilo(uint64_t byte) {
			return (double)byte / 1024.0;
		}
		static ITK_INLINE double byteToMega(uint64_t byte) {
			return (double)byte / 1048576.0;
		}
		static ITK_INLINE double byteToGiga(uint64_t byte) {
			return (double)byte / 1073741824.0;
		}
		static ITK_INLINE double byteToTera(uint64_t byte) {
			return byteToGiga(byte) / 1024.0;
		}
		static ITK_INLINE double byteToPeta(uint64_t byte) {
			return byteToGiga(byte) / 1048576.0;
		}
		static ITK_INLINE double byteToExtra(uint64_t byte) {
			return byteToGiga(byte) / 1073741824.0;
		}

		static ITK_INLINE std::string byteSmartPrint(uint64_t byte, const char* unit) {
			uint64_t check_zero = byte;

			check_zero >>= 10;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf %s", (double)byte, unit);
			check_zero >>= 10;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf K%s", byteToKilo(byte), unit);
			check_zero >>= 10;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf M%s", byteToMega(byte), unit);
			check_zero >>= 10;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf G%s", byteToGiga(byte), unit);
			check_zero >>= 10;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf T%s", byteToTera(byte), unit);
			check_zero >>= 10;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf P%s", byteToPeta(byte), unit);
			return ITKCommon::PrintfToStdString("%.2lf E%s", byteToExtra(byte), unit);
		}


		static ITK_INLINE double bitToKilo(uint64_t byte) {
			return (double)byte / 1000.0;
		}
		static ITK_INLINE double bitToMega(uint64_t byte) {
			return (double)byte / 1000000.0;
		}
		static ITK_INLINE double bitToGiga(uint64_t byte) {
			return (double)byte / 1000000000.0;
		}
		static ITK_INLINE double bitToTera(uint64_t byte) {
			return bitToGiga(byte) / 1000.0;
		}
		static ITK_INLINE double bitToPeta(uint64_t byte) {
			return bitToGiga(byte) / 1000000.0;
		}
		static ITK_INLINE double bitToExtra(uint64_t byte) {
			return bitToGiga(byte) / 1000000000.0;
		}

		static ITK_INLINE std::string bitSmartPrint(uint64_t bit, const char* unit) {
			uint64_t check_zero = bit;

			check_zero /= 1000;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf %s", (double)bit, unit);
			check_zero /= 1000;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf k%s", bitToKilo(bit), unit);
			check_zero /= 1000;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf m%s", bitToMega(bit), unit);
			check_zero /= 1000;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf g%s", bitToGiga(bit), unit);
			check_zero /= 1000;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf t%s", bitToTera(bit), unit);
			check_zero /= 1000;
			if (!check_zero)
				return ITKCommon::PrintfToStdString("%.2lf p%s", bitToPeta(bit), unit);
			return ITKCommon::PrintfToStdString("%.2lf e%s", bitToExtra(bit), unit);
		}
	}
    
}