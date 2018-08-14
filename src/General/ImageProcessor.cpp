#include "ImageProcessor.h"

ImageProcessor::ImageProcessor()
    : m_playerHP( 1.0 )
    , m_enemyHP( 1.0 )
    , m_suggestedState( StateMachine<Player>::STATE::DODGE )
    , m_stateUpdated( false )
    , m_positionUpdated( false )
    , m_healthUpdated( false )
    , m_frame( 720, 1280, CV_8UC3 )
{
    m_pos.resize( 4, 0 );
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
    auto boxes = GetBBoxes();
    auto output = std::vector<float>( 4 );
    if( boxes[0][0] != -1.f )
    {
        m_pos[0] = (boxes[0][1] + boxes[0][3]) / 2;
        m_pos[1] = (boxes[0][0] + boxes[0][2]) / 2;
    }
    if( boxes[1][0] != -1.f )
    {
        m_pos[2] = (boxes[1][1] + boxes[1][3]) / 2;
        m_pos[3] = (boxes[1][0] + boxes[1][2]) / 2;
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

void ImageProcessor::PreprocessFrame()
{
    std::stringstream ss;
    if( m_frame.channels() == 4 )
    {
        cv::cvtColor( m_frame, m_frame, CV_RGBA2RGB );
    }
    if( m_frame.rows != 720 || m_frame.cols != 1280 || m_frame.channels() != 3 || m_frame.depth() != CV_8U )
    {
        ss << m_frame.rows << " " << m_frame.cols << " " << m_frame.channels() << " " << m_frame.depth() << std::endl;
        throw std::logic_error( "Invalid mat size\n" + ss.str() );
    }
}

std::vector<std::vector<float>> ImageProcessor::GetBBoxes()
{
    PreprocessFrame();
    int detections;
    NpyArrayWrapper boxes;
    NpyArrayWrapper scores;
    NpyArrayWrapper classes;
    // TODO: catch exceptions
    m_EvalNetwork( detections, boxes, scores, classes, m_frame );
    int p1Idx = -1;
    int p2Idx = -1;
    for( int i = 0; i < detections; ++i )
    {
        if( p1Idx < 0 && classes.Get( i ) == 1 )
            p1Idx = i;
        if( p2Idx < 0 && classes.Get( i ) == 2 )
            p2Idx = i;
        if( p2Idx > 0 && p1Idx > 0 )
            break;
    }
    auto convertBox = []( NpyArrayWrapper& boxes, NpyArrayWrapper& scores, int idx, std::vector<float>& bb )
    {
        if( boxes.GetDims() == 2 && boxes.GetShape( 1 ) == 4 && scores.Get( idx ) > 0.75 )
        {
            for( int i = 0; i < 4; i++ )
            {
                bb.push_back( boxes.Get( idx, i ) );
            }
        }
        else
        {
            for( int i = 0; i < 4; i++ )
            {
                bb.push_back( static_cast<float>(-1) );
            }
        }
    };
    std::vector<float> p1bb;
    std::vector<float> p2bb;
    convertBox( boxes, scores, p1Idx, p1bb );
    convertBox( boxes, scores, p2Idx, p2bb );
    return std::vector<std::vector<float>>( { p1bb, p2bb } );
}
