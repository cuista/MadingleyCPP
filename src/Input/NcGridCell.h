#ifndef NCGRIDCELL_H
#define	NCGRIDCELL_H

#include "Types.h"

class NcGridCell {
public:
    
    NcGridCell( const int );
    ~NcGridCell( );
    
    void InitialiseVectors( const unsigned int );
    
    int GetRealm( ) const;
    double GetWaterCapacity( ) const;
    double GetSurfaceAirTemperature( const unsigned int ) const;
    double GetDiurnalTemperatureRange( const unsigned int ) const;
    double GetGroundFrostFrequency( const unsigned int ) const;
    double GetPrecipitation( const unsigned int ) const;
    double GetNearSurfaceTemperature( const unsigned int ) const;
    double GetNPP( const unsigned int ) const;
    double GetOceanNPP( const unsigned int ) const;
    double GetSST( const unsigned int ) const;
    double GetUSpeed( const unsigned int ) const;
    double GetVSpeed( const unsigned int ) const;
    
    void SetRealm( const int );
    void SetWaterCapacity( const double );
    void SetDiurnalTemperatureRange( const double, const unsigned int );
    void SetGroundFrostFrequency( const double, const unsigned int );
    void SetPrecipitation( const double, const unsigned int );
    void SetNearSurfaceTemperature( const double, const unsigned int );
    void SetNPP( const double, const unsigned int );
    void SetOceanNPP( const double, const unsigned int );
    void SetSST( const double, const unsigned int );
    void SetUSpeed( const double, const unsigned int );
    void SetVSpeed( const double, const unsigned int );

private:
    int mRealm;
    double mWaterCapacity;
    Types::DoubleVector mDiurnalTemperatureRange;
    Types::DoubleVector mGroundFrostFrequency;
    Types::DoubleVector mPrecipitation;
    Types::DoubleVector mSurfaceAirTemperature;
    Types::DoubleVector mNearSurfaceTemperature;;
    Types::DoubleVector mNPP;
    Types::DoubleVector mOceanNPP;
    Types::DoubleVector mSST;
    Types::DoubleVector mUSpeed;
    Types::DoubleVector mVSpeed;
};

#endif

