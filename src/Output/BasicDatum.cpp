#include "BasicDatum.h"
#include "Constants.h"
#include "Parameters.h"
#include "DateTime.h"

BasicDatum::BasicDatum( const std::string& name, const std::string& units ) {
    mName = name;
    mUnits = units;
    mData = new float[ Parameters::Get( )->GetLengthOfSimulationInTimeSteps( ) ];
}

BasicDatum::~BasicDatum( ) {
    delete[ ] mData;
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