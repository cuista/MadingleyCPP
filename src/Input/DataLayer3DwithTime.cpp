#include "DataLayer3DwithTime.h"

#include "DataCoords.h"
#include "Logger.h"
#include "Processor.h"
#include "Parameters.h"
#include "DataIndices.h"
#include "DateTime.h"

DataLayer3DwithTime::DataLayer3DwithTime( const std::string& name, const Types::VariableVector variableVector, const Types::VariablePointer longitudeVariable, const Types::VariablePointer latitudeVariable, const Types::VariablePointer depthVariable, const Types::VariablePointer timeVariable ) {
    mName = name;
    mVariableVector = variableVector;
    mLongitudeVariable = longitudeVariable;
    mLatitudeVariable = latitudeVariable;
    mDepthVariable = depthVariable;
    mTimeVariable = timeVariable;
}

DataLayer3DwithTime::~DataLayer3DwithTime( ) {
    delete mDepthVariable;
    delete mTimeVariable;
}

float DataLayer3DwithTime::GetDataAtGeoCoord( const Types::DataCoordsPointer coord ) const {
    return GetDataAtGeoCoordForVariable( coord, mVariableVector[ 0 ] );
}

float DataLayer3DwithTime::GetDataAtGeoCoordFor( const Types::DataCoordsPointer coord, const std::string& variableName ) const {
    return GetDataAtGeoCoordForVariable( coord, GetVariable( variableName ) );
}

float DataLayer3DwithTime::GetDataAtIndices( const Types::DataIndicesPointer indices ) const {
    return GetDataAtIndicesForVariable( indices, mVariableVector[ 0 ] );
}

float DataLayer3DwithTime::GetDataAtIndicesFor( const Types::DataIndicesPointer indices, const std::string& variableName ) const {
    return GetDataAtIndicesForVariable( indices, GetVariable( variableName ) );
}

Types::VariablePointer DataLayer3DwithTime::GetDepthVariable( ) const {
    return mDepthVariable;
}

Types::VariablePointer DataLayer3DwithTime::GetTimeVariable( ) const {
    return mTimeVariable;
}

float DataLayer3DwithTime::GetDataAtGeoCoordForVariable( const Types::DataCoordsPointer coord, const Types::VariablePointer variable ) const {

    int xIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLongitudeVariable, coord->GetLongitude( ) );
    int yIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLatitudeVariable, coord->GetLatitude( ) );
    int zIndex = Processor::Get( )->FindVariableIndexOfValue( mDepthVariable, coord->GetDepth( ) );
    unsigned tIndex = DateTime::Get( )->GetTimeStep( );
    unsigned xMax = Parameters::Get( )->GetLengthDataLongitudeArray( );
    unsigned yMax = Parameters::Get( )->GetLengthDataLatitudeArray( );
    unsigned zMax = mDepthVariable->GetSize( );

    return variable->GetDataAtIndex( Processor::Get( )->Indices4DToIndex( xIndex, yIndex, zIndex, tIndex, xMax, yMax, zMax ) );
}

float DataLayer3DwithTime::GetDataAtIndicesForVariable( const Types::DataIndicesPointer indices, const Types::VariablePointer variable ) const {

    unsigned xIndex = indices->GetDataX( );
    unsigned yIndex = indices->GetDataY( );
    unsigned zIndex = indices->GetZ( );
    unsigned xMax = Parameters::Get( )->GetLengthDataLongitudeArray( );
    unsigned yMax = Parameters::Get( )->GetLengthDataLatitudeArray( );
    unsigned tIndex = DateTime::Get( )->GetTimeStep( );
    unsigned zMax = mDepthVariable->GetSize( );

    return variable->GetDataAtIndex( Processor::Get( )->Indices4DToIndex( xIndex, yIndex, zIndex, tIndex, xMax, yMax, zMax ) );
}