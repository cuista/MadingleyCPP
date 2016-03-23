#include "Parameters.h"

#include "Constants.h"
#include "Convertor.h"
#include "Logger.h"
#include "Maths.h"
#include "Processor.h"

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
    delete[ ] mTimeStepArray;

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
                    mUserMinimumLongitude = parameterValue;
                    break;
                }
                case Constants::eMaximumLongitude:
                {
                    mUserMaximumLongitude = parameterValue;
                    break;
                }
                case Constants::eMinimumLatitude:
                {
                    mUserMinimumLatitude = parameterValue;
                    break;
                }
                case Constants::eMaximumLatitude:
                {
                    mUserMaximumLatitude = parameterValue;
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
    mLengthLongitudeArray = 360 / mGridCellSize;
    mLongitudeArray = new float[ mLengthLongitudeArray ];
    for( unsigned int longitudeIndex = 0; longitudeIndex < mLengthLongitudeArray; ++longitudeIndex ) {
        mLongitudeArray[ longitudeIndex ] = ( -180 + ( mGridCellSize / 2 ) ) + ( longitudeIndex * mGridCellSize );
    }

    mLengthLatitudeArray = 180 / mGridCellSize;
    mLatitudeArray = new float[ mLengthLatitudeArray ];
    for( unsigned int latitudeIndex = 0; latitudeIndex < mLengthLatitudeArray; ++latitudeIndex ) {
        mLatitudeArray[ latitudeIndex ] = ( -90 + ( mGridCellSize / 2 ) ) + ( latitudeIndex * mGridCellSize );
    }

    mIndexOfUserMinimumLongitude = Processor::Get( )->CalculateArrayIndexOfValue( mLongitudeArray, mLengthLongitudeArray, mUserMinimumLongitude );
    mIndexOfUserMaximumLongitude = Processor::Get( )->CalculateArrayIndexOfValue( mLongitudeArray, mLengthLongitudeArray, mUserMaximumLongitude );
    mLengthUserLongitudeArray = ( mIndexOfUserMaximumLongitude - mIndexOfUserMinimumLongitude ) + 1;

    mUserLongitudeArray = new float[ mLengthUserLongitudeArray ];
    for( unsigned int userLongitudeIndex = 0; userLongitudeIndex < mLengthUserLongitudeArray; ++userLongitudeIndex ) {
        mUserLongitudeArray[ userLongitudeIndex ] = mLongitudeArray[ userLongitudeIndex + mIndexOfUserMinimumLongitude ];
    }

    mIndexOfUserMinimumLatitude = Processor::Get( )->CalculateArrayIndexOfValue( mLatitudeArray, mLengthLatitudeArray, mUserMinimumLatitude );
    mIndexOfUserMaximumLatitude = Processor::Get( )->CalculateArrayIndexOfValue( mLatitudeArray, mLengthLatitudeArray, mUserMaximumLatitude );
    mLengthUserLatitudeArray = ( mIndexOfUserMaximumLatitude - mIndexOfUserMinimumLatitude ) + 1;

    mUserLatitudeArray = new float[ mLengthUserLatitudeArray ];
    for( unsigned int userLatitudeIndex = 0; userLatitudeIndex < mLengthUserLatitudeArray; ++userLatitudeIndex ) {
        mUserLatitudeArray[ userLatitudeIndex ] = mLatitudeArray[ userLatitudeIndex + mIndexOfUserMinimumLatitude ];
    }

    mSizeOfGridDatum = mLengthUserLongitudeArray * mLengthUserLatitudeArray * mLengthOfSimulationInTimeSteps;

    mDomainCoordsMatrix.resize( mLengthUserLongitudeArray );
    unsigned int domainLongitudeIndex = 0;
    for( unsigned int longitudeIndex = mIndexOfUserMinimumLongitude; longitudeIndex <= mIndexOfUserMaximumLongitude; ++longitudeIndex ) {
        for( unsigned int latitudeIndex = mIndexOfUserMinimumLatitude; latitudeIndex <= mIndexOfUserMaximumLatitude; ++latitudeIndex ) {
            mDomainCoordsMatrix[ domainLongitudeIndex ].push_back( std::make_pair( mLongitudeArray[ longitudeIndex ], mLatitudeArray[ latitudeIndex ] ) );
        }
        ++domainLongitudeIndex;
    }

    //    for( unsigned int domainLongitudeIndex = 0; domainLongitudeIndex < mLengthUserLongitudeArray; ++domainLongitudeIndex ) {
    //        for( unsigned int domainLatitudeIndex = 0; domainLatitudeIndex < mLengthUserLatitudeArray; ++domainLatitudeIndex ) {
    //            Logger::Get( )->LogMessageNoReturn( "lon> " );
    //            Logger::Get( )->LogMessageNoReturn( mDomainCoordMatrix[ domainLongitudeIndex ][ domainLatitudeIndex ].first );
    //            Logger::Get( )->LogMessageNoReturn( ", lat> " );
    //            Logger::Get( )->LogMessage( mDomainCoordMatrix[ domainLongitudeIndex ][ domainLatitudeIndex ].second );
    //        }
    //    }
}

unsigned int Parameters::GetLengthOfSimulationInYears( ) const {
    return mLengthOfSimulationInYears;
}

int Parameters::GetUserMinimumLongitude( ) const {
    return mUserMinimumLongitude;
}

int Parameters::GetUserMaximumLongitude( ) const {
    return mUserMaximumLongitude;
}

int Parameters::GetUserMinimumLatitude( ) const {
    return mUserMinimumLatitude;
}

int Parameters::GetUserMaximumLatitude( ) const {
    return mUserMaximumLatitude;
}

float Parameters::GetGridCellSize( ) const {
    return mGridCellSize;
}

void Parameters::SetLengthOfSimulationInYears( const unsigned int& lengthOfSimulationInYears ) {
    mLengthOfSimulationInYears = lengthOfSimulationInYears;
}

void Parameters::SetUserMinimumLongitude( const int& userMinimumLongitude ) {
    mUserMinimumLongitude = userMinimumLongitude;
}

void Parameters::SetUserMaximumLongitude( const int& userMaximumLongitude ) {
    mUserMaximumLongitude = userMaximumLongitude;
}

void Parameters::SetUserMinimumLatitude( const int& userMinimumLatitude ) {
    mUserMinimumLatitude = userMinimumLatitude;
}

void Parameters::SetUserMaximumLatitude( const int& userMaximumLatitude ) {
    mUserMaximumLatitude = userMaximumLatitude;
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

unsigned int Parameters::GetIndexOfUserMinimumLongitude( ) const {
    return mIndexOfUserMinimumLongitude;
}

unsigned int Parameters::GetIndexOfUserMaximumLongitude( ) const {
    return mIndexOfUserMaximumLongitude;
}

unsigned int Parameters::GetIndexOfUserMinimumLatitude( ) const {
    return mIndexOfUserMinimumLatitude;
}

unsigned int Parameters::GetIndexOfUserMaximumLatitude( ) const {
    return mIndexOfUserMaximumLatitude;
}

unsigned int Parameters::GetLengthUserLongitudeArray( ) const {
    return mLengthUserLongitudeArray;
}

unsigned int Parameters::GetLengthUserLatitudeArray( ) const {
    return mLengthUserLatitudeArray;
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

float* Parameters::GetUserLongitudeArray( ) const {
    return mUserLongitudeArray;
}

float* Parameters::GetUserLatitudeArray( ) const {
    return mUserLatitudeArray;
}

Types::GeoCoords Parameters::GetCoordsFromDomainIndices( const Types::GeoIndices indices ) const {
    return GetCoordsFromDomainIndices( indices.first, indices.second );
}

Types::GeoCoords Parameters::GetCoordsFromDomainIndices( const unsigned short& domainLonditudeIndex, const unsigned short& domainLatitudeIndex ) const {

    Types::GeoCoords coords;

    coords = mDomainCoordsMatrix[ domainLonditudeIndex ][ domainLatitudeIndex ];

    return coords;
}

Types::GeoIndices Parameters::GetDomainIndicesFromCoords( const Types::GeoCoords coords ) const {
    return GetDomainIndicesFromCoords( coords.first, coords.second );
}

Types::GeoIndices Parameters::GetDomainIndicesFromCoords( const float& longitude, const float& latitude ) const {

    Types::GeoIndices indices;

    indices.first = Processor::Get( )->CalculateArrayIndexOfValue( mUserLongitudeArray, mLengthUserLongitudeArray, longitude );
    indices.second = Processor::Get( )->CalculateArrayIndexOfValue( mUserLatitudeArray, mLengthUserLatitudeArray, latitude );

    return indices;
}