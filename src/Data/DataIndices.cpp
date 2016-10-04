#include "DataIndices.h"
#include "Parameters.h"
#include "Logger.h"

DataIndices::DataIndices( const unsigned& x, const unsigned& y, const unsigned& z, const Constants::eIndexDomainTypes indexDomain ) {
    Initialise( x, y, z, indexDomain );
}

DataIndices::DataIndices( const unsigned& x, const unsigned& y, const Constants::eIndexDomainTypes indexDomain ) {
    Initialise( x, y, 0, indexDomain );
}

DataIndices::~DataIndices( ) {

}

void DataIndices::Initialise( const unsigned& x, const unsigned& y, const unsigned& z, const Constants::eIndexDomainTypes indexDomain ) {
    if( indexDomain == Constants::eDataDomain ) {
        mDataX = x;
        mDataY = y;
        mUserX = CalculateUserX( x );
        mUserY = CalculateUserY( y );
    } else if( indexDomain == Constants::eUserDomain ) {
        mDataX = x + Parameters::Get( )->GetDataIndexOfUserMinimumLongitude( );
        mDataY = y + Parameters::Get( )->GetDataIndexOfUserMinimumLatitude( );
        mUserX = x;
        mUserY = y;
    } else {
        Logger::Get()->LogMessage( "ERROR> In DataIndices::Initialise. Incorrect index domain assignment." );
        mDataX = 0;
        mDataY = 0;
        mUserX = 0;
        mUserY = 0;
    }
    mZ = z;
}

unsigned DataIndices::GetDataX( ) const {
    return mDataX;
}

unsigned DataIndices::GetDataY( ) const {
    return mDataY;
}

unsigned DataIndices::GetUserX( ) const {
    return mUserX;
}

unsigned DataIndices::GetUserY( ) const {
    return mUserY;
}

unsigned DataIndices::GetZ( ) const {
    return mZ;
}

void DataIndices::SetDataX( const unsigned& dataX ) {
    mDataX = dataX;
}

void DataIndices::SetDataY( const unsigned& dataY ) {
    mDataY = dataY;
}

void DataIndices::SetUserX( const unsigned& userX ) {
    mUserX = userX;
}

void DataIndices::SetUserY( const unsigned& userY ) {
    mUserY = userY;
}

void DataIndices::SetZ( const unsigned& z ) {
    mZ = z;
}

unsigned DataIndices::CalculateUserX( const unsigned& x ) const {

    unsigned userX;

    if( x <= Parameters::Get( )->GetDataIndexOfUserMinimumLongitude( ) ) {
        userX = 0;
    } else if( x >= Parameters::Get( )->GetDataIndexOfUserMaximumLongitude( ) ) {
        userX = Parameters::Get( )->GetLengthUserLongitudeArray( ) - 1;
    } else {
        userX = x - Parameters::Get( )->GetDataIndexOfUserMinimumLongitude( );
    }

    return userX;
}

unsigned DataIndices::CalculateUserY( const unsigned& y ) const {

    unsigned userY;

    if( y <= Parameters::Get( )->GetDataIndexOfUserMinimumLatitude( ) ) {
        userY = 0;
    } else if( y >= Parameters::Get( )->GetDataIndexOfUserMaximumLatitude( ) ) {
        userY = Parameters::Get( )->GetLengthUserLatitudeArray( ) - 1;
    } else {
        userY = y - Parameters::Get( )->GetDataIndexOfUserMinimumLatitude( );
    }

    return userY;
}