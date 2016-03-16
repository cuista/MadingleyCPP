#include "Indices.h"

Indices::Indices( const unsigned int& x, const unsigned int& y, const unsigned int& z ) {
    mX = x;
    mY = y;
    mZ = z;
}

Indices::Indices( const unsigned int& x, const unsigned int& y ) {
    mX = x;
    mY = y;
    mZ = 0;
}

Indices::~Indices( ) {

}

unsigned int Indices::GetX( ) const {
    return mX;
}

unsigned int Indices::GetY( ) const {
    return mY;
}

unsigned int Indices::GetZ( ) const {
    return mZ;
}

void Indices::SetX( const unsigned int& x ) {
    mX = x;
}

void Indices::SetY( const unsigned int& y ) {
    mY = y;
}

void Indices::SetZ( const unsigned int& z ) {
    mZ = z;
}