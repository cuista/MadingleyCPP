#include "DataIndices.h"

DataIndices::DataIndices( const unsigned int& x, const unsigned int& y, const unsigned int& z ) {
    mX = x;
    mY = y;
    mZ = z;
}

DataIndices::DataIndices( const unsigned int& x, const unsigned int& y ) {
    mX = x;
    mY = y;
    mZ = 0;
}

DataIndices::~DataIndices( ) {

}

unsigned int DataIndices::GetX( ) const {
    return mX;
}

unsigned int DataIndices::GetY( ) const {
    return mY;
}

unsigned int DataIndices::GetZ( ) const {
    return mZ;
}

void DataIndices::SetX( const unsigned int& x ) {
    mX = x;
}

void DataIndices::SetY( const unsigned int& y ) {
    mY = y;
}

void DataIndices::SetZ( const unsigned int& z ) {
    mZ = z;
}