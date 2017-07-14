#ifndef DECISIONTREE_H
#define DECISIONTREE_H

#include <vector>
#include <memory>
#include <map>
#include "Serializable.h"

class DecisionTree : public Serializable
{
public:
    DecisionTree( const std::vector<int>& legalMoves, unsigned int maxDepth );

    int Step();
    void Playout( double reward );
    bool HasFinished() const;
    int64_t GetNodeCount() const { return m_nodeCount; }

private:
    struct Node : public Serializable
    {
        Node* parent;
        std::map<int, Node> children;
        double reward;
        unsigned int tries;
        bool dead;

        void Serialize( std::ostream& stream ) override;
        void Deserialize( std::istream& stream ) override;
    };

    void AddNode( Node* parent, int move );
    void Backpropagate( Node* node, double reward );
    int DecideNextMove( const Node* node ) const;
    void Prune( Node* node );

    void Serialize( std::ostream& stream ) override;
    void Deserialize( std::istream& stream ) override;

    std::unique_ptr<Node> m_root;
    std::vector<int> m_legalMoves;
    unsigned int m_maxDepth;

    Node* m_currentNode;
    unsigned int m_currentDepth;
    int m_currentMove;
    int64_t m_nodeCount;
};

#endif // !DECISIONTREE_H
