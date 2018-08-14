#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Event.h"
#include "State.h"
#include "PythonWrapper.h"

class ImageProcessor
{
public:
    ImageProcessor();

    void GetDistance( double& dis ) { UpdatePosition(); dis = m_pos[0] - m_pos[2]; }
    void GetHealthValues( double& player, double& enemy ) { UpdateHealthValues(); player = m_playerHP; enemy = m_enemyHP; }
    void GetSuggestedState( StateMachine<Player>::STATE& state ) { UpdateState(); state = m_suggestedState; }
    void Update( const cv::Mat& input );
    std::vector<std::vector<float>> GetBBoxes();

    Event<int&, NpyArrayWrapper&, NpyArrayWrapper&, NpyArrayWrapper&, const cv::Mat&> m_EvalNetwork;

private:
    double ProcessHealthBar( const cv::Mat& input );
    void UpdateHealthValues();
    void UpdatePosition();
    void UpdateState();
    void PreprocessFrame();

    double m_playerHP;
    double m_enemyHP;
    std::vector<double> m_pos;
    StateMachine<Player>::STATE m_suggestedState;
    cv::Mat m_frame;
    bool m_healthUpdated, m_stateUpdated, m_positionUpdated;
};

#endif // !IMAGEPROCESSOR_H
