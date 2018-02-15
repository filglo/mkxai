#include "NeuralNetwork.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <filesystem>
#include <chrono>
#include <iostream>
#include "Utility.h"

NeuralNetwork::NeuralNetwork()
{
}

NeuralNetwork::~NeuralNetwork()
{
}

double NeuralNetwork::EvaluateSpeed() const
{
    auto time = std::chrono::high_resolution_clock::now();
    const int totalSamples = 1000;
    cv::Mat sampleImage = LoadImage("");
    for( int i = 0; i < totalSamples; ++i )
    {
        Predict( sampleImage );
        if( i % 10 == 0 )
        {
            std::cout << "Speed evaluation: " << static_cast<int>( i * 100 / totalSamples ) << "% complete." << std::endl;
        }
    }
    std::chrono::duration<double> timeDiff = std::chrono::high_resolution_clock::now() - time;
    std::cout << "Time per prediction: " << timeDiff.count() / totalSamples << std::endl;
    return timeDiff.count() / totalSamples;
}

int NeuralNetwork::Predict( const cv::Mat& mat ) const
{
    // TODO: check validity here
    //return m_pythonWrapper.Eval( mat );
    return 0;
}

cv::Mat NeuralNetwork::LoadImage( const std::string& imagefilename ) const
{
    auto img = cv::imread( imagefilename, cv::IMREAD_COLOR );
    if( img.data == nullptr ) throw std::runtime_error( "Failed to load image\n" );
    return img;
}
