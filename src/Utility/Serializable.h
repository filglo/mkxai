#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <iostream>
#include <fstream>

class Serializable
{
public:
    Serializable() {}
    virtual ~Serializable() {}

    void Save( const std::string& filename )
    {
        std::ofstream file;
        file.open( filename );
        this->Serialize( file );
        file.close();
    }

    void Load( const std::string& filename )
    {
        std::ifstream file( filename );
        if( file.is_open() )
        {
            this->Deserialize( file );
            file.close();
        }
    }

private:
    virtual void Serialize( std::ostream& stream ) = 0;
    virtual void Deserialize( std::istream& stream ) = 0;
};

#endif // !SERIALIZABLE_H
