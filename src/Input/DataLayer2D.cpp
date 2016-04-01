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
    
    int longitudeIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLongitudeVariable, coord->GetLongitude( ) );
    int latitudeIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLatitudeVariable, coord->GetLatitude( ) );
    
    return GetDataAtIndicesForVariable( longitudeIndex, latitudeIndex, mVariableVector[ 0 ] );
}

float DataLayer2D::GetDataAtGeoCoordFor( const Types::DataCoordsPointer coord, const std::string& variableName ) const {
    
    int longitudeIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLongitudeVariable, coord->GetLongitude( ) );
    int latitudeIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLatitudeVariable, coord->GetLatitude( ) );
    
    return GetDataAtIndicesForVariable( longitudeIndex, latitudeIndex, GetVariable( variableName ) );
}

float DataLayer2D::GetDataAtIndices( const Types::DataIndicesPointer indices ) const {
    return GetDataAtIndicesForVariable( indices->GetDataX( ), indices->GetDataY( ), mVariableVector[ 0 ] );
}

float DataLayer2D::GetDataAtIndicesFor( const Types::DataIndicesPointer indices, const std::string& variableName ) const {
    return GetDataAtIndicesForVariable( indices->GetDataX( ), indices->GetDataY( ), GetVariable( variableName ) );
}

float DataLayer2D::GetDataAtGridCell( const Types::GridCellPointer gridCell ) const {  
    return 1.0;
}

float DataLayer2D::GetDataAtGridCellFor( const Types::GridCellPointer gridCell, const std::string& variableName ) const {
    return 1.0;
}

float DataLayer2D::GetDataAtIndicesForVariable( const unsigned longitudeIndex, const unsigned latitudeIndex, const Types::VariablePointer variable ) const {
    return variable->GetDataAtIndex( Processor::Get( )->Indices2DToIndex( longitudeIndex, latitudeIndex, Parameters::Get( )->GetLengthDataLongitudeArray( ) ) );
}