#ifndef STATEAPPROACH_H
#define STATEAPPROACH_H

#include "State.h"
#include "Player.h"

class StateApproach : public State<Player>
{
public:
    StateApproach( Player& owner, StateMachine<Player>& stateMachine );

    void Enter() override;
    void Update( std::chrono::duration<double> diff ) override;
    void Exit() override;

private:
    std::vector<int> m_moves;
    DecisionTree m_tree;
    const std::chrono::duration<double> m_actionDelay;
    double m_playerHP;
    double m_enemyHP;
    bool m_hasFinished;
};

#endif // !STATEAPPROACH_H
