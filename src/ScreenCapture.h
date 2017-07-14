#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H

#include <Windows.h>
#include <cstdint>

class ScreenCapture
{
public:
    ScreenCapture( unsigned int frameWidth, unsigned int frameHeight );
    ~ScreenCapture();

    void Capture();
    UINT8* GetPixelDataPointer() { return m_pixelData; }

private:
    unsigned int m_imageWidth;
    unsigned int m_imageHeight;
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
