#include "Player.h"

#include "../Application.h"

Player::Player()
    : m_sm( *this )
{
}

void Player::Update( std::chrono::duration<double> diff )
{
    m_sm.Update( diff );
}

std::string Player::GetState() const
{
    if( m_sm.GetState() == StateMachine<Player>::STATE::APPROACH )
    {
        return "Approach";
    }
    else if( m_sm.GetState() == StateMachine<Player>::STATE::ATTACK )
    {
        return "Attack";
    }
    else if( m_sm.GetState() == StateMachine<Player>::STATE::BLOCK )
    {
        return "Block";
    }
    return "Unindentified State";
}
