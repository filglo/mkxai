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
        OutputDebugString( e.what() );
    }

    return 0;
}
