#include "DataLayer2D.h"

#include "GeoCoord.h"
#include "Logger.h"
#include "Processor.h"
#include "Parameters.h"
#include "Indices.h"

DataLayer2D::DataLayer2D( const std::string& name, const Types::VariableVector& variableVector, const Types::VariablePointer longitudeVariable, const Types::VariablePointer latitudeVariable ) {
    mName = name;
    mVariableVector = variableVector;
    mLongitudeVariable = longitudeVariable;
    mLatitudeVariable = latitudeVariable;
}

DataLayer2D::~DataLayer2D( ) {
}

float DataLayer2D::GetDataAtGeoCoord( const Types::GeoCoordPointer coord ) const {
    return GetDataAtGeoCoordForVariable( coord, mVariableVector[ 0 ] );
}

float DataLayer2D::GetDataAtGeoCoordFor( const Types::GeoCoordPointer coord, const std::string& variableName ) const {
    return GetDataAtGeoCoordForVariable( coord, GetVariable( variableName ) );
}

float DataLayer2D::GetDataAtIndices( const Types::IndicesPointer indices ) const {
    return GetDataAtIndicesForVariable( indices, mVariableVector[ 0 ] );
}

float DataLayer2D::GetDataAtIndicesFor( const Types::IndicesPointer indices, const std::string& variableName ) const {
    return GetDataAtIndicesForVariable( indices, GetVariable( variableName ) );
}

float DataLayer2D::GetDataAtGeoCoordForVariable( const Types::GeoCoordPointer coord, const Types::VariablePointer variable ) const {

    int xIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLongitudeVariable, coord->GetLongitude( ) );
    int yIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLatitudeVariable, coord->GetLatitude( ) );
    unsigned int xMax = Parameters::Get( )->GetLengthLongitudeArray( );

    return variable->GetDataAtIndex( Processor::Get( )->Indices2DToIndex( xIndex, yIndex, xMax ) );
}

float DataLayer2D::GetDataAtIndicesForVariable( const Types::IndicesPointer indices, const Types::VariablePointer variable ) const {

    unsigned int xIndex = indices->GetX( );
    unsigned int yIndex = indices->GetY( );
    unsigned int xMax = Parameters::Get( )->GetLengthLongitudeArray( );

    return variable->GetDataAtIndex( Processor::Get( )->Indices2DToIndex( xIndex, yIndex, xMax ) );
}