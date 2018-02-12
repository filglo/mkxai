#ifndef IOKEYBOARD_H
#define IOKEYBOARD_H

#include <Windows.h>
#include <vector>
#include "IOInterface.h"

class IOKeyboard : public IOInterface
{
public:
    IOKeyboard();
    ~IOKeyboard();

    void ProcessInput( int key ) override;
    bool WasKeyPressed( int key ) override;
    void Update( std::chrono::duration<double> timeDiff ) override {}

private:
    void SendKey( int key );
    void PressKey( int key );
    void ReleaseKey( int key );
    void ReleaseAllKeys();
    INPUT CreateInput( int key );

    std::vector<int> m_pressedKeys;
    std::vector<bool> m_keyStates;
};

#endif // !IOKEYBOARD_H
