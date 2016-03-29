#include "DataLayer2D.h"

#include "DataCoords.h"
#include "Logger.h"
#include "Processor.h"
#include "Parameters.h"
#include "DataIndices.h"

DataLayer2D::DataLayer2D( const std::string& name, const Types::VariableVector& variableVector, const Types::VariablePointer longitudeVariable, const Types::VariablePointer latitudeVariable ) {
    mName = name;
    mVariableVector = variableVector;
    mLongitudeVariable = longitudeVariable;
    mLatitudeVariable = latitudeVariable;
}

DataLayer2D::~DataLayer2D( ) {
}

float DataLayer2D::GetDataAtGeoCoord( const Types::DataCoordsPointer coord ) const {
    return GetDataAtGeoCoordForVariable( coord, mVariableVector[ 0 ] );
}

float DataLayer2D::GetDataAtGeoCoordFor( const Types::DataCoordsPointer coord, const std::string& variableName ) const {
    return GetDataAtGeoCoordForVariable( coord, GetVariable( variableName ) );
}

float DataLayer2D::GetDataAtIndices( const Types::DataIndicesPointer indices ) const {
    return GetDataAtIndicesForVariable( indices, mVariableVector[ 0 ] );
}

float DataLayer2D::GetDataAtIndicesFor( const Types::DataIndicesPointer indices, const std::string& variableName ) const {
    return GetDataAtIndicesForVariable( indices, GetVariable( variableName ) );
}

float DataLayer2D::GetDataAtGeoCoordForVariable( const Types::DataCoordsPointer coord, const Types::VariablePointer variable ) const {

    int xIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLongitudeVariable, coord->GetLongitude( ) );
    int yIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLatitudeVariable, coord->GetLatitude( ) );
    unsigned xMax = Parameters::Get( )->GetLengthDataLongitudeArray( );

    return variable->GetDataAtIndex( Processor::Get( )->Indices2DToIndex( xIndex, yIndex, xMax ) );
}

float DataLayer2D::GetDataAtIndicesForVariable( const Types::DataIndicesPointer indices, const Types::VariablePointer variable ) const {

    unsigned xIndex = indices->GetDataX( );
    unsigned yIndex = indices->GetDataY( );
    unsigned xMax = Parameters::Get( )->GetLengthDataLongitudeArray( );

    return variable->GetDataAtIndex( Processor::Get( )->Indices2DToIndex( xIndex, yIndex, xMax ) );
}