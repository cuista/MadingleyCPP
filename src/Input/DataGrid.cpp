#include "DataGrid.h"
#include "Logger.h"
#include "Convertor.h"
#include "Constants.h"

Types::DataGridPointer DataGrid::mThis = 0;

Types::DataGridPointer DataGrid::Get( ) {
    if( mThis == 0 ) {
        mThis = new DataGrid( );
    }
    return mThis;
}

DataGrid::DataGrid( ) {

}

DataGrid::~DataGrid( ) {
    for( unsigned int longitudeIndex = 0; longitudeIndex < mLongitudeVector.size( ); ++longitudeIndex ) {
        for( unsigned int latitudeIndex = 0; latitudeIndex < mLatitudeVector.size( ); ++latitudeIndex ) {
            // delete [] mGridCells[ longitudeIndex ][ latitudeIndex ];
        }
    }

    if( mThis != 0 ) {
        // delete mThis;
    }
}

void DataGrid::AddData( const double* dataArray, const unsigned int variableIndex ) {

    for( unsigned int timeIndex = 0; timeIndex < mTimeVector.size( ); ++timeIndex ) {
        for( unsigned int latitudeIndex = 0; latitudeIndex < mLatitudeVector.size( ); ++latitudeIndex ) {
            for( unsigned int longitudeIndex = 0; longitudeIndex < mLongitudeVector.size( ); ++longitudeIndex ) {

                unsigned int dataIndex = ( timeIndex * mLatitudeVector.size( ) * mLongitudeVector.size( ) ) + ( latitudeIndex * mLongitudeVector.size( ) ) + longitudeIndex;
                //Logger::Get( )->LogString( "dataIndex> " + Convertor::Get( )->NumberToString( dataIndex ) );

                double val = dataArray[ dataIndex ];

                switch( variableIndex ) {
                    case Constants::eDiurnalTemperatureRange:
                    {
                        mGridCells[ longitudeIndex ][ latitudeIndex ]->SetDiurnalTemperatureRange( val, timeIndex );
                        break;
                    }

                    case Constants::eGroundFrostFrequency:
                    {
                        mGridCells[ longitudeIndex ][ latitudeIndex ]->SetGroundFrostFrequency( val, timeIndex );
                        break;
                    }

                    case Constants::ePrecipitation:
                    {
                        mGridCells[ longitudeIndex ][ latitudeIndex ]->SetPrecipitation( val, timeIndex );
                        break;
                    }

                    case Constants::eNearSurfaceTemperature:
                    {
                        mGridCells[ longitudeIndex ][ latitudeIndex ]->SetNearSurfaceTemperature( val, timeIndex );
                        break;
                    }
                    case Constants::eUSpeed:
                    {
                        mGridCells[ longitudeIndex ][ latitudeIndex ]->SetUSpeed( val, timeIndex );
                        break;
                    }
                    case Constants::eVSpeed:
                    {
                        mGridCells[ longitudeIndex ][ latitudeIndex ]->SetVSpeed( val, timeIndex );
                        break;
                    }
                    case Constants::eWaterCapacity:
                    {
                        mGridCells[ longitudeIndex ][ latitudeIndex ]->SetWaterCapacity( val, timeIndex );
                        break;
                    }
                    case Constants::eSST:
                    {
                        mGridCells[ longitudeIndex ][ latitudeIndex ]->SetSST( val, timeIndex );
                        break;
                    }
                    case Constants::eNPP:
                    {
                        mGridCells[ longitudeIndex ][ latitudeIndex ]->SetNPP( val, timeIndex );
                        break;
                    }
                    case Constants::eOceanNPP:
                    {
                        mGridCells[ longitudeIndex ][ latitudeIndex ]->SetOceanNPP( val, timeIndex );
                        break;
                    }
                    default:
                    {
                        Logger::Get( )->LogString( "ERROR> \"" + Convertor::Get( )->NumberToString( variableIndex ) + "\" is not a recognised variable index." );
                        break;
                    }
                }
            }
        }
    }
}

void DataGrid::InitialiseGridCells( const int* isOceanArray, const double* longitudeArray, const double* latitudeArray, const unsigned int lengthLongitudeArray, const unsigned int lengthLatitudeArray ) {

    mLongitudeVector.assign( longitudeArray, longitudeArray + lengthLongitudeArray );
    mLatitudeVector.assign( latitudeArray, latitudeArray + lengthLatitudeArray );

    mGridCells.resize( mLongitudeVector.size( ) );
    for( unsigned int longitudeIndex = 0; longitudeIndex < mLongitudeVector.size( ); ++longitudeIndex ) {
        mGridCells[ longitudeIndex ].resize( mLatitudeVector.size( ) );
        for( unsigned int latitudeIndex = 0; latitudeIndex < mLatitudeVector.size( ); ++latitudeIndex ) {
            unsigned int isOceanArrayIndex = ( latitudeIndex * mLongitudeVector.size( ) ) + longitudeIndex;
            Logger::Get( )->LogStringNoReturn( Convertor::Get( )->NumberToString( isOceanArray[ isOceanArrayIndex ] ) );
            mGridCells[ longitudeIndex ][ latitudeIndex ] = new NcGridCell( isOceanArray[ isOceanArrayIndex ] );
        }
        Logger::Get( )->LogString( "" );
    }
}

void DataGrid::InitialiseGridCellVectors( const unsigned int* timeArray, const unsigned int lengthTimeArray ) {

    mTimeVector.assign( timeArray, timeArray + lengthTimeArray );

    for( unsigned int longitudeIndex = 0; longitudeIndex < mLongitudeVector.size( ); ++longitudeIndex ) {
        for( unsigned int latitudeIndex = 0; latitudeIndex < mLatitudeVector.size( ); ++latitudeIndex ) {
            mGridCells[ longitudeIndex ][ latitudeIndex ]->InitialiseVectors( lengthTimeArray );
        }
    }
}

unsigned int DataGrid::GetLengthLongitudeVector( ) const {
    return mLongitudeVector.size( );
}

unsigned int DataGrid::GetLengthLatitudeVector( ) const {
    return mLatitudeVector.size( );
}

unsigned int DataGrid::GetLengthTimeVector( ) const {
    return mTimeVector.size( );
}

Types::FloatVector DataGrid::GetLongitudeVector( ) const {
    return mLongitudeVector;
}

Types::FloatVector DataGrid::GetLatitudeVector( ) const {
    return mLatitudeVector;
}

NcGridCell* DataGrid::GetGridCell( const unsigned int longitudeIndex, const unsigned int latitudeIndex ) const {
    return mGridCells[ longitudeIndex ][ latitudeIndex ];
}
