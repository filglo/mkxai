#include "ScreenCapture.h"

#include <stdexcept>

ScreenCapture::ScreenCapture( unsigned int frameWidth, unsigned int frameHeight )
{
    m_imageWidth = frameWidth;
    m_imageHeight = frameHeight;

    //Screen capture buffer
    RECT screenRect;
    m_hWnd = ::FindWindow(NULL, "MK10");
    if( m_hWnd == NULL )
    {
        throw std::runtime_error( "Cannot find game window./n" );
    }
    GetClientRect( m_hWnd, &screenRect );
    m_windowWidth = screenRect.right - screenRect.left;
    m_windowHeight = screenRect.bottom - screenRect.top;
    m_hWindowDC = GetDC( m_hWnd );
    m_hCaptureDC = CreateCompatibleDC( m_hWindowDC );
    m_hCaptureBitmap = CreateCompatibleBitmap( m_hWindowDC, m_imageWidth, m_imageHeight );
    SelectObject( m_hCaptureDC, m_hCaptureBitmap );
    SetStretchBltMode( m_hCaptureDC, COLORONCOLOR );

    auto bitCount = 32;
    auto length = m_imageHeight * m_imageWidth * bitCount / 8;
    m_pixelData = (UINT8*)malloc( length );
    ZeroMemory( &m_info, sizeof( BITMAPINFO ) );
    m_info.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
    m_info.bmiHeader.biPlanes = 1;
    m_info.bmiHeader.biBitCount = bitCount;
    m_info.bmiHeader.biWidth = m_imageWidth;
    m_info.bmiHeader.biHeight = -static_cast<int64_t>(m_imageHeight);
    m_info.bmiHeader.biCompression = BI_RGB;
    m_info.bmiHeader.biSizeImage = 0;
}

ScreenCapture::~ScreenCapture()
{
    free( m_pixelData );
    ReleaseDC( m_hWnd, m_hWindowDC );
    DeleteDC( m_hCaptureDC );
    DeleteObject( m_hCaptureBitmap );
}

void ScreenCapture::Capture()
{
    StretchBlt( m_hCaptureDC, 0, 0, m_imageWidth, m_imageHeight, m_hWindowDC, 0, 0, m_windowWidth, m_windowHeight, SRCCOPY );
    GetDIBits( m_hCaptureDC, m_hCaptureBitmap, 0, m_imageHeight, m_pixelData, (BITMAPINFO*)&m_info, DIB_RGB_COLORS );
}
