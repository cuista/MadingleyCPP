#ifndef DATALAYER2DWITHTIME
#define	DATALAYER2DWITHTIME

#include "DataLayer.h"

class DataLayer2DwithTime: public DataLayer {
public:
    DataLayer2DwithTime( const std::string&, const Types::VariableVector, const Types::VariablePointer, const Types::VariablePointer, const Types::VariablePointer );
    ~DataLayer2DwithTime( );

    float GetDataAtGeoCoord( const Types::GeoCoordPointer ) const;
    float GetDataAtGeoCoordFor( const Types::GeoCoordPointer, const std::string& ) const;
    
    float GetDataAtIndices( const Types::IndicesPointer ) const;
    float GetDataAtIndicesFor( const Types::IndicesPointer, const std::string& ) const;

    Types::VariablePointer GetTimeVariable( ) const;

private:
    float GetDataAtGeoCoordForVariable( const Types::GeoCoordPointer, const Types::VariablePointer ) const;
    float GetDataAtIndicesForVariable( const Types::IndicesPointer, const Types::VariablePointer ) const;
    
    Types::VariablePointer mTimeVariable;
};

#endif

