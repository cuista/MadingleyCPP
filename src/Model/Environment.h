/* 
 * File:   Environment.h
 * Author: mb425
 *
 * Created on 26 November 2015, 13:32
 */


#ifndef ENVIRONMENT_H
#define	ENVIRONMENT_H

#include "Types.h"
////////////////////////////////////////////////////////////////////////////////

class Layer {
public:

    virtual ~Layer( ) {
        ;
    }
    virtual Types::DoubleVector& operator[]( int i ) = 0;
    virtual void setTime( int tm ) = 0;
};
////////////////////////////////////////////////////////////////////////////////

class layer0: public Layer {
    Types::DoubleMatrix data;
public:

    layer0( int sx, int sy ) {
        data.resize( sx );
        for( unsigned m = 0; m < sx; m++ )data[m].resize( sy );
    }
    //--------------------------------------------------------------------------

    ~layer0( ) {
        for( unsigned m = 0; m < data.size( ); m++ )data[m].clear( );
    }
    //--------------------------------------------------------------------------

    Types::DoubleVector& operator[]( int i ) {
        return data[i];
    }
    //--------------------------------------------------------------------------

    void setTime( int tm ) {
        ;
    }
};
////////////////////////////////////////////////////////////////////////////////

class layerT: public Layer {
    int t;
    Types::Double3DMatrix data;
public:

    layerT( int q, int sx, int sy ): t( 0 ) {
        data.resize( q );
        for( unsigned u = 0; u < q; u++ ) {
            data[u].resize( sx );
            for( unsigned i = 0; i < sx; i++ ) {
                data[u][i].resize( sy );
            }
        }
    }
    //--------------------------------------------------------------------------

    ~layerT( ) {
        for( unsigned m = 0; m < data.size( ); m++ ) {
            for( unsigned n = 0; n < data[m].size( ); n++ )data[m][n].clear( );
            data[m].clear( );
        }
    }
    //--------------------------------------------------------------------------

    Types::DoubleVector& operator[]( int i ) {
        return data[t][i];
    }
    //--------------------------------------------------------------------------

    void setTime( int tm ) {
        t = tm;
    }
};
////////////////////////////////////////////////////////////////////////////////

class Environment {
    static Types::EnvironmentPointer mThis;
    static Types::LayerMap mLayers;
    Environment( );

    void addLayer( std::string );
    void addLayerT( std::string );
    void setUVel( );
    void setVVel( );
    void setTemperature( );
    void setDiurnalTemperatureRange( );
    void setPrecipitation( );
    void setNPP( );
    void setRealm( );
    void setOrganicPool( );
    void setRespiratoryCO2Pool( );
    void setAVGSDTemp( );
    void setNPPSeasonality( );
    void setBreeding( );
    void setFrostandFire( );
    void setHANPP( );
public:
    static Environment* Get( );
    static double Get( std::string s, GridCell& gcl, int tm );
    static double& Get( std::string s, GridCell& gcl );
    static double& Get( std::string s, int, int );

    static void update( int );
};

#endif	/* ENVIRONMENT_H */


