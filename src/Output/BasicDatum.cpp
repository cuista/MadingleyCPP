#include "BasicDatum.h"
#include "Constants.h"
#include "Parameters.h"
#include "DateTime.h"

BasicDatum::BasicDatum( const std::string& name, const std::string& units ) {
    mName = name;
    mUnits = units;
    Initialise( );
}

BasicDatum::~BasicDatum( ) {
    delete[ ] mData;
}

void BasicDatum::Initialise( ) {
    mData = new float[ Parameters::Get( )->GetLengthOfSimulationInTimeSteps( ) ];
}

std::string BasicDatum::GetName( ) const {
    return mName;
}

std::string BasicDatum::GetUnits( ) const {
    return mUnits;
}

float* BasicDatum::GetData( ) const {
    return mData;
}

void BasicDatum::AddData( const float& data ) {
    mData[ DateTime::Get( )->GetTimeStep( ) ] = data;
}