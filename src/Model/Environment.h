#ifndef ENVIRONMENT
#define	ENVIRONMENT

#include "Types.h"

class Environment {
public:
    static Environment* Get( );
    //static double Get( std::string s, GridCell& gcl, int tm );
    static double& Get( std::string s, GridCell& gcl );
    static double& Get( std::string s, int );
    static void update( int );
    
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
    
    static Types::EnvironmentPointer mThis;
    static Types::LayerMap mLayers;
};

#endif

