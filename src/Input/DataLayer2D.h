#ifndef DATALAYER2D
#define	DATALAYER2D

#include "DataLayer.h"

class DataLayer2D: public DataLayer {
public:
    DataLayer2D( const std::string&, const Types::VariableVector&, const Types::VariablePointer, const Types::VariablePointer );
    ~DataLayer2D( );

    float GetDataAtGeoCoord( const Types::GeoCoordPointer ) const;
    float GetDataAtGeoCoordFor( const Types::GeoCoordPointer, const std::string& ) const;

    float GetDataAtIndices( const Types::IndicesPointer ) const;
    float GetDataAtIndicesFor( const Types::IndicesPointer, const std::string& ) const;

private:
    float GetDataAtGeoCoordForVariable( const Types::GeoCoordPointer, const Types::VariablePointer ) const;
    float GetDataAtIndicesForVariable( const Types::IndicesPointer, const Types::VariablePointer ) const;
};

#endif

