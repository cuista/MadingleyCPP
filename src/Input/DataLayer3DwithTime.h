#ifndef DATALAYER3DWITHTIME
#define	DATALAYER3DWITHTIME

#include "DataLayer.h"

class DataLayer3DwithTime: public DataLayer {
public:
    DataLayer3DwithTime( const std::string&, const Types::VariableVector, const Types::VariablePointer, const Types::VariablePointer, const Types::VariablePointer, const Types::VariablePointer );
    ~DataLayer3DwithTime( );

    float GetDataAtGeoCoord( const Types::DataCoordsPointer ) const;
    float GetDataAtGeoCoordFor( const Types::DataCoordsPointer, const std::string& ) const;

    float GetDataAtIndices( const Types::DataIndicesPointer ) const;
    float GetDataAtIndicesFor( const Types::DataIndicesPointer, const std::string& ) const;

    float GetDataAtGridCell( const Types::GridCellPointer ) const;
    float GetDataAtGridCellFor( const Types::GridCellPointer, const std::string& ) const;

    Types::VariablePointer GetDepthVariable( ) const;
    Types::VariablePointer GetTimeVariable( ) const;

private:
    float GetDataAtIndicesForVariable( const unsigned, const unsigned, const unsigned, const Types::VariablePointer ) const;

    Types::VariablePointer mDepthVariable;
    Types::VariablePointer mTimeVariable;
};

#endif

