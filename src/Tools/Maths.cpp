#include "Maths.h"

//#include "Parameters.h"

#include <math.h>
#include <cmath> 

Types::MathsPointer Maths::mThis = NULL;

Types::MathsPointer Maths::Get( ) {
    if( mThis == NULL ) {
        mThis = new Maths( );
    }
    return mThis;
}

Maths::~Maths( ) {
    if( mThis != NULL ) {
        delete mThis;
    }
}

Maths::Maths( ) {

}

// int Maths::RoundWithProbability( const double value ) const {
//
//    int flooredValue = RoundDown( value );
//
//    double probability = value - flooredValue;
//
//    if( Parameters::Get( )->GetRandom( )->UniformDouble( ) < probability ) {
//        return flooredValue + 1;
//    } else {
//        return flooredValue;
//    }
//}

int Maths::Floor( const double& value ) const {
    return static_cast < int >( ::floor( value ) );
}

int Maths::Ceil( const double& value ) const {
    return static_cast < int >( ::ceil( value ) );
}

int Maths::Round( const double& value ) const {
    return static_cast < int >( ::floor( value + 0.5 ) );
}

double Maths::Abs( const double& value ) const {
    return std::abs( value );
}

double Maths::Min( const double& a, const double& b ) const {
    return ( a < b ) ? a : b;
}

double Maths::Max( const double& a, const double& b ) const {
    return ( a > b ) ? a : b;
}

double Maths::Log( const double& value ) const {
    return log( value );
}

double Maths::Log10( const double& value ) const {
    return log10( value );
}

double Maths::Exp( const double& exponent ) const {
    return exp( exponent );
}

double Maths::Pow( const double& base, const double& exponent ) const {
    return pow( base, exponent );
}

double Maths::NthRoot( const double& root, const double& degree ) const {
    return pow( root, 1.0 / degree );
}

unsigned int Maths::Mod( const unsigned int& dividend, const unsigned int& divisor ) const {
    return dividend % divisor;
}