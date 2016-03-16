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
    Initialise( );
}

GridDatum::~GridDatum( ) {
    delete[ ] mData;
}

void GridDatum::Initialise( ) {
    mData = new float[ Parameters::Get( )->GetSizeOfGridDatum( ) ];
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

    int xIndex = Processor::Get( )->CalculateArrayIndexOfValue( Parameters::Get( )->GetLongitudeArray( ), Parameters::Get( )->GetLengthLongitudeArray( ), coord->GetLongitude( ) );
    int yIndex = Processor::Get( )->CalculateArrayIndexOfValue( Parameters::Get( )->GetLatitudeArray( ), Parameters::Get( )->GetLengthLatitudeArray( ), coord->GetLatitude( ) );
    unsigned int tIndex = DateTime::Get( )->GetTimeStep( );
    unsigned int xMax = Parameters::Get( )->GetLengthLongitudeArray( );
    unsigned int yMax = Parameters::Get( )->GetLengthLatitudeArray( );

    delete coord;

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