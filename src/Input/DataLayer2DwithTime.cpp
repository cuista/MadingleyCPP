#include "DataLayer2DwithTime.h"

#include "GeoCoord.h"
#include "Logger.h"
#include "Processor.h"
#include "Parameters.h"
#include "Indices.h"
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

float DataLayer2DwithTime::GetDataAtGeoCoord( const Types::GeoCoordPointer coord ) const {
    return GetDataAtGeoCoordForVariable( coord, mVariableVector[ 0 ] );
}

float DataLayer2DwithTime::GetDataAtGeoCoordFor( const Types::GeoCoordPointer coord, const std::string& variableName ) const {
    return GetDataAtGeoCoordForVariable( coord, GetVariable( variableName ) );
}

float DataLayer2DwithTime::GetDataAtIndices( const Types::IndicesPointer indices ) const {
    return GetDataAtIndicesForVariable( indices, mVariableVector[ 0 ] );
}

float DataLayer2DwithTime::GetDataAtIndicesFor( const Types::IndicesPointer indices, const std::string& variableName ) const {
    return GetDataAtIndicesForVariable( indices, GetVariable( variableName ) );
}

Types::VariablePointer DataLayer2DwithTime::GetTimeVariable( ) const {
    return mTimeVariable;
}

float DataLayer2DwithTime::GetDataAtGeoCoordForVariable( const Types::GeoCoordPointer coord, const Types::VariablePointer variable ) const {

    int xIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLongitudeVariable, coord->GetLongitude( ) );
    int yIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLatitudeVariable, coord->GetLatitude( ) );
    unsigned int tIndex = DateTime::Get( )->GetTimeStep( );
    unsigned int xMax = Parameters::Get( )->GetLengthLongitudeArray( );
    unsigned int yMax = Parameters::Get( )->GetLengthLatitudeArray( );

    delete coord;

    return variable->GetDataAtIndex( Processor::Get( )->Indices3DToIndex( xIndex, yIndex, tIndex, xMax, yMax ) );
}

float DataLayer2DwithTime::GetDataAtIndicesForVariable( const Types::IndicesPointer indices, const Types::VariablePointer variable ) const {

    unsigned int xIndex = indices->GetX( );
    unsigned int yIndex = indices->GetY( );
    unsigned int tIndex = DateTime::Get( )->GetTimeStep( );
    unsigned int xMax = Parameters::Get( )->GetLengthLongitudeArray( );
    unsigned int yMax = Parameters::Get( )->GetLengthLatitudeArray( );
    
    delete indices;

    return variable->GetDataAtIndex( Processor::Get( )->Indices3DToIndex( xIndex, yIndex, tIndex, xMax, yMax ) );
}