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

    int longitudeIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLongitudeVariable, coord->GetLongitude( ) );
    int latitudeIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLatitudeVariable, coord->GetLatitude( ) );
    int depthIndex = Processor::Get( )->FindVariableIndexOfValue( mDepthVariable, coord->GetDepth( ) );

    return GetDataAtIndicesForVariable( longitudeIndex, latitudeIndex, depthIndex, mVariableVector[ 0 ] );
}

float DataLayer3D::GetDataAtGeoCoordFor( const Types::DataCoordsPointer coord, const std::string& variableName ) const {

    int longitudeIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLongitudeVariable, coord->GetLongitude( ) );
    int latitudeIndex = Processor::Get( )->CalculateVariableIndexOfValue( mLatitudeVariable, coord->GetLatitude( ) );
    int depthIndex = Processor::Get( )->FindVariableIndexOfValue( mDepthVariable, coord->GetDepth( ) );

    return GetDataAtIndicesForVariable( longitudeIndex, latitudeIndex, depthIndex, GetVariable( variableName ) );
}

float DataLayer3D::GetDataAtIndices( const Types::DataIndicesPointer indices ) const {
    return GetDataAtIndicesForVariable( indices->GetDataX( ), indices->GetDataY( ), indices->GetZ( ), mVariableVector[ 0 ] );
}

float DataLayer3D::GetDataAtIndicesFor( const Types::DataIndicesPointer indices, const std::string& variableName ) const {
    return GetDataAtIndicesForVariable( indices->GetDataX( ), indices->GetDataY( ), indices->GetZ( ), GetVariable( variableName ) );
}

float DataLayer3D::GetDataAtGridCell( const Types::GridCellPointer gridCell ) const {
    return 1.0;
}

float DataLayer3D::GetDataAtGridCellFor( const Types::GridCellPointer gridCell, const std::string& variableName ) const {
    return 1.0;
}

Types::VariablePointer DataLayer3D::GetDepthVariable( ) const {
    return mDepthVariable;
}

float DataLayer3D::GetDataAtIndicesForVariable( const unsigned longitudeIndex, const unsigned latitudeIndex, const unsigned depthIndex, const Types::VariablePointer variable ) const {
    return variable->GetDataAtIndex( Processor::Get( )->Indices3DToIndex( longitudeIndex, latitudeIndex, depthIndex, Parameters::Get( )->GetLengthDataLongitudeArray( ), Parameters::Get( )->GetLengthDataLatitudeArray( ) ) );
}