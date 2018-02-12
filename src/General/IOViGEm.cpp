#include "IOViGEm.h"

#include <vector>
#include "Keycodes.h"

IOViGEm::IOViGEm()
    : m_report( {0} )
{
    if( !VIGEM_SUCCESS( vigem_init() ) )
    {
        throw std::runtime_error( "Cannot initialize gamepad.\n" );
        return;
    }
    VIGEM_TARGET_INIT( &m_x360 );
    if( !VIGEM_SUCCESS( vigem_target_plugin( Xbox360Wired, &m_x360 ) ) )
    {
        throw std::runtime_error( "Cannot initialize gamepad.\n" );
        return;
    }
}

IOViGEm::~IOViGEm()
{
    vigem_target_unplug( &m_x360 );
    vigem_shutdown();
}

void IOViGEm::ProcessInput( int key )
{
    if( key < 4 )
    {
        PressKey( key );
    }
    else
    {
        HoldKey( key, std::chrono::duration<double>( 0.55 ) );
    }
    m_report = { 0 };
    for( auto& keyPressed : m_releaseTimers )
    {
        SetOutput( keyPressed.first );
    }
    vigem_xusb_submit_report( m_x360, m_report );
}

// Detects if key was pressed between function executions
bool IOViGEm::WasKeyPressed( int key )
{
    // pure virtual gamepad
    return false;
}

void IOViGEm::ReleaseAllKeys()
{
    SetOutput( 0 );
    m_releaseTimers.clear();
}

void IOViGEm::Update( std::chrono::duration<double> timeDiff )
{
    bool released = false;
    std::vector<int> keys;
    for( auto& keyPressed : m_releaseTimers )
    {
        keyPressed.second -= timeDiff;
        if( keyPressed.second <= std::chrono::duration<double>::zero() )
        {
            keys.push_back( keyPressed.first );
            released = true;
        }
    }
    for( auto& key : keys )
    {
        ReleaseKey( key );
    }
    keys.clear();
    if( released )
    {
        m_report = { 0 };
        for( auto& keyPressed : m_releaseTimers )
        {
            SetOutput( keyPressed.first );
        }
        vigem_xusb_submit_report( m_x360, m_report );
    }
}

void IOViGEm::HoldKey( int key, std::chrono::duration<double> duration )
{
    m_releaseTimers[key] = duration;
}

void IOViGEm::PressKey( int key )
{
    m_releaseTimers[key] = std::chrono::duration<double>(0.1);
}

void IOViGEm::ReleaseKey( int key )
{
    if( m_releaseTimers.count( key ) )
    {
        m_releaseTimers.erase( key );
    }
}

void IOViGEm::SetOutput( int key )
{
    switch( key )
    {
    case Keycodes::GPKEY::A:
        m_report.wButtons += XUSB_GAMEPAD_A;
        break;
    case Keycodes::GPKEY::B:
        m_report.wButtons += XUSB_GAMEPAD_B;
        break;
    case Keycodes::GPKEY::X:
        m_report.wButtons += XUSB_GAMEPAD_X;
        break;
    case Keycodes::GPKEY::Y:
        m_report.wButtons += XUSB_GAMEPAD_Y;
        break;
    case Keycodes::GPKEY::DOWN:
        m_report.wButtons += XUSB_GAMEPAD_DPAD_DOWN;
        break;
    case Keycodes::GPKEY::LEFT:
        m_report.wButtons += XUSB_GAMEPAD_DPAD_LEFT;
        break;
    case Keycodes::GPKEY::RIGHT:
        m_report.wButtons += XUSB_GAMEPAD_DPAD_RIGHT;
        break;
    case Keycodes::GPKEY::UP:
        m_report.wButtons += XUSB_GAMEPAD_DPAD_UP;
        break;
    default:
        m_report = { 0 };
        break;
    }
}
