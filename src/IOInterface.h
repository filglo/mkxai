#ifndef IOINTERFACE_H
#define IOINTERFACE_H

#include <chrono>

class IOInterface
{
public:
    IOInterface();
    virtual ~IOInterface();

    virtual void ProcessInput( int key ) = 0;
    virtual bool WasKeyPressed( int key ) = 0;
    virtual void Update( std::chrono::duration<double> timeDiff ) = 0;
};

class IODummyInterface : public IOInterface
{
public:
    IODummyInterface() {}
    ~IODummyInterface() {}

    void ProcessInput( int key ) override {}
    bool WasKeyPressed( int key ) override { return false; }
    void Update( std::chrono::duration<double> timeDiff ) override {}

};

#endif // !IOINTERFACE_H
