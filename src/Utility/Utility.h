#ifndef UTILITY_H
#define UTILITY_H

#include <vector>

namespace Utility
{
    double SigmoidFunction( double value );
    double MSE( const std::vector<double>& outputs, const std::vector<double>& values );
    template<typename T, typename U = std::allocator<T>> double Distance( const std::vector<T,U>& outputs, const std::vector<T,U>& values )
    {
        double total = 0.0;
        for( size_t i = 0; i < outputs.size(); ++i )
        {
            total += std::pow( outputs[i] - values[i], 2 );
        }
        return std::pow( total, 0.5 );
    }
}

#endif // !UTILITY_H
