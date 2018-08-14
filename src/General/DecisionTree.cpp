#include "DecisionTree.h"

#include <algorithm>
#include "RandomNumberGenerator.h"

DecisionTree::DecisionTree( const std::vector<int>& legalMoves, unsigned int maxDepth )
    : m_root( new Node )
    , m_legalMoves( legalMoves )
    , m_maxDepth( maxDepth )
    , m_currentNode( m_root.get() )
    , m_currentDepth( 0 )
    , m_currentMove( -1 )
    , m_nodeCount( 0 )
{
    m_root->parent = nullptr;
    m_root->reward = 0;
    m_root->tries = 0;
    m_root->dead = false;
}

int DecisionTree::Step()
{
    if( !HasFinished() )
    {
        // Conditionally remove worst branch
        if( m_currentNode->children.size() >= 6 && RNG::g_rng.GetReal() < 0.05 * m_currentDepth / m_maxDepth )
        {
            auto it = std::max_element( m_currentNode->children.begin(), m_currentNode->children.end(),
                [&]( const std::pair<int, Node>& s1, const std::pair<int, Node>& s2 )
            {
                double a1 = s1.second.reward / s1.second.tries;
                double a2 = s2.second.reward / s2.second.tries;
                return a1 > a2;
            } );
            Prune( &m_currentNode->children[it->first] );
        }
        m_currentMove = DecideNextMove( m_currentNode );
        auto it = m_currentNode->children.find( m_currentMove );
        if( it == m_currentNode->children.end() )
        {
            AddNode( m_currentNode, m_currentMove );
        }
        m_currentNode = &m_currentNode->children[m_currentMove];
        m_currentDepth += 1;
        return m_currentMove;
    }
    return 0;
}

// rewards should have linear decay in time ( faster reward = better )
// branches shouldn't be too short or too long
void DecisionTree::Playout( double reward )
{
    Backpropagate( m_currentNode, reward );
    m_currentDepth = 0;
    m_currentMove = -1;
    m_currentNode = m_root.get();
}

bool DecisionTree::HasFinished() const
{
    return m_currentDepth >= m_maxDepth || m_currentMove == 0;
}

void DecisionTree::Serialize( std::ostream & stream )
{
    stream << m_nodeCount << std::endl;
    m_root->Serialize( stream );
}

void DecisionTree::Deserialize( std::istream & stream )
{
    // Not checking move list and max depth
    stream >> m_nodeCount;
    m_root->Deserialize( stream );
}

void DecisionTree::AddNode( Node* parent, int move )
{
    Node node;
    node.parent = parent;
    node.reward = 0;
    node.tries = 0;
    node.dead = false;
    parent->children.insert( std::make_pair( move, node ) );
    m_nodeCount += 1;
}

void DecisionTree::Backpropagate( Node* node, double reward )
{
    node->tries += 1;
    node->reward += reward;
    if( node->parent )
    {
        Backpropagate( node->parent, reward );
    }
}

int DecisionTree::DecideNextMove( const Node* node ) const
{
    if( node->children.size() >= 4 )
    {
        // If not all nodes are explored - with a set probability choose random node from the set of unexplored nodes
        if( node->children.size() != m_legalMoves.size() && RNG::g_rng.GetReal() < 0.3 )
        {
            auto moves = m_legalMoves;
            for( auto& child : node->children )
            {
                moves.erase( std::find( moves.begin(), moves.end(), child.first ) );
            }
            return moves[RNG::g_rng.GetUInt64( 0, moves.size() - 1 )];
        }
        // Choose next node according to UCB1 formula
        double c = 0.01; // 0.25
        auto it = std::max_element( node->children.begin(), node->children.end(),
            [&]( const std::pair<int, Node>& s1, const std::pair<int, Node>& s2 )
        {
            double a1 = s1.second.reward / s1.second.tries + std::sqrt( c * 2 * std::log( node->tries ) / s1.second.tries );
            double a2 = s2.second.reward / s2.second.tries + std::sqrt( c * 2 * std::log( node->tries ) / s2.second.tries );
            return a1 < a2;
        } );
        return it->first;
    }
    // If there are not enough explored nodes - choose one at random
    else if( !node->children.empty() )
    {
        return m_legalMoves[RNG::g_rng.GetUInt64( 0, m_legalMoves.size() - 1 )];
    }
    // If there are no explored nodes - end
    return 0;
}

void DecisionTree::Prune( Node* node )
{
    node->dead = true;
    for( auto& n : node->children )
    {
        m_nodeCount += -1;
        Prune( &n.second );
    }
    node->children.clear();
}

void DecisionTree::Node::Serialize( std::ostream & stream )
{
    stream << reward << " " << tries << " " << dead << " " << children.size() << std::endl;
    for( auto& pair : children )
    {
        stream << pair.first << std::endl;
        pair.second.Serialize( stream );
    }
}

void DecisionTree::Node::Deserialize( std::istream & stream )
{
    int childrenSize = 0;
    stream >> reward;
    stream >> tries;
    stream >> dead;
    stream >> childrenSize;
    for( int i = 0; i < childrenSize; ++i )
    {
        int move = 0;
        stream >> move;
        Node node;
        node.parent = this;
        children.insert( std::make_pair( move, node ) );
        node.Deserialize( stream );
    }
}
