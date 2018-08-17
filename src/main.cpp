#include "Application.h"

int main()
{
    try
    {
        Application app;
        app.Run();
    }
    catch( std::exception& e )
    {
        OutputDebugString( std::string(e.what() + std::string("Unhandled exception in main.\n")).c_str() );
    }

    return 0;
}
