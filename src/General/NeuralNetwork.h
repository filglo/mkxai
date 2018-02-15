#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <opencv2/core/core.hpp>
#include "PythonWrapper.h"

class NeuralNetwork
{
public:
    struct NetworkOutputData
    {
        std::vector<double> posA;
        std::vector<double> posB;
    };

    NeuralNetwork();
    ~NeuralNetwork();

    double EvaluateSpeed() const;
    int Predict( const cv::Mat& mat ) const;

private:
    cv::Mat NeuralNetwork::LoadImage( const std::string& imagefilename ) const;
};

#endif // !NEURALNETWORK_H
