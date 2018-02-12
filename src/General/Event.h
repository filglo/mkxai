#ifndef EVENT_H
#define EVENT_H

#include <vector>
#include <algorithm>

// this needs cleanup

template<class... Args> class Event;

class BaseDispatcher
{
public:
    virtual ~BaseDispatcher() { }
};

template<class... Args> class AbstractDispatcher : public BaseDispatcher
{
protected:
    virtual ~AbstractDispatcher();

    friend class Event<Args...>;

    void add( Event<Args...> *s ) { v.push_back( s ); }
    void remove( Event<Args...> *s ) { v.erase( std::remove( v.begin(), v.end(), s ), v.end() ); }

    virtual void call( Args... args ) = 0;

    std::vector<Event<Args...>*> v;
};

template<class T, class... Args> class ConcreteDispatcher : public AbstractDispatcher<Args...>
{
public:
    ConcreteDispatcher( T *t, void(T::*f)(Args...), Event<Args...> &s );

private:
    ConcreteDispatcher( const ConcreteDispatcher& );

    friend class Event<Args...>;

    virtual void call( Args... args ) { (t->*f)(args...); }

    T *t;
    void(T::*f)(Args...);
};

template<class... Args> class Event
{
public:
    Event() { }
    ~Event() { for( auto i : v ) i->remove( this ); }

    void connect( AbstractDispatcher<Args...> &s ) { v.push_back( &s ); s.add( this ); }
    void disconnect( AbstractDispatcher<Args...> &s ) { v.erase( std::remove( v.begin(), v.end(), &s ), v.end() ); }

    void operator()( Args... args ) { for( auto i : v ) i->call( args... ); }

private:
    Event( const Event& );

    std::vector<AbstractDispatcher<Args...>*> v;
};

template<class... Args> AbstractDispatcher<Args...>::~AbstractDispatcher()
{
    for( auto i : v ) i->disconnect( *this );
}

template<class T, class... Args> ConcreteDispatcher<T, Args...>::ConcreteDispatcher( T *t, void(T::*f)(Args...), Event<Args...> &s ) : t( t ), f( f )
{
    s.connect( *this );
}

class Dispatcher
{
public:
    Dispatcher() { }
    ~Dispatcher() { for( auto i : v ) delete i; }

    template<class T, class... Args> void connect( T *t, void(T::*f)(Args...), Event<Args...> &s ) { v.push_back( new ConcreteDispatcher<T, Args...>( t, f, s ) ); }

private:
    Dispatcher( const Dispatcher& );

    std::vector<BaseDispatcher*> v;
};

#endif // !EVENT_H
