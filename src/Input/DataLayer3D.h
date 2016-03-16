#ifndef DATALAYER3D
#define	DATALAYER3D

#include "DataLayer.h"

class DataLayer3D: public DataLayer {
public:
    DataLayer3D( const std::string&, const Types::VariableVector, const Types::VariablePointer, const Types::VariablePointer, const Types::VariablePointer );
    ~DataLayer3D( );

    float GetDataAtGeoCoord( Types::GeoCoordPointer ) const;
    float GetDataAtGeoCoordFor( const Types::GeoCoordPointer, const std::string& ) const;
    
    float GetDataAtIndices( const Types::IndicesPointer ) const;
    float GetDataAtIndicesFor( const Types::IndicesPointer, const std::string& ) const;
    
    Types::VariablePointer GetDepthVariable( ) const;

private:
    float GetDataAtGeoCoordForVariable( const Types::GeoCoordPointer, const Types::VariablePointer ) const;
    float GetDataAtIndicesForVariable( const Types::IndicesPointer, const Types::VariablePointer ) const;
    
    Types::VariablePointer mDepthVariable;
};

#endif

