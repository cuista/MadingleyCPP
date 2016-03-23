#ifndef DATALAYER3D
#define	DATALAYER3D

#include "DataLayer.h"

class DataLayer3D: public DataLayer {
public:
    DataLayer3D( const std::string&, const Types::VariableVector, const Types::VariablePointer, const Types::VariablePointer, const Types::VariablePointer );
    ~DataLayer3D( );

    float GetDataAtGeoCoord( Types::DataCoordsPointer ) const;
    float GetDataAtGeoCoordFor( const Types::DataCoordsPointer, const std::string& ) const;
    
    float GetDataAtIndices( const Types::DataIndicesPointer ) const;
    float GetDataAtIndicesFor( const Types::DataIndicesPointer, const std::string& ) const;
    
    Types::VariablePointer GetDepthVariable( ) const;

private:
    float GetDataAtGeoCoordForVariable( const Types::DataCoordsPointer, const Types::VariablePointer ) const;
    float GetDataAtIndicesForVariable( const Types::DataIndicesPointer, const Types::VariablePointer ) const;
    
    Types::VariablePointer mDepthVariable;
};

#endif

