#include "IOKeyboard.h"

IOKeyboard::IOKeyboard()
{
    m_keyStates.resize( 256 );
}

IOKeyboard::~IOKeyboard()
{
    ReleaseAllKeys();
}

void IOKeyboard::ProcessInput( int key )
{
    ReleaseAllKeys();
    SendKey( key );
}

// Detects if key was pressed between function executions
bool IOKeyboard::WasKeyPressed( int key )
{
    bool kp = GetAsyncKeyState( key ) & 0x8000;
    bool ret = kp && !m_keyStates[key];
    m_keyStates[key] = kp;
    return ret;
}

void IOKeyboard::SendKey( int key )
{
    auto ip = CreateInput( key );
    ip.ki.dwFlags = 0;
    SendInput( 1, &ip, sizeof( INPUT ) );

    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput( 1, &ip, sizeof( INPUT ) );
}

void IOKeyboard::PressKey( int key )
{
    auto ip = CreateInput( key );
    SendInput( 1, &ip, sizeof( INPUT ) );

    m_pressedKeys.push_back( key );
}

void IOKeyboard::ReleaseKey( int key )
{
    auto it = std::find( m_pressedKeys.begin(), m_pressedKeys.end(), key );
    if( it != m_pressedKeys.end() )
    {
        auto ip = CreateInput( key );
        ip.ki.dwFlags = ip.ki.dwFlags + KEYEVENTF_KEYUP;
        SendInput( 1, &ip, sizeof( INPUT ) );

        m_pressedKeys.erase( it );
    }
}

void IOKeyboard::ReleaseAllKeys()
{
    while( m_pressedKeys.size() != 0 )
    {
        ReleaseKey( m_pressedKeys.back() );
    }
}

INPUT IOKeyboard::CreateInput( int key )
{
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.wVk = static_cast<WORD>( key );
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    ip.ki.dwFlags = 0;
    return ip;
}
