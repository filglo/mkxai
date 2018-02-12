#ifndef STATE_H
#define STATE_H

#include <memory>
#include <chrono>
#include <map>

template <class T> class StateMachine;

template <class T>
class State
{
public:
    State( T& owner, StateMachine<T>& stateMachine ) : m_owner( owner ), m_stateMachine( stateMachine ), m_timer( 0 ) {}

    virtual void Enter() = 0;
    virtual void Update( std::chrono::duration<double> diff ) = 0;
    virtual void Exit() = 0;

protected:
    T& m_owner;
    StateMachine<T>& m_stateMachine;
    std::chrono::duration<double> m_timer;
};

template <class T>
class StateMachine
{
public:
    enum class STATE
    {
        APPROACH,
        ATTACK,
        DODGE,
        BLOCK,
        SPECIAL
    };

    StateMachine( T& owner );

    void SetState( STATE newState );
    STATE GetState() const { return m_currentStateId; }
    void Update( std::chrono::duration<double> diff );

private:
    T& m_owner;
    std::shared_ptr<State<T>> m_currentState;
    STATE m_currentStateId;
    std::map<STATE, std::shared_ptr<State<T>>> m_states;
};

class Player;

StateMachine<Player>::StateMachine( Player& owner );
void StateMachine<Player>::SetState( STATE newState );
void StateMachine<Player>::Update( std::chrono::duration<double> diff );

#endif // !STATE_H
