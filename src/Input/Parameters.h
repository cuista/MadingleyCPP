#ifndef PARAMETERS
#define	PARAMETERS

#include "Types.h"

class Parameters {
public:
    ~Parameters( );
    static Types::ParametersPointer Get( );

    bool Initialise( const Types::StringMatrix& );

    // User defined parameters
    unsigned int GetLengthOfSimulationInYears( ) const;
    int GetUserMinimumLatitude( ) const;
    int GetUserMaximumLatitude( ) const;
    int GetUserMinimumLongitude( ) const;
    int GetUserMaximumLongitude( ) const;
    float GetGridCellSize( ) const;

    void SetLengthOfSimulationInYears( const unsigned int& );
    void SetUserMinimumLongitude( const int& );
    void SetUserMaximumLongitude( const int& );
    void SetUserMinimumLatitude( const int& );
    void SetUserMaximumLatitude( const int& );
    void SetGridCellSize( const float& );

    // Calculated parameters
    unsigned int GetLengthOfSimulationInTimeSteps( ) const;
    unsigned int GetLengthLongitudeArray( ) const;
    unsigned int GetLengthLatitudeArray( ) const;
    unsigned int GetIndexOfUserMinimumLongitude( ) const;
    unsigned int GetIndexOfUserMaximumLongitude( ) const;
    unsigned int GetIndexOfUserMinimumLatitude( ) const;
    unsigned int GetIndexOfUserMaximumLatitude( ) const;
    unsigned int GetLengthUserLongitudeArray( ) const;
    unsigned int GetLengthUserLatitudeArray( ) const;
    unsigned int GetSizeOfGridDatum( ) const;
    float GetLongitudeAtIndex( const unsigned int& ) const;
    float GetLatitudeAtIndex( const unsigned int& ) const;
    float* GetLongitudeArray( ) const;
    float* GetLatitudeArray( ) const;
    float* GetTimeStepArray( ) const;
    float* GetUserLongitudeArray( ) const;
    float* GetUserLatitudeArray( ) const;
    
    Types::GeoCoords GetCoordsFromDomainIndices( const Types::GeoIndices ) const;
    Types::GeoCoords GetCoordsFromDomainIndices( const unsigned short&, const unsigned short& ) const;
    Types::GeoIndices GetDomainIndicesFromCoords( const Types::GeoCoords ) const;
    Types::GeoIndices GetDomainIndicesFromCoords( const float&, const float& ) const;

private:
    Parameters( );
    void CalculateParameters( );
    
    static Types::ParametersPointer mThis;

    // User defined parameters
    unsigned int mLengthOfSimulationInYears;
    int mUserMinimumLongitude;
    int mUserMaximumLongitude;
    int mUserMinimumLatitude;
    int mUserMaximumLatitude;
    float mGridCellSize;

    // Calculated parameters
    unsigned int mLengthOfSimulationInTimeSteps;
    unsigned int mLengthLongitudeArray;
    unsigned int mLengthLatitudeArray;
    unsigned int mLengthUserLongitudeArray;
    unsigned int mLengthUserLatitudeArray;
    unsigned int mIndexOfUserMinimumLongitude;
    unsigned int mIndexOfUserMaximumLongitude;
    unsigned int mIndexOfUserMinimumLatitude;
    unsigned int mIndexOfUserMaximumLatitude;
    unsigned int mSizeOfGridDatum;
    float* mLongitudeArray;
    float* mLatitudeArray;
    float* mTimeStepArray;
    float* mUserLongitudeArray;
    float* mUserLatitudeArray;
    
    Types::GeoCoordsMatrix mDomainCoordsMatrix;
};

#endif

