#ifndef DATALAYER
#define	DATALAYER

#include "Types.h"
#include "Variable.h"
#include "Logger.h"

class DataLayer {
public:

    virtual ~DataLayer( ) {
        for( unsigned variableIndex = 0; variableIndex < mVariableVector.size( ); ++variableIndex ) {
            delete mVariableVector[ variableIndex ];
        }
        delete mLongitudeVariable;
        delete mLatitudeVariable;
    };

    virtual float GetDataAtGeoCoord( const Types::DataCoordsPointer ) const = 0;
    virtual float GetDataAtGeoCoordFor( const Types::DataCoordsPointer, const std::string& ) const = 0;

    virtual float GetDataAtIndices( const Types::DataIndicesPointer ) const = 0;
    virtual float GetDataAtIndicesFor( const Types::DataIndicesPointer, const std::string& ) const = 0;
    
    virtual float GetDataAtGridCell( const Types::GridCellPointer ) const = 0;
    virtual float GetDataAtGridCellFor( const Types::GridCellPointer, const std::string& ) const = 0;

    std::string GetName( );

    Types::VariablePointer GetDefaultVariable( ) const;
    Types::VariablePointer GetVariable( const std::string& ) const;

    Types::VariablePointer GetLongitudeVariable( ) const;
    Types::VariablePointer GetLatitudeVariable( ) const;

protected:
    Types::VariableVector mVariableVector;
    Types::VariablePointer mLongitudeVariable;
    Types::VariablePointer mLatitudeVariable;

    std::string mName;
};

#endif
