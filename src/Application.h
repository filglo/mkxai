#ifndef APPLICATION_H
#define APPLICATION_H

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <chrono>
#include "Config.h"
#include "ScreenCapture.h"
#include "Player.h"
#include "ImageProcessor.h"
#include "IOKeyboard.h"
#include "IOViGEm.h"

class Application
{
public:
    Application();
    ~Application();

    bool Run();

private:
    bool ProcessUserInput();
    void Update( std::chrono::duration<double> timeDiff );
    void Render();
    void SaveFrame();

    Dispatcher m_dispatcher;
    ScreenCapture m_screenCapture;
    IOKeyboard m_ioKeyboard;
#if MOCK_GAMEPAD_IO
    IODummyInterface m_ioGamepad;
#else
    IOViGEm m_ioGamepad;
#endif
    ImageProcessor m_imageProcessor;
    Player m_player;
    cv::Mat m_matBitmap;
    std::chrono::duration<double> m_timeCount;
    int m_updateCount;
    bool m_pause;


    // shouldnt be here
    std::vector<double> posA;
    std::vector<double> posB;
};

#endif // !APPLICATION_H
