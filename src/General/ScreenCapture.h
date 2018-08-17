#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H

#include <Windows.h>
#include <cstdint>
#include <opencv2/highgui/highgui.hpp>

class ScreenCapture
{
public:
    ScreenCapture( unsigned int frameWidth, unsigned int frameHeight );
    ~ScreenCapture() {}

    virtual void Capture() = 0;
    virtual UINT8* GetPixelDataPointer() = 0;

protected:
    unsigned int m_imageWidth;
    unsigned int m_imageHeight;
};

class ScreenCaptureDummy : ScreenCapture
{
public:
    ScreenCaptureDummy( unsigned int frameWidth, unsigned int frameHeight );
    ~ScreenCaptureDummy() {}

    void Capture() {}
    UINT8* GetPixelDataPointer() { return m_matBitmap.data; }

private:
    cv::Mat m_matBitmap;
};

class ScreenCaptureWindows : ScreenCapture
{
public:
    ScreenCaptureWindows( unsigned int frameWidth, unsigned int frameHeight );
    ~ScreenCaptureWindows();

    void Capture();
    UINT8* GetPixelDataPointer() { return m_pixelData; }

private:
    unsigned int m_windowWidth;
    unsigned int m_windowHeight;
    UINT8* m_pixelData;
    HWND m_hWnd;
    HDC m_hWindowDC;
    HDC m_hCaptureDC;
    HBITMAP m_hCaptureBitmap;
    BITMAPINFO m_info;

};

#endif // !SCREENCAPTURE_H
