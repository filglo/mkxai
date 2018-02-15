#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "NeuralNetwork.h"
#include "Event.h"
#include "State.h"

class ImageProcessor
{
public:
    ImageProcessor();

    void GetDistance( double& dis ) { UpdatePosition(); dis = m_pos[0] - m_pos[2]; }
    void GetHealthValues( double& player, double& enemy ) { UpdateHealthValues(); player = m_playerHP; enemy = m_enemyHP; }
    void GetSuggestedState( StateMachine<Player>::STATE& state ) { UpdateState(); state = m_suggestedState; }
    void Update( const cv::Mat& input );

    Event<int&, const cv::Mat&> m_EvalNetwork;

private:
    double ProcessHealthBar( const cv::Mat& input );
    void UpdateHealthValues();
    void UpdatePosition();
    void UpdateState();

    double m_playerHP;
    double m_enemyHP;
    std::vector<double> m_pos;
    StateMachine<Player>::STATE m_suggestedState;
    cv::Mat m_frame;
    bool m_healthUpdated, m_stateUpdated, m_positionUpdated;
    NeuralNetwork m_network;
};

#endif // !IMAGEPROCESSOR_H
