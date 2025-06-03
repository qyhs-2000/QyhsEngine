#pragma once

#include <algorithm>
#include <cfloat>
#include <random>

namespace qyhs
{
    template<typename NumericType>
    using uniform_distribution = typename std::conditional<std::is_integral<NumericType>::value,
        std::uniform_int_distribution<NumericType>,
        std::uniform_real_distribution<NumericType>>::type;

    template<typename RandomEngine = std::default_random_engine>
    class RandomNumberGenerator
    {

    private:
        RandomEngine m_engine;

    public:
        template<typename... Params>
        explicit RandomNumberGenerator(Params&&... params) : m_engine(std::forward<Params>(params)...)
        {}

        template<typename... Params>
        void seed(Params&&... seeding)
        {
            m_engine.seed(std::forward<Params>(seeding)...);
        }

        template<typename DistributionFunc, typename... Params>
        typename DistributionFunc::result_type distribution(Params&&... params)
        {
            DistributionFunc dist(std::forward<Params>(params)...);
            return dist(m_engine);
        }

        template<typename NumericType>
        NumericType uniformDistribution(NumericType lower, NumericType upper)
        {
            if (lower == upper)
            {
                return lower;
            }
            return distribution<uniform_distribution<NumericType>>(lower, upper);
        }

        float uniformUnit() { return uniformDistribution(0.f, std::nextafter(1.f, FLT_MAX)); }

        float uniformSymmetry() { return uniformDistribution(-1.f, std::nextafter(1.f, FLT_MAX)); }

        bool bernoulliDistribution(float probability) { return distribution<std::bernoulli_distribution>(probability); }

        float normalDistribution(float mean, float stddev)
        {
            return distribution<std::normal_distribution<float>>(mean, stddev);
        }

        template<typename DistributionFunc, typename Range, typename... Params>
        void generator(Range&& range, Params&&... params)
        {
            // using ResultType = typename DistributionFunc::result_type;

            DistributionFunc dist(std::forward<Params>(params)...);
            return std::generate(std::begin(range), std::end(range), [&] { return dist(m_engine); });
        }
    };

    template<typename DistributionFunc,
        typename RandomEngine = std::default_random_engine,
        typename SeedType = std::seed_seq>
    class DistRandomNumberGenerator
    {
        using ResultType = typename DistributionFunc::result_type;

    private:
        RandomEngine      m_engine;
        DistributionFunc* m_dist = nullptr;

    public:
        template<typename... Params>
        explicit DistRandomNumberGenerator(SeedType&& seeding, Params&&...  /*params*/) : m_engine(seeding)
        {
            // m_dist = CHAOS_NEW_T(DistributionFunc)(std::forward<Params>(params)...);
        }

        ~DistRandomNumberGenerator() { CHAOS_DELETE_T(m_dist); }

        template<typename... Params>
        void seed(Params&&... params)
        {
            m_engine.seed(std::forward<Params>(params)...);
        }

        ResultType next() { return (*m_dist)(m_engine); }
    };

    using DefaultRNG = RandomNumberGenerator<std::mt19937>;
} // namespace Chaos

namespace qyhs::random
{
    //Pseudo-Random Number Generator
    class RandomNumberGenerator
    {
    public:
        uint64_t state = 0;

        inline RandomNumberGenerator(uint64_t seed = 0) : state(seed) {}

        inline void seed(uint64_t seed)
        {
            state = seed;
        }

        inline uint32_t next_uint(uint32_t min, uint32_t max)
        {
            return min + (uint32_t(next_uint()) % (std::min(std::numeric_limits<uint32_t>::max() - uint32_t(1), std::max(uint32_t(1), max - min)) + uint32_t(1)));
        }

        inline uint64_t next_uint()
        {
            state ^= state >> 12;
            state ^= state << 25;
            state ^= state >> 27;
            return state * 0x2545F4914F6CDD1DULL;
        }
        inline uint32_t next_int(int32_t min, int32_t max)
        {
            auto t = std::min(std::numeric_limits<int32_t>::max() - int32_t(1), std::max(int32_t(1), max - min)) + int32_t(1);
            return min + int32_t(next_uint() % (std::min(std::numeric_limits<int32_t>::max() - int32_t(1), std::max(int32_t(1), max - min)) + int32_t(1)));
        }
    private:
    };

    uint32_t getRandom(uint32_t max_value);
}