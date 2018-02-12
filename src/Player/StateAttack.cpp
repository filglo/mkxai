#include "StateAttack.h"

#include "Keycodes.h"

StateAttack::StateAttack( Player& owner, StateMachine<Player>& stateMachine )
    : State<Player>( owner, stateMachine )
    , m_moves( {
        Keycodes::GPKEY::NONE,
        Keycodes::GPKEY::A,
        Keycodes::GPKEY::B,
        Keycodes::GPKEY::X,
        Keycodes::GPKEY::Y,
        Keycodes::GPKEY::UP,
        Keycodes::GPKEY::DOWN,
        Keycodes::GPKEY::LEFT,
        Keycodes::GPKEY::RIGHT } )
    , m_tree( m_moves, 5 )
    , m_actionDelay( 0.35 )
    , m_hasFinished( true )
{
    m_owner.m_GetPlayersHealth( m_playerHP, m_enemyHP );
    m_timer = m_actionDelay;
    m_tree.Load( "AttackTree" );
}

StateAttack::~StateAttack()
{
    m_tree.Save( "AttackTree" );
}

void StateAttack::Enter()
{
    m_hasFinished = false;
    m_timer = m_actionDelay;
    m_owner.m_GetPlayersHealth( m_playerHP, m_enemyHP );
}

void StateAttack::Update( std::chrono::duration<double> diff )
{
    m_timer += diff;
    double distance = 0.0;
    m_owner.m_GetDistance( distance );
    if( std::abs( distance ) > 0.5 && m_hasFinished )
    {
        m_stateMachine.SetState( StateMachine<Player>::STATE::APPROACH );
        return;
    }
    // reverse timer - action delay? - timer countdown + reset?
    if( m_timer >= m_actionDelay )
    {
        m_timer -= m_actionDelay;
        m_owner.m_SendInput( m_tree.Step() );
        if( m_tree.HasFinished() )
        {
            double newPlayerHP, newEnemyHP;
            m_owner.m_GetPlayersHealth( newPlayerHP, newEnemyHP );
            double playerHPDiff = newPlayerHP - m_playerHP;
            double enemyHPDiff = newEnemyHP - m_enemyHP;
            if( enemyHPDiff > 0.5 )
            {
                enemyHPDiff = newEnemyHP - 1.0;
            }
            m_tree.Playout( playerHPDiff - enemyHPDiff );
            m_playerHP = newPlayerHP;
            m_enemyHP = newEnemyHP;
            m_hasFinished = true;
        }
    }
}

void StateAttack::Exit()
{

}
