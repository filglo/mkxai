#include "StateApproach.h"
#include "Keycodes.h"

StateApproach::StateApproach( Player& owner, StateMachine<Player>& stateMachine )
    : State<Player>( owner, stateMachine )
    , m_moves( { 0, 1, 2, 3, 4, 5, 6, 7, 8 } )
    , m_tree( m_moves, 5 )
    , m_actionDelay( 0.35 )
    , m_hasFinished( true )
{
    m_owner.m_GetPlayersHealth( m_playerHP, m_enemyHP );
    m_timer = m_actionDelay;
}

void StateApproach::Enter()
{
    m_hasFinished = false;
    m_timer = m_actionDelay;
    m_owner.m_GetPlayersHealth( m_playerHP, m_enemyHP );
}

void StateApproach::Update( std::chrono::duration<double> diff )
{
    double distance = 0;
    m_timer += diff;
    m_owner.m_GetDistance( distance );
    if( std::abs( distance ) < 0.2 && m_hasFinished )
    {
        m_stateMachine.SetState( StateMachine<Player>::STATE::ATTACK );
        return;
    }
    m_hasFinished = true;
    if( distance > 0.0 )
    {
        m_owner.m_SendInput( Keycodes::GPKEY::LEFT );
    }
    else
    {
        m_owner.m_SendInput( Keycodes::GPKEY::RIGHT );
    }
}

void StateApproach::Exit()
{

}
