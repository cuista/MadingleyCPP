#include "DataLayer2DwithTime.h"

#include "DataCoords.h"
#include "Logger.h"
#include "Processor.h"
#include "Parameters.h"
#include "DataIndices.h"
#include "DateTime.h"

DataLayer2DwithTime::DataLayer2DwithTime( const std::string& name, const Types::VariableVector variableVector, Types::VariablePointer longitudeVariable, Types::VariablePointer latitudeVariable, Types::VariablePointer timeVariable ) {
    mName = name;
    mVariableVector = variableVector;
    mLongitudeVariable = longitudeVariable;
    mLatitudeVariable = latitudeVariable;
    mTimeVariable = timeVariable;
}

DataLayer2DwithTime::~DataLayer2DwithTime( ) {
    delete mTimeVariable;
}

float DataLayer2DwithTime::GetDataAtGeoCoord( const Types::DataCoordsPointer coord ) const {

    int longitudeIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLongitudeVariable, coord->GetLongitude( ) );
    int latitudeIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLatitudeVariable, coord->GetLatitude( ) );

    return GetDataAtIndicesForVariable( longitudeIndex, latitudeIndex, mVariableVector[ 0 ] );
}

float DataLayer2DwithTime::GetDataAtGeoCoordFor( const Types::DataCoordsPointer coord, const std::string& variableName ) const {

    int longitudeIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLongitudeVariable, coord->GetLongitude( ) );
    int latitudeIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLatitudeVariable, coord->GetLatitude( ) );

    return GetDataAtIndicesForVariable( longitudeIndex, latitudeIndex, GetVariable( variableName ) );
}

float DataLayer2DwithTime::GetDataAtIndices( const Types::DataIndicesPointer indices ) const {
    return GetDataAtIndicesForVariable( indices->GetDataX( ), indices->GetDataY( ), mVariableVector[ 0 ] );
}

float DataLayer2DwithTime::GetDataAtIndicesFor( const Types::DataIndicesPointer indices, const std::string& variableName ) const {
    return GetDataAtIndicesForVariable( indices->GetDataX( ), indices->GetDataY( ), GetVariable( variableName ) );
}

float DataLayer2DwithTime::GetDataAtGridCell( const Types::GridCellPointer gridCell ) const {
    return 1.0;
}

float DataLayer2DwithTime::GetDataAtGridCellFor( const Types::GridCellPointer gridCell, const std::string& variableName ) const {
    return 1.0;
}

Types::VariablePointer DataLayer2DwithTime::GetTimeVariable( ) const {
    return mTimeVariable;
}

float DataLayer2DwithTime::GetDataAtIndicesForVariable( const unsigned longitudeIndex, const unsigned latitudeIndex, const Types::VariablePointer variable ) const {
    return variable->GetDataAtIndex( Processor::Get( )->Indices3DToIndex( longitudeIndex, latitudeIndex, DateTime::Get( )->GetTimeStep( ), Parameters::Get( )->GetLengthDataLongitudeArray( ), Parameters::Get( )->GetLengthDataLatitudeArray( ) ) );
}