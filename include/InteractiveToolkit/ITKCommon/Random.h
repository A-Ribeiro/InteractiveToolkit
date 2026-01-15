#pragma once

#include "../common.h"
#include <random>

namespace ITKCommon
{

    template <typename _int_type_>
    struct RandomDefinition
    {
    };

    template <>
    struct RandomDefinition<uint32_t>
    {
        using mt19937 = std::mt19937;
        static ITK_INLINE uint32_t randomSeed() noexcept
        {
            // printf("randomSeed 32 bits\n");
            return std::random_device{}();
        }
    };

    template <>
    struct RandomDefinition<uint64_t>
    {
        using mt19937 = std::mt19937_64;
        static ITK_INLINE uint64_t randomSeed() noexcept
        {
            // printf("randomSeed 64 bits\n");
            std::random_device rnd_sd;
            uint64_t rnd_sd_64 = rnd_sd();
            rnd_sd_64 = (rnd_sd_64 << 32) | (uint64_t)rnd_sd();
            return rnd_sd_64;
        }
    };

    template <typename _type_>
    class RandomTemplate
    {
        using self_type = RandomTemplate<_type_>;
        using random_definition = RandomDefinition<_type_>;
        using mt19937_type = typename random_definition::mt19937;

        mt19937_type mt;

    public:
        using TypeDefinition = RandomDefinition<_type_>;

        _type_ seed;

        RandomTemplate(_type_ seed = mt19937_type::default_seed)
        {
            // printf("Random 32 bits\n");
            this->setSeed(seed);
        }

        void setSeed(_type_ s)
        {
            seed = s;
            mt.seed(s);
            mt.discard(700000);
        }

        _type_ randomSeed()
        {
            // auto time_point = std::chrono::high_resolution_clock::now();
            // auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(time_point.time_since_epoch()).count();
            _type_ rnd_sd = random_definition::randomSeed();
            this->setSeed(rnd_sd);
            return rnd_sd;
        }

        double getDouble01()
        {
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            return dist(mt);
        }

        float getFloat01()
        {
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            return dist(mt);
        }

        template <typename f_type,
                  typename std::enable_if<
                      std::is_same<f_type, float>::value,
                      bool>::type = true>
        f_type next01()
        {
            return getFloat01();
        }

        template <typename f_type,
                  typename std::enable_if<
                      std::is_same<f_type, double>::value,
                      bool>::type = true>
        f_type next01()
        {
            return getDouble01();
        }

        // int getRange(int min, int max)
        // {
        //     std::uniform_int_distribution<int> dist(min, max);
        //     return dist(mt);
        // }

        template<typename T>
        typename std::enable_if<(sizeof(T) == 1), T>::type
        getRange(T min, T max)
        {
            std::uniform_int_distribution<int> dist(min, max);
            return static_cast<T>(dist(mt));
        }

        template<typename T>
        typename std::enable_if<(sizeof(T) > 1), T>::type
        getRange(T min, T max)
        {
            std::uniform_int_distribution<T> dist(min, max);
            return dist(mt);
        }

        static self_type *Instance()
        {
            // auto time_point = std::chrono::high_resolution_clock::now();
            // auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(time_point.time_since_epoch()).count();
            static self_type _rnd(random_definition::randomSeed());
            return &_rnd;
        }
    };

    using Random32 = ITKCommon::RandomTemplate<uint32_t>;
    using Random64 = ITKCommon::RandomTemplate<uint64_t>;
    using Random = Random32;
}
