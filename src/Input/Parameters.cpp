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

    delete[ ] mDataLongitudeArray;
    delete[ ] mDataLatitudeArray;
    delete[ ] mTimeStepArray;

    if( mThis != NULL ) {
        delete mThis;
    }
}

Parameters::Parameters( ) {
}

bool Parameters::Initialise( const Types::StringMatrix& rawInputParameterData ) {
    if( rawInputParameterData.size( ) > 0 ) {
        for( unsigned rowIndex = 0; rowIndex < rawInputParameterData.size( ); ++rowIndex ) {

            std::string parameterName = Convertor::Get( )->ToLowercase( Convertor::Get( )->RemoveWhiteSpace( rawInputParameterData[ rowIndex ][ Constants::eParameterName ] ) );

            if( parameterName == "timestepunits" ) SetTimeStepUnits( Convertor::Get( )->RemoveWhiteSpace( rawInputParameterData[ rowIndex ][ Constants::eParameterValue ] ) );
            if( parameterName == "lengthofsimulationinyears" ) SetLengthOfSimulationInYears( Convertor::Get( )->StringToNumber( Convertor::Get( )->RemoveWhiteSpace( rawInputParameterData[ rowIndex ][ Constants::eParameterValue ] ) ) );
            if( parameterName == "minimumlongitude" ) SetUserMinimumLongitude( Convertor::Get( )->StringToNumber( Convertor::Get( )->RemoveWhiteSpace( rawInputParameterData[ rowIndex ][ Constants::eParameterValue ] ) ) );
            if( parameterName == "maximumlongitude" ) SetUserMaximumLongitude( Convertor::Get( )->StringToNumber( Convertor::Get( )->RemoveWhiteSpace( rawInputParameterData[ rowIndex ][ Constants::eParameterValue ] ) ) );
            if( parameterName == "minimumlatitude" ) SetUserMinimumLatitude( Convertor::Get( )->StringToNumber( Convertor::Get( )->RemoveWhiteSpace( rawInputParameterData[ rowIndex ][ Constants::eParameterValue ] ) ) );
            if( parameterName == "maximumlatitude" ) SetUserMaximumLatitude( Convertor::Get( )->StringToNumber( Convertor::Get( )->RemoveWhiteSpace( rawInputParameterData[ rowIndex ][ Constants::eParameterValue ] ) ) );
            if( parameterName == "gridcellsize" ) SetGridCellSize( Convertor::Get( )->StringToNumber( Convertor::Get( )->RemoveWhiteSpace( rawInputParameterData[ rowIndex ][ Constants::eParameterValue ] ) ) );
            if( parameterName == "extinctionthreshold" ) SetExtinctionThreshold( Convertor::Get( )->StringToNumber( Convertor::Get( )->RemoveWhiteSpace( rawInputParameterData[ rowIndex ][ Constants::eParameterValue ] ) ) );
            if( parameterName == "maximumnumberofcohorts" ) SetMaximumNumberOfCohorts( Convertor::Get( )->StringToNumber( Convertor::Get( )->RemoveWhiteSpace( rawInputParameterData[ rowIndex ][ Constants::eParameterValue ] ) ) );
            if( parameterName == "planktonsizethreshold" ) SetPlanktonSizeThreshold( Convertor::Get( )->StringToNumber( Convertor::Get( )->RemoveWhiteSpace( rawInputParameterData[ rowIndex ][ Constants::eParameterValue ] ) ) );
            if( parameterName == "drawrandomly" ) SetDrawRandomly( Convertor::Get( )->RemoveWhiteSpace( rawInputParameterData[ rowIndex ][ Constants::eParameterValue ] ) );
            if( parameterName == "humannppextraction" ) SetHumanNPPExtraction( Convertor::Get( )->RemoveWhiteSpace( rawInputParameterData[ rowIndex ][ Constants::eParameterValue ] ) );
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

    for( unsigned timeStep = 0; timeStep < mLengthOfSimulationInTimeSteps; ++timeStep ) {
        mTimeStepArray[ timeStep ] = timeStep;
    }

    // Calculate spatial parameters
    mLengthDataLongitudeArray = 360 / mGridCellSize;
    mDataLongitudeArray = new float[ mLengthDataLongitudeArray ];
    for( unsigned longitudeIndex = 0; longitudeIndex < mLengthDataLongitudeArray; ++longitudeIndex ) {
        mDataLongitudeArray[ longitudeIndex ] = ( -180 + ( mGridCellSize / 2 ) ) + ( longitudeIndex * mGridCellSize );
    }

    mLengthDataLatitudeArray = 180 / mGridCellSize;
    mDataLatitudeArray = new float[ mLengthDataLatitudeArray ];
    for( unsigned latitudeIndex = 0; latitudeIndex < mLengthDataLatitudeArray; ++latitudeIndex ) {
        mDataLatitudeArray[ latitudeIndex ] = ( -90 + ( mGridCellSize / 2 ) ) + ( latitudeIndex * mGridCellSize );
    }

    mDataIndexOfUserMinimumLongitude = Processor::Get( )->CalculateArrayIndexOfValue( mDataLongitudeArray, mLengthDataLongitudeArray, mUserMinimumLongitude );
    mDataIndexOfUserMaximumLongitude = Processor::Get( )->CalculateArrayIndexOfValue( mDataLongitudeArray, mLengthDataLongitudeArray, mUserMaximumLongitude );
    mLengthUserLongitudeArray = ( mDataIndexOfUserMaximumLongitude - mDataIndexOfUserMinimumLongitude ) + 1;

    mUserLongitudeArray = new float[ mLengthUserLongitudeArray ];
    for( unsigned userLongitudeIndex = 0; userLongitudeIndex < mLengthUserLongitudeArray; ++userLongitudeIndex ) {
        mUserLongitudeArray[ userLongitudeIndex ] = mDataLongitudeArray[ userLongitudeIndex + mDataIndexOfUserMinimumLongitude ];
    }

    mDataIndexOfUserMinimumLatitude = Processor::Get( )->CalculateArrayIndexOfValue( mDataLatitudeArray, mLengthDataLatitudeArray, mUserMinimumLatitude );
    mDataIndexOfUserMaximumLatitude = Processor::Get( )->CalculateArrayIndexOfValue( mDataLatitudeArray, mLengthDataLatitudeArray, mUserMaximumLatitude );
    mLengthUserLatitudeArray = ( mDataIndexOfUserMaximumLatitude - mDataIndexOfUserMinimumLatitude ) + 1;

    mUserLatitudeArray = new float[ mLengthUserLatitudeArray ];
    for( unsigned userLatitudeIndex = 0; userLatitudeIndex < mLengthUserLatitudeArray; ++userLatitudeIndex ) {
        mUserLatitudeArray[ userLatitudeIndex ] = mDataLatitudeArray[ userLatitudeIndex + mDataIndexOfUserMinimumLatitude ];
    }

    mSizeOfGridDatum = mLengthUserLongitudeArray * mLengthUserLatitudeArray * mLengthOfSimulationInTimeSteps;

    mDomainCoordsMatrix.resize( mLengthUserLongitudeArray );
    unsigned domainLongitudeIndex = 0;
    for( unsigned longitudeIndex = mDataIndexOfUserMinimumLongitude; longitudeIndex <= mDataIndexOfUserMaximumLongitude; ++longitudeIndex ) {
        for( unsigned latitudeIndex = mDataIndexOfUserMinimumLatitude; latitudeIndex <= mDataIndexOfUserMaximumLatitude; ++latitudeIndex ) {
            mDomainCoordsMatrix[ domainLongitudeIndex ].push_back( std::make_pair( mDataLongitudeArray[ longitudeIndex ], mDataLatitudeArray[ latitudeIndex ] ) );
        }
        ++domainLongitudeIndex;
    }
}

std::string Parameters::GetTimeStepUnits( ) const {
    return mTimeStepUnits;
}

unsigned Parameters::GetLengthOfSimulationInYears( ) const {
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

unsigned Parameters::GetGridCellSize( ) const {
    return mGridCellSize;
}

float Parameters::GetExtinctionThreshold( ) const {
    return mExtinctionThreshold;
}

unsigned Parameters::GetMaximumNumberOfCohorts( ) const {
    return mMaximumNumberOfCohorts;
}

float Parameters::GetPlanktonSizeThreshold( ) const {
    return mPlanktonSizeThreshold;
}

bool Parameters::GetDrawRandomly( ) const {
    return mDrawRandomly;
}

bool Parameters::GetHumanNPPExtraction( ) const {
    return mHumanNPPExtraction;
}

void Parameters::SetTimeStepUnits( const std::string timeStepUnits ) {
    mTimeStepUnits = timeStepUnits;
}

void Parameters::SetLengthOfSimulationInYears( const unsigned& lengthOfSimulationInYears ) {
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

void Parameters::SetGridCellSize( const unsigned& gridCellSize ) {
    mGridCellSize = gridCellSize;
}

void Parameters::SetExtinctionThreshold( const float& extinctionThreshold ) {
    mExtinctionThreshold = extinctionThreshold;
}

void Parameters::SetMaximumNumberOfCohorts( const unsigned& maximumNumberOfCohorts ) {
    mMaximumNumberOfCohorts = maximumNumberOfCohorts;
}

void Parameters::SetPlanktonSizeThreshold( const float& planktonSizeThreshold ) {
    mPlanktonSizeThreshold = planktonSizeThreshold;
}

void Parameters::SetDrawRandomly( const std::string drawRandomlyString ) {
    if( drawRandomlyString == "no" )
        mDrawRandomly = false;
    else
        mDrawRandomly = true;
}

void Parameters::SetHumanNPPExtraction( const std::string humanNPPExtractionString ) {
    if( humanNPPExtractionString == "no" )
        mHumanNPPExtraction = false;
    else
        mHumanNPPExtraction = true;
}

unsigned Parameters::GetLengthOfSimulationInTimeSteps( ) const {
    return mLengthOfSimulationInTimeSteps;
}

unsigned Parameters::GetLengthDataLongitudeArray( ) const {
    return mLengthDataLongitudeArray;
}

unsigned Parameters::GetLengthDataLatitudeArray( ) const {
    return mLengthDataLatitudeArray;
}

unsigned Parameters::GetDataIndexOfUserMinimumLongitude( ) const {
    return mDataIndexOfUserMinimumLongitude;
}

unsigned Parameters::GetDataIndexOfUserMaximumLongitude( ) const {
    return mDataIndexOfUserMaximumLongitude;
}

unsigned Parameters::GetDataIndexOfUserMinimumLatitude( ) const {
    return mDataIndexOfUserMinimumLatitude;
}

unsigned Parameters::GetDataIndexOfUserMaximumLatitude( ) const {
    return mDataIndexOfUserMaximumLatitude;
}

unsigned Parameters::GetLengthUserLongitudeArray( ) const {
    return mLengthUserLongitudeArray;
}

unsigned Parameters::GetLengthUserLatitudeArray( ) const {
    return mLengthUserLatitudeArray;
}

unsigned Parameters::GetSizeOfGridDatum( ) const {
    return mSizeOfGridDatum;
}

float Parameters::GetDataLongitudeAtIndex( const unsigned& index ) const {
    return mDataLongitudeArray[ index ];
}

float Parameters::GetDataLatitudeAtIndex( const unsigned& index ) const {
    return mDataLatitudeArray[ index ];
}

float* Parameters::GetDataLongitudeArray( ) const {
    return mDataLongitudeArray;
}

float* Parameters::GetDataLatitudeArray( ) const {
    return mDataLatitudeArray;
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