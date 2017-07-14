#ifndef STATEATTACK_H
#define STATEATTACK_H

#include "State.h"
#include "../Player.h"

class StateAttack : public State<Player>
{
public:
    StateAttack( Player& owner, StateMachine<Player>& stateMachine );
    ~StateAttack();

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

#endif // !STATEATTACK_H
