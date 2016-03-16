#include "GeoCoord.h"

GeoCoord::GeoCoord( const float& longitude, const float& latitude, const float& depth ) {
    mLongitude = longitude;
    mLatitude = latitude;
    mDepth = depth;
}

GeoCoord::GeoCoord( const float& longitude, const float& latitude ) {
    mLongitude = longitude;
    mLatitude = latitude;
    mDepth = 0;
}

GeoCoord::~GeoCoord( ) {

}

float GeoCoord::GetLongitude( ) const {
    return mLongitude;
}

float GeoCoord::GetLatitude( ) const {
    return mLatitude;
}

float GeoCoord::GetDepth( ) const {
    return mDepth;
}

void GeoCoord::SetLongitude( const float& longitude ) {
    mLongitude = longitude;
}

void GeoCoord::SetLatitude( const float& latitude ) {
    mLatitude = latitude;
}

void GeoCoord::SetDepth( const float& depth ) {
    mDepth = depth;
}