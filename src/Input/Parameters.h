#ifndef PARAMETERS
#define	PARAMETERS

#include "Types.h"

class Parameters {
public:
    ~Parameters( );
    static Types::ParametersPointer Get( );

    bool Initialise( const Types::StringMatrix& );

    // User defined parameters
    std::string GetTimeStepUnits( ) const;
    unsigned GetLengthOfSimulationInYears( ) const;
    int GetUserMinimumLatitude( ) const;
    int GetUserMaximumLatitude( ) const;
    int GetUserMinimumLongitude( ) const;
    int GetUserMaximumLongitude( ) const;
    unsigned GetGridCellSize( ) const;
    float GetExtinctionThreshold( ) const;
    unsigned GetMaximumNumberOfCohorts( ) const;
    float GetPlanktonSizeThreshold( ) const;
    bool GetDrawRandomly( ) const;
    bool GetHumanNPPExtraction( ) const;

    void SetTimeStepUnits( const std::string& );
    void SetLengthOfSimulationInYears( const unsigned& );
    void SetUserMinimumLongitude( const int& );
    void SetUserMaximumLongitude( const int& );
    void SetUserMinimumLatitude( const int& );
    void SetUserMaximumLatitude( const int& );
    void SetGridCellSize( const unsigned& );
    void SetExtinctionThreshold( const float& );
    void SetMaximumNumberOfCohorts( const unsigned& );
    void SetPlanktonSizeThreshold( const float& );
    void SetDrawRandomly( const std::string& );
    void SetHumanNPPExtraction( const std::string& );

    // Calculated parameters
    unsigned GetLengthOfSimulationInTimeSteps( ) const;
    unsigned GetLengthDataLongitudeArray( ) const;
    unsigned GetLengthDataLatitudeArray( ) const;
    unsigned GetDataIndexOfUserMinimumLongitude( ) const;
    unsigned GetDataIndexOfUserMaximumLongitude( ) const;
    unsigned GetDataIndexOfUserMinimumLatitude( ) const;
    unsigned GetDataIndexOfUserMaximumLatitude( ) const;
    unsigned GetLengthUserLongitudeArray( ) const;
    unsigned GetLengthUserLatitudeArray( ) const;
    unsigned GetSizeOfGridDatum( ) const;
    float GetDataLongitudeAtIndex( const unsigned& ) const;
    float GetDataLatitudeAtIndex( const unsigned& ) const;
    float GetUserLongitudeAtIndex( const unsigned& ) const;
    float GetUserLatitudeAtIndex( const unsigned& ) const;

    float* GetDataLongitudeArray( ) const;
    float* GetDataLatitudeArray( ) const;
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
    std::string mTimeStepUnits;
    unsigned mLengthOfSimulationInYears;
    int mUserMinimumLongitude;
    int mUserMaximumLongitude;
    int mUserMinimumLatitude;
    int mUserMaximumLatitude;
    unsigned mGridCellSize;
    float mExtinctionThreshold;
    unsigned mMaximumNumberOfCohorts;
    float mPlanktonSizeThreshold;
    bool mDrawRandomly;
    bool mHumanNPPExtraction;

    // Calculated parameters
    unsigned mLengthOfSimulationInTimeSteps;
    unsigned mLengthDataLongitudeArray;
    unsigned mLengthDataLatitudeArray;
    unsigned mLengthUserLongitudeArray;
    unsigned mLengthUserLatitudeArray;
    unsigned mDataIndexOfUserMinimumLongitude;
    unsigned mDataIndexOfUserMaximumLongitude;
    unsigned mDataIndexOfUserMinimumLatitude;
    unsigned mDataIndexOfUserMaximumLatitude;
    unsigned mSizeOfGridDatum;
    float* mDataLongitudeArray;
    float* mDataLatitudeArray;
    float* mTimeStepArray;
    float* mUserLongitudeArray;
    float* mUserLatitudeArray;

    Types::GeoCoordsMatrix mDomainCoordsMatrix;
};

#endif

