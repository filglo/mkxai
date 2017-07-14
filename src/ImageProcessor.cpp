#include "ImageProcessor.h"

ImageProcessor::ImageProcessor()
    : m_playerHP( 1.0 )
    , m_enemyHP( 1.0 )
    , m_suggestedState( StateMachine<Player>::STATE::DODGE )
    , m_stateUpdated( false )
    , m_positionUpdated( false )
    , m_healthUpdated( false )
    , m_frame( 720, 1280, CV_8UC4 )
{
    m_pos.resize( 4 );
    m_network.LoadNetwork( "D:/repos/NNP/NNProject/models/" );
}

void ImageProcessor::UpdateHealthValues()
{
    if( m_healthUpdated )
        return;
    cv::Rect roiA( 12, 13, 380, 2 );
    cv::Rect roiB( 420, 13, 380, 2 );
    m_playerHP = ProcessHealthBar( m_frame( roiA ) );
    m_enemyHP = ProcessHealthBar( m_frame( roiB ) );
    m_healthUpdated = true;
}

double ImageProcessor::ProcessHealthBar( const cv::Mat& input )
{
    cv::Mat bar = input;
    cv::cvtColor( bar, bar, CV_BGR2GRAY );
    cv::Mat otsu;
    cv::threshold( bar, otsu, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU );
    cv::threshold( bar, bar, 60, 255, CV_THRESH_BINARY );
    double c = cv::countNonZero( bar );
    if( c < 664 )
    {
        c = cv::countNonZero( otsu );
    }
    c = c / 707;
    return c;
}

void ImageProcessor::Update( const cv::Mat& input )
{
    m_frame = input;
    m_stateUpdated = false;
    m_positionUpdated = false;
    m_healthUpdated = false;
}

void ImageProcessor::UpdatePosition()
{
    if( m_positionUpdated )
        return;
    auto output = m_network.Predict( m_frame, 1.0 );
    for( int i = 0; i < m_pos.size(); ++i )
    {
        m_pos[i] = output[i];
    }
    m_positionUpdated = true;
}

void ImageProcessor::UpdateState()
{
    if( m_stateUpdated )
        return;
    /*
    auto output = m_network.Predict( input, 1.0 );
    if( output[0] > 0.9 )
    {
        m_suggestedState = StateMachine<Player>::STATE::ATTACK;
    }
    */
    m_stateUpdated = true;
}
