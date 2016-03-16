#ifndef PROCESSOR
#define	PROCESSOR

#include "Types.h"

class Processor {
public:
    ~Processor( );
    static Types::ProcessorPointer Get( );

    int Indices2DToIndex( const int&, const int&, const unsigned int& ) const;
    int Indices3DToIndex( const int&, const int&, const int&, const unsigned int&, const unsigned int& ) const;
    int Indices4DToIndex( const int&, const int&, const int&, const int&, const unsigned int&, const unsigned int&, const unsigned int& ) const;

    int FindVariableIndexOfValue( const Types::VariablePointer, const double& ) const;
    int CalculateVariableIndexOfValue( const Types::VariablePointer, const double& ) const;
    //int FindArrayIndexOfValue( const float*, const unsigned int&, const double& ) const;
    int CalculateArrayIndexOfValue( const float*, const unsigned int&, const double& ) const;
    
    template< class T, unsigned int N >
    unsigned int SizeOfArray( const T( &array )[ N ] ) const {
        // N has same value as count:
        unsigned int count = sizeof ( array ) / sizeof ( array[ 0 ] );

        return count;
    }
    
private:
    Processor( );
    
    static Types::ProcessorPointer mThis;
};

#endif