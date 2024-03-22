#pragma once

#include "math_common.h"
#include "float.h"

namespace MathCore
{

#if defined(ITK_TRIGONOMETRIC_FASTEST_MORE_MEMORY) || defined(ITK_TRIGONOMETRIC_FAST_LESS_MEMORY)

    class FastArc
    {

        const int number_of_samples_cos = 180 * 100;
        const float number_of_samples_cos_float = (float)number_of_samples_cos;

        const int number_of_samples_sin = 90 * 100;
        const float number_of_samples_sin_float = (float)number_of_samples_sin;

        std::vector<float> acos_values;
        std::vector<float> asin_values;

        // number_of_samples_cos =>
        //    90 degrees
        //    times the number of places of precision (100) [you can query angles like these: 90.23]
        //    times 2 (to double that precision)
        //    = 90 * 100 * 2
        FastArc() : acos_values(number_of_samples_cos + 1),
                    asin_values(number_of_samples_sin + 1)
        {

            int32_t values_size_int32 = (int)number_of_samples_cos;
            double values_size_double = (double)values_size_int32;

            for (int i = 0; i < values_size_int32 + 1; i++)
            {
                double _cs = (double)i / values_size_double;
                _cs = _cs * 2.0 - 1.0;
                _cs = OP<double>::clamp(_cs, -1.0, 1.0);
                acos_values[i] = (float)::acos(_cs);

                if (i > number_of_samples_sin)
                    continue;

                _cs = OP<double>::clamp(1.0 + _cs, -1.0, 1.0);

                asin_values[i] = (float)::asin(_cs);
            }
        }

    public:
        float ITK_INLINE acos(const float &_csp)
        {
            float _abs = OP<float>::abs(_csp);

            if (_abs > 1.0f)
                return NAN;

            // float _cs = (_csp + 1.0f) * 0.5f;
            float _cs = _csp * 0.5f + 0.5f;
            _cs = _cs * number_of_samples_cos_float + 0.5f;

            int32_t array_index = (int32_t)_cs;

            return acos_values[array_index];
        }

        float inline asin(float _csp)
        {
            float _abs = OP<float>::abs(_csp);

            if (_abs > 1.0f)
                return NAN;

            float sign = OP<float>::sign(_csp);
            float _cs = _abs * number_of_samples_sin_float + 0.5f;

            int32_t array_index = (int32_t)_cs;

            return sign * asin_values[array_index];
        }

        static FastArc *Instance()
        {
            static FastArc fastACos;
            return &fastACos;
        }
    };
#endif

#if defined(ITK_TRIGONOMETRIC_FASTEST_MORE_MEMORY)
    class FastCos
    {

        const int number_of_samples = 360 * 100;
        // const int number_of_samples = 32 * 1024;//32k

        const float number_of_samples_float = (float)number_of_samples;
        std::vector<float> cos_values;

        // number_of_samples =>
        //    90 degrees
        //    times the number of places of precision (100) [you can query angles like these: 90.23]
        //    times 2 (to double that precision)
        //    = 90 * 100 * 2
        //
        FastCos() : cos_values(number_of_samples + 1)
        {

            int32_t values_size_int32 = (int)number_of_samples;

            double values_size_double = (double)values_size_int32;
            const double _2pi = 2.0 * CONSTANT<double>::PI;

            // this fill gives more precision on high cos or sin values
            for (int i = 0; i < values_size_int32 + 1; i++)
            {
                double angle = ((double)i * _2pi) / values_size_double;

                const double _45_deg = OP<double>::deg_2_rad(45.0);
                const double _135_deg = OP<double>::deg_2_rad(135.0);

                const double _225_deg = OP<double>::deg_2_rad(225.0);
                const double _315_deg = OP<double>::deg_2_rad(315.0);

                if (angle >= _45_deg && angle <= _135_deg)
                {
                    const double _offset = -(double)(_2pi * 0.25);
                    cos_values[i] = -(float)::sin(angle + _offset);
                }
                else
                if (angle >= _225_deg && angle <= _315_deg)
                {
                    const double _offset = -(double)(_2pi * 0.75);
                    cos_values[i] = (float)::sin(angle + _offset);
                }
                else
                if (angle >= _315_deg || angle <= _45_deg)
                {
                    cos_values[i] = (float)::cos(angle);
                }
                else
                if (angle >= _135_deg && angle <= _225_deg)
                {
                    const double _offset = -(double)(_2pi * 0.5);
                    cos_values[i] = -(float)::cos(angle + _offset);
                }
                else
                {
                    cos_values[i] = (float)::cos(angle);
                }

                if (cos_values[i] == -0)
                    cos_values[i] = 0;
            }
        }

    public:
        float ITK_INLINE cos(const float &angle_queryp)const
        {
            const float inv_2pi = 1.0f / (2.0f * CONSTANT<float>::PI);

            float angle_query = angle_queryp;
            angle_query *= inv_2pi;
            angle_query -= OP<float>::floor(angle_query);
            angle_query *= number_of_samples_float;
            angle_query += 0.5f;

            int32_t array_index = (int32_t)angle_query;

#if !defined(NDEBUG)
            if (array_index < 0 || array_index > number_of_samples)
                throw std::runtime_error("x out of bounds exception.");
#endif

            return cos_values[array_index];
        }

        float inline sin(float angle_query)const
        {
            const float _270_deg = -(float)((2.0 * CONSTANT<double>::PI) * 0.25);
            angle_query += _270_deg;
            return cos(angle_query);
        }

        static FastCos *Instance()
        {
            static FastCos fastCos;
            return &fastCos;
        }
    };
#endif


#if defined(ITK_TRIGONOMETRIC_FAST_LESS_MEMORY)
    class FastCosQuarter
    {

        const int number_of_samples = 90 * 100;
        // const int number_of_samples = 32 * 1024;//32k

        const float number_of_samples_float_times_4 = (float)(number_of_samples << 2);
        std::vector<float> cos_values;

        // number_of_samples =>
        //    90 degrees
        //    times the number of places of precision (100) [you can query angles like these: 90.23]
        //    times 2 (to double that precision)
        //    = 90 * 100 * 2
        FastCosQuarter() : cos_values(number_of_samples + 1)
        {

            int32_t values_size_int32 = (int)number_of_samples;
            int32_t values_size_int32_times_4 = values_size_int32 << 2;
            float values_size_float_times_4 = (float)values_size_int32_times_4;

            double values_size_double = (double)values_size_int32;

            double _2pi_times_0_25 = CONSTANT<double>::PI * 0.5;
            double _2pi = 2.0 * CONSTANT<double>::PI;

            for (int i = 0; i < values_size_int32 + 1; i++)
            {
                double angle = ((double)i * _2pi_times_0_25) / values_size_double;

                const double _45_deg = OP<double>::deg_2_rad(45.0);
                const double _135_deg = OP<double>::deg_2_rad(135.0);

                const double _225_deg = OP<double>::deg_2_rad(225.0);
                const double _315_deg = OP<double>::deg_2_rad(315.0);

                if (angle >= _45_deg && angle <= _135_deg)
                {
                    const double _offset = -(double)(_2pi * 0.25);
                    cos_values[i] = -(float)::sin(angle + _offset);
                } else {
                    cos_values[i] = (float)::cos(angle);
                }

                if (cos_values[i] == -0.0f)
                    cos_values[i] = 0;
            }
        }

    public:
        float ITK_INLINE cos(const float &angle_queryp)const
        {
            const float inv_2pi = 1.0f / (2.0f * CONSTANT<float>::PI);

            float angle_query = angle_queryp;
            angle_query *= inv_2pi;
            angle_query -= OP<float>::floor(angle_query);
            angle_query *= number_of_samples_float_times_4;
            angle_query += 0.5f;

            int32_t int_angle = (int32_t)angle_query;

            int32_t region = int_angle / number_of_samples;
            int32_t real_index = int_angle - region * number_of_samples;

            int32_t is_to_invert_mask = -(region & 0x01);
            int32_t inverted_index = number_of_samples - real_index;
            real_index = inverted_index & is_to_invert_mask |
                        real_index & ~is_to_invert_mask;

            int32_t sig = 1 - ( (region ^ (region << 1)) & 0x02 );

            return (float)sig * cos_values[real_index];

// #if !defined(NDEBUG)
//             if (region != 4 && (real_index < 0 || real_index > number_of_samples))
//                 throw std::runtime_error("x out of bounds exception.");
// #endif

//             switch (region)
//             {
//             case 0:
//                 return cos_values[real_index];
//             case 1:
//                 real_index = num_samples_90 - real_index;
//                 return -cos_values[real_index];// * -OP<int32_t>::sign(real_index - number_of_samples);
//             case 2:
//                 return -cos_values[real_index];// * -OP<int32_t>::sign(real_index - number_of_samples);
//             case 3:
//                 real_index = num_samples_90 - real_index;
//                 return cos_values[real_index];
//             case 4:
//                 return 1.0f;
//             default:
//                 return NAN;
//             }

            // int32_t real_index_inverse = num_samples_90 - real_index;
            // int32_t idx_array[5] = {real_index,real_index_inverse,real_index,real_index_inverse,0};
            // const int32_t sign_array[5] = {1,-1,-1,1,1};
            // return sign_array[region] * cos_values[idx_array[region]];
        }

        float inline sin(float angle_query) const
        {
            const float _270_deg = -(float)((2.0 * CONSTANT<double>::PI) * 0.25);
            angle_query += _270_deg;
            return cos(angle_query);
        }

        static FastCosQuarter *Instance()
        {
            static FastCosQuarter fastCosQuarter;
            return &fastCosQuarter;
        }
    };

#endif

}