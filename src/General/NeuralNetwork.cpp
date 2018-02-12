#include "NeuralNetwork.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <filesystem>
#include <tinyxml2.cpp>
#include <chrono>
#include "Utility.h"

NeuralNetwork::NeuralNetwork()
{
    try
    {
        CreateNetwork();
    }
    catch( tiny_dnn::nn_error err )
    {
        std::cout << err.what();
    }
}

NeuralNetwork::~NeuralNetwork()
{
}

void NeuralNetwork::LoadNetwork( const std::string& path )
{
    m_network.load( path + "Network-model", tiny_dnn::content_type::weights_and_model, tiny_dnn::file_format::binary );
}

void NeuralNetwork::LoadNetwork( const std::string& path, const std::string& name )
{
    m_network.load( path + name, tiny_dnn::content_type::weights_and_model, tiny_dnn::file_format::binary );
}

void NeuralNetwork::SaveNetwork()
{
    m_network.save( "Network-model", tiny_dnn::content_type::weights_and_model, tiny_dnn::file_format::binary );
}

void NeuralNetwork::CreateNetwork()
{
    using conv = tiny_dnn::layers::conv;
    using max_pool = tiny_dnn::layers::max_pool;
    using fc = tiny_dnn::layers::fc;
    using relu = tiny_dnn::activation::leaky_relu;
    using tiny_dnn::core::connection_table;

    // try only classification
    /*
    m_network << conv( m_sizeW, m_sizeH, 5, 5, 1, 512, tiny_dnn::padding::same, true, 2, 2 )
        << relu( 128, 72, 512, 0.1 )
        << max_pool( 128, 72, 512, 2 )
        << conv( 64, 36, 3, 3, 512, 256, tiny_dnn::padding::same, true, 1, 1 )
        << relu( 64, 36, 256, 0.1 )
        << max_pool( 64, 36, 256, 2 )
        << conv( 32, 18, 3, 3, 256, 128, tiny_dnn::padding::same, true, 1, 1 )
        << relu( 32, 18, 128, 0.1 )
        << max_pool( 32, 18, 128, 2 )
        << conv( 16, 9, 1, 1, 128, 32, tiny_dnn::padding::same, true, 1, 1 )
        << relu( 16, 9, 32, 0.1 )
        << fc( 16*9*32, 16*9*2 )
        << relu( 16*9*2, 0.0001 )
        << fc( 16*9*2, 16*9*16 )
        << relu( 16*9*16, 0.0001 )
        << fc( 16*9*16, 4 );
        */

    /*m_network << conv( m_sizeW, m_sizeH, 7, 7, 1, 64, tiny_dnn::padding::same, true, 2, 2 )
        << relu( 128, 72, 64, 0.1 )
        << max_pool( 128, 72, 64, 2 )
        << conv( 64, 36, 3, 3, 64, 128, tiny_dnn::padding::same, true, 1, 1 )
        << relu( 64, 36, 128, 0.1 )
        << max_pool( 64, 36, 128, 4 )
        << conv( 16, 9, 1, 1, 128, 64, tiny_dnn::padding::same, true, 1, 1 )
        << relu( 16, 9, 64, 0.1 )
        << fc( 16*9*64, 4096 )
        << relu( 4096, 0.1 )
        << fc( 4096, 4 );*/

    int scale = 64;
    m_network << conv( m_sizeW, m_sizeH, 7, 7, 1, scale, tiny_dnn::padding::same, true, 2, 2 )
        << relu( 128, 72, scale, 0.1 )
        << max_pool( 128, 72, scale, 2 )
        << conv( 64, 36, 3, 3, scale, scale/2, tiny_dnn::padding::same, true, 1, 1 )
        << relu( 64, 36, scale/2, 0.1 )
        << max_pool( 64, 36, scale/2, 4 )
        << conv( 16, 9, 1, 1, scale/2, scale/4, tiny_dnn::padding::same, true, 1, 1 )
        << relu( 16, 9, scale/4, 0.1 )
        << fc( 16 * 9 * scale/4, 1024 )
        << relu( 1024, 0.1 )
        << fc( 1024, 4 );

    m_network.weight_init( tiny_dnn::weight_init::xavier( 0.01 ) );
}

void NeuralNetwork::Evaluate()
{
    // test and show results
    std::vector<tiny_dnn::vec_t> outputs = m_network.test( m_trainingData.test_images );

    auto loss = m_network.get_loss<tiny_dnn::mse>( m_trainingData.test_images, m_trainingData.test_labels );
    auto trainingLoss = m_network.get_loss<tiny_dnn::mse>( m_trainingData.train_images, m_trainingData.train_labels );
    std::cout << "Test results: " << std::endl;
    std::cout << "Accuracy: " << CalculateAccuracy( outputs, m_trainingData.test_labels ) << " Loss: " << loss << std::endl;
    std::cout << "Training loss: " << trainingLoss << std::endl;
}

void NeuralNetwork::LoadTrainingData( const std::string& path )
{
    ConvertImages( path + "/standing/images", 1.0 / 255, m_trainingData.train_images );
    //ConvertImages( path + "/random/images", 1.0 / 255, m_trainingData.train_images );
    ConvertImages( path + "/random_test/images", 1.0 / 255, m_trainingData.test_images );
    try
    {
        ParseLabels( path + "/standing/labels", 4, m_trainingData.train_labels );
        //ParseLabels( path + "/random/labels", 4, m_trainingData.train_labels );
        ParseLabels( path + "/random_test/labels", 4, m_trainingData.test_labels );
    }
    catch( tiny_dnn::nn_error err )
    {
        std::cout << err.what();
    }
}

void NeuralNetwork::Train()
{
    tiny_dnn::progress_display disp( m_trainingData.train_images.size() );
    tiny_dnn::timer t;
    int minibatch_size = 10;

    m_optimizer.alpha *= static_cast<tiny_dnn::float_t>(std::sqrt( minibatch_size ));

    int epoch = 0;

    auto on_enumerate_epoch = [&]() {
        std::cout << std::endl << "epoch " << ++epoch << std::endl;
        std::cout << t.elapsed() << "s elapsed." << std::endl;

        Evaluate();

        disp.restart( m_trainingData.train_images.size() );
        t.restart();
    };

    auto on_enumerate_minibatch = [&]() { disp += minibatch_size; };

    m_network.fit<tiny_dnn::mse>( m_optimizer, m_trainingData.train_images, m_trainingData.train_labels, minibatch_size, 20,
        on_enumerate_minibatch, on_enumerate_epoch );

    std::cout << "end training." << std::endl;
}

double NeuralNetwork::EvaluateSpeed()
{
    auto time = std::chrono::high_resolution_clock::now();
    const int totalSamples = 200; //1500
    for( int i = 0; i < totalSamples; ++i )
    {
        Predict( m_trainingData.test_images[3] );
        if( i % 10 == 0 )
        {
            std::cout << "Speed evaluation: " << static_cast<int>( i * 100 / totalSamples ) << "% complete." << std::endl;
        }
    }
    std::chrono::duration<double> timeDiff = std::chrono::high_resolution_clock::now() - time;
    std::cout << "Time per prediction: " << timeDiff.count() / totalSamples << std::endl;
    return timeDiff.count() / totalSamples;
}

tiny_dnn::vec_t NeuralNetwork::Predict( const tiny_dnn::vec_t& input )
{
    return m_network.predict( input );
}

tiny_dnn::vec_t NeuralNetwork::Predict( const cv::Mat& img, double scale )
{
    auto im2 = img;
    cv::cvtColor( im2, im2, CV_BGRA2GRAY );
    cv::Mat_<uint8_t> resized;
    cv::resize( im2, resized, cv::Size( m_sizeW, m_sizeH ) );
    
    //resized = im2;
    tiny_dnn::vec_t input;
    std::transform( resized.begin(), resized.end(), std::back_inserter( input ),
        [=]( uint8_t c ) { return c * scale; } );
    return m_network.predict( input );
}

NeuralNetwork::NetworkOutputData NeuralNetwork::OutputToData( const tiny_dnn::vec_t & output ) const
{
    double highA = -15000.0;
    int highAcellX = 0;
    int highAcellY = 0;
    double highB = -15000.0;
    int highBcellX = 0;
    int highBcellY = 0;
    for( int y = 0; y < 9; ++y )
    {
        for( int x = 0; x < 16; ++x )
        {
            if( output[(16 * y + x) * 5 + 3] >= highA )
            {
                highA = output[(16 * y + x) * 5 + 3];
                highAcellX = x;
                highAcellY = y;
            }
            if( output[(16 * y + x) * 5 + 4] >= highB )
            {
                highB = output[(16 * y + x) * 5 + 4];
                highBcellX = x;
                highBcellY = y;
            }
        }
    }
    NetworkOutputData data;
    data.posA.resize( 2 );
    data.posB.resize( 2 );
    data.posA[0] = highAcellX + output[(16 * highAcellY + highAcellX) * 5] / 16.0;
    data.posA[1] = highAcellY + output[(16 * highAcellY + highAcellX) * 5 + 1] / 9.0;
    data.posB[0] = highBcellX + output[(16 * highBcellY + highBcellX) * 5] / 16.0;
    data.posB[1] = highBcellY + output[(16 * highBcellY + highBcellX) * 5 + 1] / 9.0;
    return data;
}

tiny_dnn::vec_t NeuralNetwork::DataToOutput( const NetworkOutputData& data ) const
{
    tiny_dnn::vec_t output( 720, 0.0 );

    double ACellX = static_cast<int>(data.posA[0] * 16.0001);
    double ACellY = static_cast<int>(data.posA[1] * 9.0001);
    double BCellX = static_cast<int>(data.posB[0] * 16.0001);
    double BCellY = static_cast<int>(data.posB[1] * 9.0001);

    for( int y = 0; y < 9; ++y )
    {
        for( int x = 0; x < 16; ++x )
        {
            if( x == ACellX && y == ACellY )
            {
                output[(16 * y + x) * 5 + 0] = (data.posA[0] - x / 16.0) * 16;
                output[(16 * y + x) * 5 + 1] = (data.posA[1] - y / 9.0) * 9;
                output[(16 * y + x) * 5 + 3] = 1.0;
            }
            else if( x == BCellX && y == BCellY )
            {
                output[(16 * y + x) * 5 + 0] = (data.posB[0] - x / 16.0) * 16;
                output[(16 * y + x) * 5 + 1] = (data.posB[1] - y / 9.0) * 9;
                output[(16 * y + x) * 5 + 4] = 1.0;
            }
            else
            {
                output[(16 * y + x) * 5 + 2] = 1.0;
            }
        }
    }

    return output;
}

NeuralNetwork::NetworkOutputData NeuralNetwork::DecodeOutput( const tiny_dnn::vec_t & output ) const
{
    NetworkOutputData data;
    data.posA.resize( 2 );
    data.posB.resize( 2 );
    data.posA[0] = output[0];
    data.posA[1] = output[1];
    data.posB[0] = output[2];
    data.posB[1] = output[3];
    return data;
}

tiny_dnn::vec_t NeuralNetwork::EncodeOutput( const NetworkOutputData & data ) const
{
    tiny_dnn::vec_t output( 4, 0.0 );

    output[0] = data.posA[0];
    output[1] = data.posA[1];
    output[2] = data.posB[0];
    output[3] = data.posB[1];

    return output;
}

void NeuralNetwork::ConvertImage( const std::string & imagefilename, double scale, std::vector<tiny_dnn::vec_t>& data )
{
    auto img = cv::imread( imagefilename, cv::IMREAD_GRAYSCALE );
    if( img.data == nullptr ) return; // cannot open, or it's not an image

    cv::resize( img, img, cv::Size( m_sizeW, m_sizeH ) );
    cv::Mat_<uint8_t> resized;
    resized = img;
    tiny_dnn::vec_t d;

    std::transform( resized.begin(), resized.end(), std::back_inserter( d ),
        [=]( uint8_t c ) { return c * scale; } );
    data.push_back( d );
}

void NeuralNetwork::ParseLabels( const std::string& directory, int labelSize, std::vector<tiny_dnn::vec_t>& labels )
{
    std::experimental::filesystem::directory_iterator di( directory );

    for( auto& fn : std::experimental::filesystem::directory_iterator( directory ) )
    {
        tinyxml2::XMLDocument doc;
        if( doc.LoadFile( fn.path().string().c_str() ) == 0 )
        {
            int width, height;
            auto sizeElement = doc.FirstChildElement( "annotation" )->FirstChildElement( "size" );
            sizeElement->FirstChildElement( "width" )->QueryIntText( &width );
            sizeElement->FirstChildElement( "height" )->QueryIntText( &height );
            auto objectElement = doc.FirstChildElement( "annotation" )->FirstChildElement( "object" );
            
            auto GetBBox = []( int& xmin, int& xmax, int& ymin, int& ymax, tinyxml2::XMLElement* objectElement )
            {
                objectElement->FirstChildElement("bndbox")->FirstChildElement( "xmin" )->QueryIntText( &xmin );
                objectElement->FirstChildElement( "bndbox" )->FirstChildElement( "xmax" )->QueryIntText( &xmax );
                objectElement->FirstChildElement( "bndbox" )->FirstChildElement( "ymin" )->QueryIntText( &ymin );
                objectElement->FirstChildElement( "bndbox" )->FirstChildElement( "ymax" )->QueryIntText( &ymax );
            };

            int xmin, xmax, ymin, ymax;
            float x1, y1, x2, y2;

            for( int i = 0; i < 2; ++i )
            {
                GetBBox( xmin, xmax, ymin, ymax, objectElement );
                std::string name = objectElement->FirstChildElement( "name" )->GetText();
                if( name == "cassieCage" )
                {
                    x1 = static_cast<double>(xmax + xmin) / (2 * width);
                    y1 = static_cast<double>(ymax + ymin) / (2 * height);
                }
                else
                {
                    x2 = static_cast<double>(xmax + xmin) / (2 * width);
                    y2 = static_cast<double>(ymax + ymin) / (2 * height);
                }
                objectElement = objectElement->NextSiblingElement( "object" );
            }
            NetworkOutputData data;
            data.posA.resize( 2 );
            data.posA[0] = x1;
            data.posA[1] = y1;
            data.posB.resize( 2 );
            data.posB[0] = x2;
            data.posB[1] = y2;
            labels.push_back( EncodeOutput( data ) );
        }
        else
        {
            throw tiny_dnn::nn_error( "failed to open labels file: " + fn.path().string() );
        }
    }   
}

double NeuralNetwork::CalculateAccuracy( const std::vector<tiny_dnn::vec_t>& outputs, const std::vector<tiny_dnn::vec_t>& labels ) const
{
    double acc = 0.0;
    for( size_t i = 0; i < outputs.size(); ++i )
    {
        double d1 = 0;
        double d2 = 0;
        auto data1 = DecodeOutput( outputs[i] );

        auto data2 = DecodeOutput( labels[i] );

        const double b = -6.513;
        d1 = std::exp( b * Utility::Distance<double>( data1.posA, data2.posA ) );
        d2 = std::exp( b * Utility::Distance<double>( data1.posB, data2.posB ) );
        acc += (d1 + d2) / 2;
    }
    return acc / outputs.size();
}

void NeuralNetwork::ConvertImages( const std::string & directory, double scale, std::vector<tiny_dnn::vec_t>& data )
{
    std::experimental::filesystem::directory_iterator di( directory );

    for( auto& fn : std::experimental::filesystem::directory_iterator( directory ) )
    {
        ConvertImage( fn.path().string(), scale, data );
    }
}
