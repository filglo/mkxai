#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#undef min
#undef max
#define _SCL_SECURE_NO_WARNINGS
#include <tiny_dnn/tiny_dnn.h>
#include <opencv2/core/core.hpp>

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

    void LoadNetwork( const std::string& path );
    void LoadNetwork( const std::string & path, const std::string & name );
    void SaveNetwork();
    void Evaluate();
    void LoadTrainingData( const std::string& path );
    void Train();
    double EvaluateSpeed();
    tiny_dnn::vec_t Predict( const tiny_dnn::vec_t& input );
    tiny_dnn::vec_t Predict( const cv::Mat& img, double scale );
    NetworkOutputData OutputToData( const tiny_dnn::vec_t& output ) const;
    tiny_dnn::vec_t DataToOutput( const NetworkOutputData& data ) const;
    NetworkOutputData DecodeOutput( const tiny_dnn::vec_t& output ) const;
    tiny_dnn::vec_t EncodeOutput( const NetworkOutputData& data ) const;

private:
    struct TrainingData
    {
        std::vector<tiny_dnn::vec_t> train_labels, test_labels;
        std::vector<tiny_dnn::vec_t> train_images, test_images;
    };

    void CreateNetwork();
    void ConvertImage( const std::string& imagefilename, double scale, std::vector<tiny_dnn::vec_t>& data );
    void ConvertImages( const std::string& directory, double scale, std::vector<tiny_dnn::vec_t>& data );
    void ParseLabels( const std::string& filename, int labelSize, std::vector<tiny_dnn::vec_t>& labels );
    double CalculateAccuracy( const std::vector<tiny_dnn::vec_t>& outputs, const std::vector<tiny_dnn::vec_t>& labels ) const;

    const size_t m_sizeW = 256; //640 // 256
    const size_t m_sizeH = 144; //360 // 144
    tiny_dnn::network<tiny_dnn::sequential> m_network;
    tiny_dnn::adamax m_optimizer;
    TrainingData m_trainingData;
};

#endif // !NEURALNETWORK_H
