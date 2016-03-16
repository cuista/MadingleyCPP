#ifndef DATALAYER
#define	DATALAYER

#include "Types.h"
#include "Variable.h"
#include "Logger.h"

class DataLayer {
public:

    virtual ~DataLayer( ) {
        for( unsigned int variableIndex = 0; variableIndex < mVariableVector.size( ); ++variableIndex ) {
            delete mVariableVector[ variableIndex ];
        }
        delete mLongitudeVariable;
        delete mLatitudeVariable;
    };

    virtual float GetDataAtGeoCoord( const Types::GeoCoordPointer ) const = 0;
    virtual float GetDataAtGeoCoordFor( const Types::GeoCoordPointer, const std::string& ) const = 0;

    virtual float GetDataAtIndices( const Types::IndicesPointer ) const = 0;
    virtual float GetDataAtIndicesFor( const Types::IndicesPointer, const std::string& ) const = 0;

    std::string GetName( );

    Types::VariablePointer GetDefaultVariable( ) const;
    Types::VariablePointer GetVariable( const std::string& ) const;

    Types::VariablePointer GetLongitudeVariable( ) const;
    Types::VariablePointer GetLatitudeVariable( ) const;

protected:
    virtual float GetDataAtGeoCoordForVariable( const Types::GeoCoordPointer, const Types::VariablePointer ) const = 0;
    virtual float GetDataAtIndicesForVariable( const Types::IndicesPointer, const Types::VariablePointer ) const = 0;

    Types::VariableVector mVariableVector;
    Types::VariablePointer mLongitudeVariable;
    Types::VariablePointer mLatitudeVariable;

    std::string mName;
};

#endif

