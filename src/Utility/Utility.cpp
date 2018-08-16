#include "Utility.h"

namespace Utility
{
    double SigmoidFunction( double x )
    {
        return 1.0 / ( 1.0 + exp( -x ) );
    }

    double MSE( const std::vector<double>& outputs, const std::vector<double>& values )
    {
        double sum = 0.0;
        for( uint32_t idx = 0; idx < outputs.size(); ++idx )
        {
            sum += std::pow( values[idx] - outputs[idx], 2 );
        }
        return sum / outputs.size();
    }
}
