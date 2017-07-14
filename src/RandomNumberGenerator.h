#ifndef RANDOMNUMBERGENERATOR_H
#define RANDOMNUMBERGENERATOR_H

#include <random>

class RandomNumberGenerator
{
public:
    RandomNumberGenerator();

    void Seed( unsigned int seed );
    double GetReal();
    double GetRealNormalDist();
    double GetSigmoid();
    uint64_t GetUInt64( uint64_t min, uint64_t max );
    
private:
    std::mt19937 m_rng;
};

namespace RNG
{
    extern RandomNumberGenerator g_rng;
}

#endif // !RANDOMNUMBERGENERATOR_H
