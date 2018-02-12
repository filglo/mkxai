#include "State.h"

#include "StateApproach.h"
#include "StateAttack.h"

StateMachine<Player>::StateMachine( Player& owner )
    : m_owner( owner )
{
    m_states[STATE::APPROACH].reset( new StateApproach( owner, *this ) );
    m_states[STATE::ATTACK].reset( new StateAttack( owner, *this ) );
    m_currentState = m_states[STATE::APPROACH];
    m_currentStateId = STATE::APPROACH;
}

template<> void StateMachine<Player>::SetState( STATE newState )
{
    m_currentState->Exit();
    m_currentState = m_states[newState];
    m_currentStateId = newState;
    m_currentState->Enter();
}

void StateMachine<Player>::Update( std::chrono::duration<double> diff )
{
    m_currentState->Update( diff );
}

template <class T>
StateMachine<T>::StateMachine( T& owner )
    : m_owner( owner )
{
}

template <class T>
void StateMachine<T>::SetState( STATE newState )
{
    m_currentState->Exit();
    m_currentState = m_states[newState];
    m_currentStateId = newState;
    m_currentState->Enter();
}

template <class T>
void StateMachine<T>::Update( std::chrono::duration<double> diff )
{
    m_currentState->Update( diff );
}
