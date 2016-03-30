#ifndef DATALAYER2D
#define	DATALAYER2D

#include "DataLayer.h"

class DataLayer2D: public DataLayer {
public:
    DataLayer2D( const std::string&, const Types::VariableVector&, const Types::VariablePointer, const Types::VariablePointer );
    ~DataLayer2D( );

    float GetDataAtGeoCoord( const Types::DataCoordsPointer ) const;
    float GetDataAtGeoCoordFor( const Types::DataCoordsPointer, const std::string& ) const;

    float GetDataAtIndices( const Types::DataIndicesPointer ) const;
    float GetDataAtIndicesFor( const Types::DataIndicesPointer, const std::string& ) const;

private:
    float GetDataAtGeoCoordForVariable( const Types::DataCoordsPointer, const Types::VariablePointer ) const;
    float GetDataAtIndicesForVariable( const Types::DataIndicesPointer, const Types::VariablePointer ) const;
};

#endif
