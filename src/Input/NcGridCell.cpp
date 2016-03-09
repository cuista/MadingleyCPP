#include "NcGridCell.h"

#include "Constants.h"
#include <iostream>

NcGridCell::NcGridCell( const int realm ) {
    mRealm = realm;
}

NcGridCell::~NcGridCell( ) {

}

void NcGridCell::InitialiseVectors( const unsigned int lengthTimeArray ) {

    mDiurnalTemperatureRange.resize( lengthTimeArray, Constants::cMissingValue );
    mGroundFrostFrequency.resize( lengthTimeArray, Constants::cMissingValue );
    mPrecipitation.resize( lengthTimeArray, Constants::cMissingValue );
    mNearSurfaceTemperature.resize( lengthTimeArray, Constants::cMissingValue );
    mNPP.resize( lengthTimeArray, Constants::cMissingValue );
    mOceanNPP.resize( lengthTimeArray, Constants::cMissingValue );
    mSST.resize( lengthTimeArray, Constants::cMissingValue );
    mUSpeed.resize( lengthTimeArray, Constants::cMissingValue );
    mVSpeed.resize( lengthTimeArray, Constants::cMissingValue );
}

int NcGridCell::GetRealm( ) const {
    return mRealm;
}

double NcGridCell::GetWaterCapacity(  ) const {
    return mWaterCapacity;
}

double NcGridCell::GetDiurnalTemperatureRange( const unsigned int timeIndex ) const {
    return mDiurnalTemperatureRange[ timeIndex ];
}

double NcGridCell::GetPrecipitation( const unsigned int timeIndex ) const {
    return mPrecipitation[ timeIndex ];
}

double NcGridCell::GetGroundFrostFrequency( const unsigned int timeIndex ) const {
    return mGroundFrostFrequency[ timeIndex ];
}

double NcGridCell::GetNearSurfaceTemperature( const unsigned int timeIndex ) const {
    return mNearSurfaceTemperature[ timeIndex ];
}

double NcGridCell::GetUSpeed( const unsigned int timeIndex ) const {
    return mUSpeed[ timeIndex ];
}

double NcGridCell::GetVSpeed( const unsigned int timeIndex ) const {
    return mVSpeed[ timeIndex ];
}

double NcGridCell::GetSST( const unsigned int timeIndex ) const {
    return mSST[ timeIndex ];
}

double NcGridCell::GetNPP( const unsigned int timeIndex ) const {
    return mNPP[ timeIndex ];
}

double NcGridCell::GetOceanNPP( const unsigned int timeIndex ) const {
    return mOceanNPP[ timeIndex ];
}

void NcGridCell::SetRealm( const int realm ) {
    mRealm = realm;
}

void NcGridCell::SetWaterCapacity( const double WaterCapacity ) {
    mWaterCapacity = WaterCapacity;
}

void NcGridCell::SetDiurnalTemperatureRange( const double diurnalTemperatureRange, const unsigned int timeIndex ) {
    mDiurnalTemperatureRange[ timeIndex ] = diurnalTemperatureRange;
}

void NcGridCell::SetPrecipitation( const double precipitation, const unsigned int timeIndex ) {
    mPrecipitation[ timeIndex ] = precipitation;
}

void NcGridCell::SetGroundFrostFrequency( const double groundFrostFrequency, const unsigned int timeIndex ) {
    mGroundFrostFrequency[ timeIndex ] = groundFrostFrequency;
}

void NcGridCell::SetNearSurfaceTemperature( const double nearSurfaceTemperature, const unsigned int timeIndex ) {
    mNearSurfaceTemperature[ timeIndex ] = nearSurfaceTemperature;
}

void NcGridCell::SetUSpeed( const double USpeed, const unsigned int timeIndex ) {
    mUSpeed[ timeIndex ] = USpeed;
}

void NcGridCell::SetVSpeed( const double VSpeed, const unsigned int timeIndex ) {
    mVSpeed[ timeIndex ] = VSpeed;
}

void NcGridCell::SetSST( const double SST, const unsigned int timeIndex ) {
    mSST[ timeIndex ] = SST;
}

void NcGridCell::SetNPP( const double NPP, const unsigned int timeIndex ) {
    mNPP[ timeIndex ] = NPP;
}

void NcGridCell::SetOceanNPP( const double OceanNPP, const unsigned int timeIndex ) {
    mOceanNPP[ timeIndex ] = OceanNPP;
}