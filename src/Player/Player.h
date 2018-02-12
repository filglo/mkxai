#ifndef AI_H
#define AI_H

#include <chrono>
#include <functional>
#include "DecisionTree.h"
#include "Event.h"
#include "State.h"

class Application;

class Player
{
public:
    Player();

    void Update( std::chrono::duration<double> timeDiff );
    std::string GetState() const;
    void SetState( StateMachine<Player>::STATE state ) { m_sm.SetState( state ); }

    Event<int> m_SendInput;
    Event<double&> m_GetDistance; //- test
    Event<double&, double&> m_GetPlayersHealth;
    Event<StateMachine<Player>::STATE&> m_GetSuggestedState;

private:
    StateMachine<Player> m_sm;
};

#endif // !AI_H
