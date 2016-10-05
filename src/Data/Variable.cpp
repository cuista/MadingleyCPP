#include "Variable.h"

#include "Constants.h"
#include "Logger.h"
#include "Convertor.h"

Variable::Variable( const std::string& name, const Types::UnsignedVector& dimensions, const unsigned& size, float* data, const bool isDefault ) {
    mName = name;
    mDimensions = dimensions;
    mSize = size;
    mData = data;
    mIsDefault = isDefault;

    mMaximumDataDifference = Constants::cMissingValue; // Is only set on axes (e.g. latitude) when variable type is known.
}

Variable::~Variable( ) {
    delete[ ] mData;
}

std::string Variable::GetName( ) const {
    return mName;
}

Types::UnsignedVector Variable::GetDimensions( ) const {
    return mDimensions;
}

unsigned Variable::GetDimension( const unsigned& index ) const {
    return mDimensions[ index ];
}

unsigned Variable::GetSize( ) const {
    return mSize;
}

float* Variable::GetData( ) const {
    return mData;
}

float Variable::GetDataAtIndex( const int& index ) const {
    float data = Constants::cMissingValue;

    if( index != Constants::cMissingValue ) {
        if( index < ( int )mSize ) {
            data = mData[ index ];
        } else {
            Logger::Get( )->LogMessage( "ERROR> Index of " + Convertor::Get( )->ToString( index ) + " exceeds size (" + Convertor::Get( )->ToString( mSize ) + ") of variable \"" + mName + "\"." );
        }
    } else {
        Logger::Get( )->LogMessage( "ERROR> Index of " + Convertor::Get( )->ToString( index ) + " is invalid." );
    }

    return data;
}

float Variable::GetMaximumDataDifference( ) const {
    return mMaximumDataDifference;
}

bool Variable::IsDefault( ) const {
    return mIsDefault;
}

void Variable::SetData( float* data, const unsigned& size ) {
    mSize = size;
    delete[ ] mData;
    mData = data;
}

void Variable::SetDataAtIndex( const int& index, const float& data ) {
    if( index != Constants::cMissingValue ) {
        if( index < ( int )mSize ) {
            mData[ index ] = data;
        } else {
            Logger::Get( )->LogMessage( "ERROR> Index of " + Convertor::Get( )->ToString( index ) + " exceeds size (" + Convertor::Get( )->ToString( mSize ) + ") of variable \"" + mName + "\"." );
        }
    } else {
        Logger::Get( )->LogMessage( "ERROR> Index of " + Convertor::Get( )->ToString( index ) + " is invalid." );
    }
}

void Variable::SetMaximumDataDifference( const float& maximumDataDifference ) {
    mMaximumDataDifference = maximumDataDifference;
}