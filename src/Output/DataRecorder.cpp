#include "DataRecorder.h"

#include "Constants.h"
#include "Logger.h"
#include "BasicDatum.h"
#include "GridDatum.h"
#include "Convertor.h"
#include "Parameters.h"

Types::DataRecorderPointer DataRecorder::mThis = NULL;

Types::DataRecorderPointer DataRecorder::Get( ) {
    if( mThis == NULL ) {
        mThis = new DataRecorder( );
    }
    return mThis;
}

DataRecorder::~DataRecorder( ) {

    for( Types::BasicDatumMap::iterator iter = mBasicDatumMap.begin( ); iter != mBasicDatumMap.end( ); ++iter ) {
        delete iter->second;
    }

    for( Types::GridDatumMap::iterator iter = mGridDatumMap.begin( ); iter != mGridDatumMap.end( ); ++iter ) {
        delete iter->second;
    }

    if( mThis != NULL ) {
        delete mThis;
    }
}

DataRecorder::DataRecorder( ) {

}

bool DataRecorder::Initialise( const Types::StringMatrix& rawOutputParameterData ) {

    if( rawOutputParameterData.size( ) > 0 ) {
        for( unsigned int rowIndex = 0; rowIndex < rawOutputParameterData.size( ); ++rowIndex ) {
            std::string name = rawOutputParameterData[ rowIndex ][ Constants::eOutputDatumName ];
            std::string units = Convertor::Get( )->ToLowercase( rawOutputParameterData[ rowIndex ][ Constants::eUnits ] );
            std::string type = Convertor::Get( )->ToLowercase( rawOutputParameterData[ rowIndex ][ Constants::eType ] );

            if( type == Constants::cBasicDatumTypeName ) {
                Types::StringVector basicDatumMetadata;
                basicDatumMetadata.push_back( name );
                basicDatumMetadata.push_back( units );
                mBasicOutputMetadata.push_back( basicDatumMetadata );

            } else if( type == Constants::cGridDatumTypeName ) {
                Types::StringVector gridDatumMetadata;
                gridDatumMetadata.push_back( name );
                gridDatumMetadata.push_back( units );
                mGridOutputMetadata.push_back( gridDatumMetadata );

            } else {
                Logger::Get( )->LogMessage( "ERROR> Defined output type \"" + type + "\" for datum \"" + name + "\" is not valid." );
            }
        }
        return true;
    } else {
        return false;
    }
}

void DataRecorder::AddDataTo( const std::string& name, const float& data ) {

    Types::BasicDatumPointer basicDatum = GetBasicDatum( name );

    if( basicDatum != NULL ) {
        basicDatum->AddData( data );
    }
}

void DataRecorder::AddDataTo( const std::string& name, const Types::GeoCoordPointer coord, const float& data ) {

    Types::GridDatumPointer gridDatum = GetGridDatum( name );

    if( gridDatum != NULL ) {
        gridDatum->AddData( coord, data );
    }
}

void DataRecorder::AddDataTo( const std::string& name, const Types::IndicesPointer indices, const float& data ) {

    Types::GridDatumPointer gridDatum = GetGridDatum( name );

    if( gridDatum != NULL ) {
        gridDatum->AddData( indices, data );
    }
}

Types::BasicDatumMap DataRecorder::GetBasicDatumMap( ) const {
    return mBasicDatumMap;
}

Types::GridDatumMap DataRecorder::GetGridDatumMap( ) const {
    return mGridDatumMap;
}

Types::StringVector DataRecorder::GetMetadataFilePaths( ) const {
    return mMetadataFilePaths;
}

void DataRecorder::AddMetadataFilePath( const std::string& metadataFilePath ) {
    mMetadataFilePaths.push_back( metadataFilePath );
}

Types::BasicDatumPointer DataRecorder::GetBasicDatum( const std::string& name ) {

    Types::BasicDatumPointer basicDatum = NULL;

    Types::BasicDatumMap::iterator iter = mBasicDatumMap.find( name );

    if( iter != mBasicDatumMap.end( ) ) {
        basicDatum = iter->second;
    } else {
        for( unsigned int datumIndex = 0; datumIndex < mBasicOutputMetadata.size( ); ++datumIndex ) {

            std::string basicDatumName = mBasicOutputMetadata[ datumIndex ][ Constants::eOutputDatumName ];

            if( Convertor::Get( )->ToLowercase( basicDatumName ) == Convertor::Get( )->ToLowercase( name ) ) {

                basicDatum = new BasicDatum( basicDatumName, mBasicOutputMetadata[ datumIndex ][ Constants::eUnits ] );
                mBasicDatumMap.insert( std::pair< std::string, Types::BasicDatumPointer >( basicDatumName, basicDatum ) );

                Logger::Get( )->LogMessage( "Initialising basic output datum \"" + name + "\"." );
                break;
            }
        }
    }

    // It is considered a feature to ignore an attempt to output to a non-existent datum. This could be because output to that datum is been switched off the OutputControlParameters.csv file.
    //    if( basicDatum == NULL ) {
    //        Logger::Get( )->LogMessage( "ERROR> Basic output datum \"" + name + "\" has not been defined in \"" + Constants::cOutputControlParametersFileName + "\"." );
    //    }

    return basicDatum;
}

Types::GridDatumPointer DataRecorder::GetGridDatum( const std::string& name ) {

    Types::GridDatumPointer gridDatum = NULL;

    Types::GridDatumMap::iterator iter = mGridDatumMap.find( name );

    if( iter != mGridDatumMap.end( ) ) {
        gridDatum = iter->second;
    } else {
        for( unsigned int datumIndex = 0; datumIndex < mGridOutputMetadata.size( ); ++datumIndex ) {

            std::string gridDatumName = mGridOutputMetadata[ datumIndex ][ Constants::eOutputDatumName ];

            if( Convertor::Get( )->ToLowercase( gridDatumName ) == Convertor::Get( )->ToLowercase( name ) ) {

                gridDatum = new GridDatum( gridDatumName, mGridOutputMetadata[ datumIndex ][ Constants::eUnits ] );
                mGridDatumMap.insert( std::pair< std::string, Types::GridDatumPointer >( gridDatumName, gridDatum ) );

                Logger::Get( )->LogMessage( "Initialising grid output datum \"" + name + "\"." );
                break;
            }
        }
    }

    // It is considered a feature to ignore an attempt to output to a non-existent datum. This could be because output to that datum is been switched off the OutputControlParameters.csv file.
    //    if( gridDatum == NULL ) {
    //        Logger::Get( )->LogMessage( "ERROR> Grid output datum \"" + name + "\" has not been defined in \"" + Constants::cOutputControlParametersFileName + "\"." );
    //    }

    return gridDatum;
}