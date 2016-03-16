#ifndef PARAMETERS
#define	PARAMETERS

#include "Types.h"

class Parameters {
public:
    ~Parameters( );
    static Types::ParametersPointer Get( );

    void Initialise( Types::StringMatrix& );

    // User defined parameters
    unsigned int GetLengthOfSimulationInYears( ) const;
    int GetMinimumLatitude( ) const;
    int GetMaximumLatitude( ) const;
    int GetMinimumLongitude( ) const;
    int GetMaximumLongitude( ) const;
    float GetGridCellSize( ) const;

    void SetLengthOfSimulationInYears( const unsigned int& );
    void SetMinimumLongitude( const int& );
    void SetMaximumLongitude( const int& );
    void SetMinimumLatitude( const int& );
    void SetMaximumLatitude( const int& );
    void SetGridCellSize( const float& );

    // Calculated parameters
    unsigned int GetLengthOfSimulationInTimeSteps( ) const;
    unsigned int GetLengthLongitudeArray( ) const;
    unsigned int GetLengthLatitudeArray( ) const;
    unsigned int GetSizeOfGridDatum( ) const;
    float GetLongitudeAtIndex( const unsigned int& ) const;
    float GetLatitudeAtIndex( const unsigned int& ) const;
    float* GetLongitudeArray( ) const;
    float* GetLatitudeArray( ) const;
    float* GetTimeStepArray( ) const;

private:
    Parameters( );
    void CalculateParameters( );
    
    static Types::ParametersPointer mThis;

    // User defined parameters
    unsigned int mLengthOfSimulationInYears;
    int mMinimumLongitude;
    int mMaximumLongitude;
    int mMinimumLatitude;
    int mMaximumLatitude;
    float mGridCellSize;

    // Calculated parameters
    unsigned int mLengthOfSimulationInTimeSteps;
    unsigned int mLengthLongitudeArray;
    unsigned int mLengthLatitudeArray;
    unsigned int mSizeOfGridDatum;
    float* mLongitudeArray;
    float* mLatitudeArray;
    float* mTimeStepArray;
};

#endif

