#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

class InitException : public std::runtime_error
{
    //const char * what() const throw () {
    //    return (std::string( "Initialization Exception" )).c_str();
    //}
};

#endif // !EXCEPTIONS_H
