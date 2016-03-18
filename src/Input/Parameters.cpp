#include "Parameters.h"

#include "Constants.h"
#include "Convertor.h"
#include "Logger.h"

Types::ParametersPointer Parameters::mThis = NULL;

Types::ParametersPointer Parameters::Get( ) {
    if( mThis == NULL ) {
        mThis = new Parameters( );
    }
    return mThis;
}

Parameters::~Parameters( ) {

    delete[ ] mLongitudeArray;
    delete[ ] mLatitudeArray;

    if( mThis != NULL ) {
        delete mThis;
    }
}

Parameters::Parameters( ) {
}

bool Parameters::Initialise( const Types::StringMatrix& rawInputParameterData ) {
    if( rawInputParameterData.size( ) > 0 ) {
        for( unsigned int rowIndex = 0; rowIndex < rawInputParameterData.size( ); ++rowIndex ) {

            float parameterValue = Convertor::Get( )->StringToNumber( rawInputParameterData[ rowIndex ][ Constants::eValue ] );

            switch( rowIndex ) {
                case Constants::eLengthOfSimulationInYears:
                {
                    mLengthOfSimulationInYears = parameterValue;
                    break;
                }
                case Constants::eMinimumLongitude:
                {
                    mMinimumLongitude = parameterValue;
                    break;
                }
                case Constants::eMaximumLongitude:
                {
                    mMaximumLongitude = parameterValue;
                    break;
                }
                case Constants::eMinimumLatitude:
                {
                    mMinimumLatitude = parameterValue;
                    break;
                }
                case Constants::eMaximumLatitude:
                {
                    mMaximumLatitude = parameterValue;
                    break;
                }
                case Constants::eGridCellSize:
                {
                    mGridCellSize = parameterValue;
                    break;
                }
            }
        }

        CalculateParameters( );
        
        return true;
    } else {
        return false;
    }
}

void Parameters::CalculateParameters( ) {

    // Calculate temporal parameters
    mLengthOfSimulationInTimeSteps = mLengthOfSimulationInYears * 12;
    mTimeStepArray = new float[ mLengthOfSimulationInTimeSteps ];

    for( unsigned int timeStep = 0; timeStep < mLengthOfSimulationInTimeSteps; ++timeStep ) {
        mTimeStepArray[ timeStep ] = timeStep;
    }

    // Calculate spatial parameters
    mLengthLongitudeArray = ( mMaximumLongitude - mMinimumLongitude ) / mGridCellSize;
    mLengthLatitudeArray = ( mMaximumLatitude - mMinimumLatitude ) / mGridCellSize;
    mSizeOfGridDatum = mLengthLongitudeArray * mLengthLatitudeArray * mLengthOfSimulationInTimeSteps;

    mLongitudeArray = new float[ mLengthLongitudeArray ];
    mLatitudeArray = new float[ mLengthLatitudeArray ];

    for( unsigned int longitudeIndex = 0; longitudeIndex < mLengthLongitudeArray; ++longitudeIndex ) {
        mLongitudeArray[ longitudeIndex ] = ( mMinimumLongitude + ( mGridCellSize / 2 ) ) + ( longitudeIndex * mGridCellSize );
    }

    for( unsigned int latitudeIndex = 0; latitudeIndex < mLengthLatitudeArray; ++latitudeIndex ) {
        mLatitudeArray[ latitudeIndex ] = ( mMinimumLatitude + ( mGridCellSize / 2 ) ) + ( latitudeIndex * mGridCellSize );
    }
}

unsigned int Parameters::GetLengthOfSimulationInYears( ) const {
    return mLengthOfSimulationInYears;
}

int Parameters::GetMinimumLongitude( ) const {
    return mMinimumLongitude;
}

int Parameters::GetMaximumLongitude( ) const {
    return mMaximumLongitude;
}

int Parameters::GetMinimumLatitude( ) const {
    return mMinimumLatitude;
}

int Parameters::GetMaximumLatitude( ) const {
    return mMaximumLatitude;
}

float Parameters::GetGridCellSize( ) const {
    return mGridCellSize;
}

void Parameters::SetLengthOfSimulationInYears( const unsigned int& lengthOfSimulationInYears ) {
    mLengthOfSimulationInYears = lengthOfSimulationInYears;
}

void Parameters::SetMinimumLongitude( const int& minimumLongitude ) {
    mMinimumLongitude = minimumLongitude;
}

void Parameters::SetMaximumLongitude( const int& maximumLongitude ) {
    mMaximumLongitude = maximumLongitude;
}

void Parameters::SetMinimumLatitude( const int& minimumLatitude ) {
    mMinimumLatitude = minimumLatitude;
}

void Parameters::SetMaximumLatitude( const int& maximumLatitude ) {
    mMaximumLatitude = maximumLatitude;
}

void Parameters::SetGridCellSize( const float& gridCellSize ) {
    mGridCellSize = gridCellSize;
}

unsigned int Parameters::GetLengthOfSimulationInTimeSteps( ) const {
    return mLengthOfSimulationInTimeSteps;
}

unsigned int Parameters::GetLengthLongitudeArray( ) const {
    return mLengthLongitudeArray;
}

unsigned int Parameters::GetLengthLatitudeArray( ) const {
    return mLengthLatitudeArray;
}

unsigned int Parameters::GetSizeOfGridDatum( ) const {
    return mSizeOfGridDatum;
}

float Parameters::GetLongitudeAtIndex( const unsigned int& index ) const {
    return mLongitudeArray[ index ];
}

float Parameters::GetLatitudeAtIndex( const unsigned int& index ) const {
    return mLatitudeArray[ index ];
}

float* Parameters::GetLongitudeArray( ) const {
    return mLongitudeArray;
}

float* Parameters::GetLatitudeArray( ) const {
    return mLatitudeArray;
}

float* Parameters::GetTimeStepArray( ) const {
    return mTimeStepArray;
}