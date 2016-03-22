#include "GridDatum.h"
#include "Types.h"
#include "Processor.h"
#include "Maths.h"
#include "Parameters.h"
#include "GeoCoord.h"
#include "Indices.h"
#include "Logger.h"
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

void GridDatum::AddData( const Types::GeoCoordPointer coord, const float& data ) {

    int xIndex = Processor::Get( )->CalculateArrayIndexOfValue( Parameters::Get( )->GetUserLongitudeArray( ), Parameters::Get( )->GetLengthUserLongitudeArray( ), coord->GetLongitude( ) );
    int yIndex = Processor::Get( )->CalculateArrayIndexOfValue( Parameters::Get( )->GetUserLatitudeArray( ), Parameters::Get( )->GetLengthUserLatitudeArray( ), coord->GetLatitude( ) );
    unsigned int tIndex = DateTime::Get( )->GetTimeStep( );
    unsigned int xMax = Parameters::Get( )->GetLengthUserLongitudeArray( );
    unsigned int yMax = Parameters::Get( )->GetLengthUserLatitudeArray( );

    unsigned int index = Processor::Get( )->Indices3DToIndex( xIndex, yIndex, tIndex, xMax, yMax );
    
    mData[ index ] = data;
}

void GridDatum::AddData( const Types::IndicesPointer indies, const float& data ) {

    unsigned int tIndex = DateTime::Get( )->GetTimeStep( );
    unsigned int xMax = Parameters::Get( )->GetLengthLongitudeArray( );
    unsigned int yMax = Parameters::Get( )->GetLengthLatitudeArray( );

    unsigned int index = Processor::Get( )->Indices3DToIndex( indies->GetX( ), indies->GetY( ), tIndex, xMax, yMax );

    mData[ index ] = data;
}