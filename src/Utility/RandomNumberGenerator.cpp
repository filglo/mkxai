#include "RandomNumberGenerator.h"

#include <Chrono>

#include "Utility.h"

namespace RNG
{
    RandomNumberGenerator g_rng;
}

RandomNumberGenerator::RandomNumberGenerator()
    : m_rng( static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()) )
{
}

void RandomNumberGenerator::Seed( unsigned int seed )
{
    m_rng.seed( seed );
}

double RandomNumberGenerator::GetReal()
{
    std::uniform_real_distribution<> urd( 0, 1.0 );
    return urd( m_rng );
}

double RandomNumberGenerator::GetRealNormalDist()
{
    std::normal_distribution<> nd( 0, 1 );
    return nd( m_rng );
}

double RandomNumberGenerator::GetSigmoid()
{
    std::uniform_real_distribution<> urd( -5, 5 );
    return Utility::SigmoidFunction( urd( m_rng ) );
}

uint64_t RandomNumberGenerator::GetUInt64( uint64_t min, uint64_t max )
{
    std::uniform_int_distribution<uint64_t> uid( min, max );
    return uid( m_rng );
}
