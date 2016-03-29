#include "GridDatum.h"
#include "Types.h"
#include "Processor.h"
#include "Maths.h"
#include "Parameters.h"
#include "DataCoords.h"
#include "DataIndices.h"
#include "DateTime.h"

GridDatum::GridDatum( const std::string& name, const std::string& units ) {
    mName = name;
    mUnits = units;
    mData = new float[ Parameters::Get( )->GetSizeOfGridDatum( ) ];
}

GridDatum::~GridDatum( ) {
    delete[ ] mData;
}

std::string GridDatum::GetName( ) const {
    return mName;
}

std::string GridDatum::GetUnits( ) const {
    return mUnits;
}

float* GridDatum::GetData( ) const {
    return mData;
}

void GridDatum::AddData( const Types::DataCoordsPointer coord, const float& data ) {

    int xIndex = Processor::Get( )->CalculateArrayIndexOfValue( Parameters::Get( )->GetUserLongitudeArray( ), Parameters::Get( )->GetLengthUserLongitudeArray( ), coord->GetLongitude( ) );
    int yIndex = Processor::Get( )->CalculateArrayIndexOfValue( Parameters::Get( )->GetUserLatitudeArray( ), Parameters::Get( )->GetLengthUserLatitudeArray( ), coord->GetLatitude( ) ); 
    unsigned tIndex = DateTime::Get( )->GetTimeStep( );
    unsigned xMax = Parameters::Get( )->GetLengthUserLongitudeArray( );
    unsigned yMax = Parameters::Get( )->GetLengthUserLatitudeArray( );

    unsigned index = Processor::Get( )->Indices3DToIndex( xIndex, yIndex, tIndex, xMax, yMax );

    mData[ index ] = data;
}

void GridDatum::AddData( const Types::DataIndicesPointer indies, const float& data ) {

    unsigned tIndex = DateTime::Get( )->GetTimeStep( );
    unsigned xMax = Parameters::Get( )->GetLengthUserLongitudeArray( );
    unsigned yMax = Parameters::Get( )->GetLengthUserLatitudeArray( );

    unsigned index = Processor::Get( )->Indices3DToIndex( indies->GetUserX( ), indies->GetUserY( ), tIndex, xMax, yMax );

    mData[ index ] = data;
}