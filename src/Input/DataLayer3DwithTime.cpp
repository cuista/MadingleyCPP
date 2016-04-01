#include "DataLayer3DwithTime.h"

#include "DataCoords.h"
#include "Logger.h"
#include "Processor.h"
#include "Parameters.h"
#include "DataIndices.h"
#include "DateTime.h"
#include "GridCell.h"

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

    int longitudeIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLongitudeVariable, coord->GetLongitude( ) );
    int latitudeIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLatitudeVariable, coord->GetLatitude( ) );
    int depthIndex = Processor::Get( )->FindVariableIndexOfValue( mDepthVariable, coord->GetDepth( ) );

    return GetDataAtIndicesForVariable( longitudeIndex, latitudeIndex, depthIndex, mVariableVector[ 0 ] );
}

float DataLayer3DwithTime::GetDataAtGeoCoordFor( const Types::DataCoordsPointer coord, const std::string& variableName ) const {

    int longitudeIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLongitudeVariable, coord->GetLongitude( ) );
    int latitudeIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLatitudeVariable, coord->GetLatitude( ) );
    int depthIndex = Processor::Get( )->FindVariableIndexOfValue( mDepthVariable, coord->GetDepth( ) );

    return GetDataAtIndicesForVariable( longitudeIndex, latitudeIndex, depthIndex, GetVariable( variableName ) );
}

float DataLayer3DwithTime::GetDataAtIndices( const Types::DataIndicesPointer indices ) const {
    return GetDataAtIndicesForVariable( indices->GetDataX( ), indices->GetDataY( ), indices->GetZ( ), mVariableVector[ 0 ] );
}

float DataLayer3DwithTime::GetDataAtIndicesFor( const Types::DataIndicesPointer indices, const std::string& variableName ) const {
    return GetDataAtIndicesForVariable( indices->GetDataX( ), indices->GetDataY( ), indices->GetZ( ), GetVariable( variableName ) );
}

float DataLayer3DwithTime::GetDataAtGridCell( const Types::GridCellPointer gridCell ) const {
    return 1.0;
}

float DataLayer3DwithTime::GetDataAtGridCellFor( const Types::GridCellPointer gridCell, const std::string& variableName ) const {
    return 1.0;
}

Types::VariablePointer DataLayer3DwithTime::GetDepthVariable( ) const {
    return mDepthVariable;
}

Types::VariablePointer DataLayer3DwithTime::GetTimeVariable( ) const {
    return mTimeVariable;
}

float DataLayer3DwithTime::GetDataAtIndicesForVariable( const unsigned longitudeIndex, const unsigned latitudeIndex, const unsigned depthIndex, const Types::VariablePointer variable ) const {
    return variable->GetDataAtIndex( Processor::Get( )->Indices3DToIndex( longitudeIndex, latitudeIndex, depthIndex, Parameters::Get( )->GetLengthDataLongitudeArray( ), Parameters::Get( )->GetLengthDataLatitudeArray( ) ) );
}