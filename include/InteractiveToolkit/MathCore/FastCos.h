#pragma once

#include "math_common.h"
#include "float.h"

namespace MathCore
{

#if defined(ITK_TRIGONOMETRIC_FASTEST_MORE_MEMORY) || defined(ITK_TRIGONOMETRIC_FAST_LESS_MEMORY)

    #define FastArc_DIM 16.0

    class FastArc
    {

        const int number_of_samples_cos = 180 * 1000 * 4;
        const float number_of_samples_cos_float = (float)number_of_samples_cos;

        // const float one_sample_above_1_0 = 1.0f + 1.0f / number_of_samples_cos_float;

        // const int number_of_samples_sin = 90 * 1000 * 4;
        // const float number_of_samples_sin_float = (float)number_of_samples_sin;

        // const float one_sample_above_1_0_sin = 1.0f + 1.0f / number_of_samples_sin_float;

        std::vector<float> acos_values;
        // std::vector<float> asin_values;

        // number_of_samples_cos =>
        //    90 degrees
        //    times the number of places of precision (100) [you can query angles like these: 90.23]
        //    times 2 (to double that precision)
        //    = 90 * 100 * 2
        FastArc() : acos_values(number_of_samples_cos + 1)
                    //,asin_values(number_of_samples_sin + 1)
        {

            for (int i = 0; i < number_of_samples_cos + 1; i++)
            {
                double _cs = (double)i / (double)number_of_samples_cos;

                // [-1..1]
                _cs = _cs * 2.0 - 1.0;

                _cs = array_map_2_cos(_cs);
                //_cs = cos_map_2_array(_cs);

                _cs = OP<double>::clamp(_cs, -1.0, 1.0);

                acos_values[i] = (float)::acos(_cs);
            }
            acos_values[number_of_samples_cos] = acos_values[number_of_samples_cos - 1];

            // for (int i = 0; i < number_of_samples_sin + 1; i++)
            // {
            //     double _cs = (double)i / (double)number_of_samples_sin;
            //     _cs = _cs * 2.0 - 1.0;

            //     float sign = OP<double>::sign(_cs);
            //     _cs = OP<double>::sqrt(OP<double>::abs(_cs)) * sign;

            //     _cs = OP<double>::clamp(_cs, -1.0, 1.0);
            //     asin_values[i] = (float)::asin(_cs);
            // }
        }

    public:
        // [-1..1]
        static float ITK_INLINE array_map_2_cos(const double &_csp)
        {

            // [ < 0 => -1 baixo , > 0 => 1 cima ]
            double sign_baixo_cima = OP<double>::sign(_csp);

            double _cs = OP<double>::abs(_csp);
            //new new attempt
            // https://www.desmos.com/calculator/heapv9nm20
            //
            //
            // y = ((x+1)^16-1)/(65535)
            //
            // x = (65535 y + 1)^(1/16) - 1
            // _cs = OP<float>::pow( 1.0 + 65535 * _cs, 1.0/16.0 ) - 1.0;



            //new attempt
            // https://www.desmos.com/calculator/olxtedbbyk
            //
            // binomio base:
            // x = sqrt(4 - 3 y) + 2         x = (16 - 15 y)^(1/4) + 2
            //
            // first case:
            // y= (-(2-x)^2 + 1) / (2^2-1) + 1        y= (-(2-x)^4 + 1) / (2^4-1) + 1
            //
            double d = FastArc_DIM;
            _cs = ( 1.0 - OP<double>::pow(2.0 - _cs, d) ) / ( OP<double>::pow(2.0, d) - 1.0 ) + 1.0;


            // https://www.desmos.com/calculator/gun4vmvrf8
            //
            // binomio base:
            // y = ((x+1)^d - 1)/(2^d-1)
            //
            // first case:
            // 
            // y = ((x+1)^3 - 1)/7
            // x = -1 + (1 + 7 y)^(1/3)
            //
            // double d = 2.0;
            //  _cs = OP<double>::pow(1.0 + (OP<double>::pow(2.0,d)-1.0) * _cs, 1.0/d) - 1.0;
            //
            // second case:
            // 
            // y = ((x+1)^4 - 1)/15
            // x == (1 + 15 y)^(1/4) - 1
            //
            //_cs = OP<double>::pow(1.0 + 255.0 * _cs, 1.0/8.0) - 1.0;

            // graph 0..1 to 0..0.5
            _cs *= sign_baixo_cima;

            return _cs;
        }

        // [-1..1]
        static float ITK_INLINE cos_map_2_array(const float &_csp)
        {

            // [ < 0 => -1 baixo , > 0 => 1 cima ]
            float sign_baixo_cima = OP<float>::sign(_csp);

            float _cs = OP<float>::abs(_csp);

            //new new attempt
            // https://www.desmos.com/calculator/heapv9nm20
            //
            //
            // y = ((x+1)^16-1)/(65535)
            //
            // x = (65535 y + 1)^(1/16) - 1
            // float _cnst_ = 1.0f/65535.0f;
            // _cs = _cs + 1.0f;
            // _cs = _cs*_cs;// ^2
            // _cs = _cs*_cs;// ^4
            // _cs = _cs*_cs;// ^8
            // _cs = _cs*_cs;// ^16
            // _cs = (_cs-1.0f) * _cnst_;

            //new attempt
            // https://www.desmos.com/calculator/olxtedbbyk
            //
            // binomio base:
            // x = -sqrt(4 - 3 y) + 2         x = (16 - 15 y)^(1/4) + 2
            //
            // first case:
            // y= (-(2-x)^2 + 1) / (2^2-1) + 1        y= (-(2-x)^4 + 1) / (2^4-1) + 1
            // x = 2 - ((1 - 2^d) (2^d/(1 - 2^d) + y))^(1/d)
            //
            const float d = (float)FastArc_DIM;
            const float one_over_d = 1.0f/d;
            const float _2_power_d = OP<float>::pow(2.0f, d);
            const float one_minus_2_power_d = 1.0f - _2_power_d;
            const float _2_power_d_over_one_minus_2_power_d = _2_power_d / one_minus_2_power_d;

            _cs = 2.0f - OP<float>::pow( one_minus_2_power_d * ( _2_power_d_over_one_minus_2_power_d + _cs ), one_over_d  );
            //_cs = 2.0f - ::expf( ::logf( one_minus_2_power_d * ( _2_power_d_over_one_minus_2_power_d + _cs ) ) * one_over_d );

            // _cs = OP<float>::sqrt( one_minus_2_power_d * ( _2_power_d_over_one_minus_2_power_d + _cs ) );
            // _cs = OP<float>::sqrt(_cs); // 1/4
            // _cs = OP<float>::sqrt(_cs); // 1/8
            // _cs = OP<float>::sqrt(_cs); // 1/16
            // _cs = 2.0f - _cs;

            // https://www.desmos.com/calculator/gun4vmvrf8
            //
            // binomio base:
            // y = ((x+1)^d - 1)/(2^d-1)
            //
            // first case:
            //
            // y = ((x+1)^3 - 1)/7
            // x = -1 + (1 + 7 y)^(1/3)
            // float _cs3 = _cs + 1;
            // _cs3 = _cs3 * _cs3;
            // const float _1_7 = 1.0f/3.0f;
            // _cs = (_cs3 - 1.0f) * _1_7;
            //
            // second case:
            // 
            // y = ((x+1)^4 - 1)/15
            // x == (1 + 15 y)^(1/4) - 1
            //
            // double _cs4 = _cs + 1;
            // _cs4 = _cs4 * _cs4;
            // _cs4 = _cs4 * _cs4;
            // _cs4 = _cs4 * _cs4;
            // const double _1_15 = 1.0f/255.0f;
            // _cs = (float)((_cs4 - 1.0) * _1_15);

            // graph 0..1 to 0..0.5
            _cs *= sign_baixo_cima;

            return _cs;
        }

        // // [-1..1]
        // static double ITK_INLINE array_map_2_cos2(const double &_csp)
        // {

        //     // [ < 0 => -1 baixo , > 0 => 1 cima ]
        //     double sign_baixo_cima = OP<double>::sign(_csp);
        //     double _cs = OP<double>::abs(_csp) * 2.0f - 1.0f;
        //     double _second_sign = OP<double>::sign(_cs);

        //     // circ_sqrt
        //     _cs = _second_sign - _cs;
        //     _cs = OP<double>::sqrt(1.0f - _cs * _cs);
        //     _cs *= _second_sign;

        //     _cs = _cs + 1.0f;

        //     // graph 0..1 to 0..0.5
        //     _cs *= sign_baixo_cima * 0.5f;

        //     return _cs;
        // }

        // // [-1..1]
        // static float ITK_INLINE cos_map_2_array2(const float &_csp)
        // {

        //     // [ < 0 => -1 baixo , > 0 => 1 cima ]
        //     float sign_baixo_cima = OP<float>::sign(_csp);
        //     float _cs = OP<float>::abs(_csp) * 2.0f - 1.0f;
        //     float _second_sign = OP<float>::sign(_cs);

        //     // circ_sqrt
        //     _cs = 1.0 - OP<float>::sqrt(1.0f - _cs * _cs);
        //     _cs *= _second_sign;

        //     _cs = _cs + 1.0f;

        //     // graph 0..1 to 0..0.5
        //     _cs *= sign_baixo_cima * 0.5f;

        //     return _cs;
        // }

        float ITK_INLINE acos(const float &_csp) const
        {
            float _abs = OP<float>::abs(_csp);

            if (_abs > 1.0f)
                return NAN;

            float _cs = cos_map_2_array(_csp);
            //float _cs = array_map_2_cos(_csp);

            _cs = _cs * 0.5f + 0.5f;
            _cs *= number_of_samples_cos_float;
            _cs += 0.5f;

            int32_t array_index = (int32_t)_cs;

            return acos_values[array_index];
        }

        float inline asin(const float &_csp) const
        {
            float _abs = OP<float>::abs(_csp);

            if (_abs > 1.0f)
                return NAN;

            const float cos_0 = MathCore::OP<float>::deg_2_rad( 90.0 );

            float sign = OP<float>::sign(_csp);

            float _cs = cos_map_2_array(_abs);

            _cs = _cs * 0.5f + 0.5f;
            _cs *= number_of_samples_cos_float;
            _cs += 0.5f;

            int32_t array_index = (int32_t)_cs;

            return sign * (cos_0 -  acos_values[array_index]);
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