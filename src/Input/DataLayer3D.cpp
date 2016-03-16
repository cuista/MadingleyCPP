#include "DataLayer3D.h"

#include "GeoCoord.h"
#include "Logger.h"
#include "Processor.h"
#include "Parameters.h"
#include "Indices.h"

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

float DataLayer3D::GetDataAtGeoCoord( const Types::GeoCoordPointer coord ) const {
    return GetDataAtGeoCoordForVariable( coord, mVariableVector[ 0 ] );
}

float DataLayer3D::GetDataAtGeoCoordFor( const Types::GeoCoordPointer coord, const std::string& variableName ) const {
    return GetDataAtGeoCoordForVariable( coord, GetVariable( variableName ) );
}

float DataLayer3D::GetDataAtIndices( const Types::IndicesPointer indices ) const {
    return GetDataAtIndicesForVariable( indices, mVariableVector[ 0 ] );
}

float DataLayer3D::GetDataAtIndicesFor( const Types::IndicesPointer indices, const std::string& variableName ) const {
    return GetDataAtIndicesForVariable( indices, GetVariable( variableName ) );
}

Types::VariablePointer DataLayer3D::GetDepthVariable( ) const {
    return mDepthVariable;
}

float DataLayer3D::GetDataAtGeoCoordForVariable( const Types::GeoCoordPointer coord, const Types::VariablePointer variable ) const {

    int xIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLongitudeVariable, coord->GetLongitude( ) );
    int yIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLatitudeVariable, coord->GetLatitude( ) );
    int zIndex = Processor::Get( )->FindVariableIndexOfValue( mDepthVariable, coord->GetDepth( ) );
    unsigned int xMax = Parameters::Get( )->GetLengthLongitudeArray( );
    unsigned int yMax = Parameters::Get( )->GetLengthLatitudeArray( );

    delete coord;

    return variable->GetDataAtIndex( Processor::Get( )->Indices3DToIndex( xIndex, yIndex, zIndex, xMax, yMax ) );
}

float DataLayer3D::GetDataAtIndicesForVariable( const Types::IndicesPointer indices, const Types::VariablePointer variable ) const {

    unsigned int xIndex = indices->GetX( );
    unsigned int yIndex = indices->GetY( );
    unsigned int zIndex = indices->GetZ( );
    unsigned int xMax = Parameters::Get( )->GetLengthLongitudeArray( );
    unsigned int yMax = Parameters::Get( )->GetLengthLatitudeArray( );
    
    delete indices;

    return variable->GetDataAtIndex( Processor::Get( )->Indices3DToIndex( xIndex, yIndex, zIndex, xMax, yMax ) );
}