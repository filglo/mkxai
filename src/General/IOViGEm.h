#ifndef IOVIGEM_H
#define IOVIGEM_H

#include <Windows.h>
#include <chrono>
#include <map>
#include <initguid.h>
#include <ViGEmUM.h>
#include "IOInterface.h"

class IOVigem : public IOInterface
{
public:
    IOVigem();
    ~IOVigem();

    void ProcessInput( int key ) override;
    bool WasKeyPressed( int key ) override;
    void Update( std::chrono::duration<double> timeDiff ) override;

private:
    void SetOutput( int key );
    void HoldKey( int key, std::chrono::duration<double> duration );
    void PressKey( int key );
    void ReleaseKey( int key );
    void ReleaseAllKeys();

    VIGEM_TARGET m_x360;
    std::map<int, std::chrono::duration<double>> m_releaseTimers;
    XUSB_REPORT m_report;
};

#endif // !IOVIGEM_H
