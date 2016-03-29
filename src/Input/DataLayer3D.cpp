#include "DataLayer3D.h"

#include "DataCoords.h"
#include "Logger.h"
#include "Processor.h"
#include "Parameters.h"
#include "DataIndices.h"

DataLayer3D::DataLayer3D( const std::string& name, const Types::VariableVector variableVector, const Types::VariablePointer longitudeVariable, const Types::VariablePointer latitudeVariable, const Types::VariablePointer depthVariable ) {
    mName = name;
    mVariableVector = variableVector;
    mLongitudeVariable = longitudeVariable;
    mLatitudeVariable = latitudeVariable;
    mDepthVariable = depthVariable;
}

DataLayer3D::~DataLayer3D( ) {
    delete mDepthVariable;
}

float DataLayer3D::GetDataAtGeoCoord( const Types::DataCoordsPointer coord ) const {
    return GetDataAtGeoCoordForVariable( coord, mVariableVector[ 0 ] );
}

float DataLayer3D::GetDataAtGeoCoordFor( const Types::DataCoordsPointer coord, const std::string& variableName ) const {
    return GetDataAtGeoCoordForVariable( coord, GetVariable( variableName ) );
}

float DataLayer3D::GetDataAtIndices( const Types::DataIndicesPointer indices ) const {
    return GetDataAtIndicesForVariable( indices, mVariableVector[ 0 ] );
}

float DataLayer3D::GetDataAtIndicesFor( const Types::DataIndicesPointer indices, const std::string& variableName ) const {
    return GetDataAtIndicesForVariable( indices, GetVariable( variableName ) );
}

Types::VariablePointer DataLayer3D::GetDepthVariable( ) const {
    return mDepthVariable;
}

float DataLayer3D::GetDataAtGeoCoordForVariable( const Types::DataCoordsPointer coord, const Types::VariablePointer variable ) const {

    int xIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLongitudeVariable, coord->GetLongitude( ) );
    int yIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLatitudeVariable, coord->GetLatitude( ) );
    int zIndex = Processor::Get( )->FindVariableIndexOfValue( mDepthVariable, coord->GetDepth( ) );
    unsigned xMax = Parameters::Get( )->GetLengthDataLongitudeArray( );
    unsigned yMax = Parameters::Get( )->GetLengthDataLatitudeArray( );

    return variable->GetDataAtIndex( Processor::Get( )->Indices3DToIndex( xIndex, yIndex, zIndex, xMax, yMax ) );
}

float DataLayer3D::GetDataAtIndicesForVariable( const Types::DataIndicesPointer indices, const Types::VariablePointer variable ) const {

    unsigned xIndex = indices->GetDataX( );
    unsigned yIndex = indices->GetDataY( );
    unsigned zIndex = indices->GetZ( );
    unsigned xMax = Parameters::Get( )->GetLengthDataLongitudeArray( );
    unsigned yMax = Parameters::Get( )->GetLengthDataLatitudeArray( );

    return variable->GetDataAtIndex( Processor::Get( )->Indices3DToIndex( xIndex, yIndex, zIndex, xMax, yMax ) );
}