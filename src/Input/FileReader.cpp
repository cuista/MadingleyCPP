#include "FileReader.h"

#include "Logger.h"
#include "Convertor.h"
#include "Constants.h"
#include "InputData.h"
#include "DataLayerSet.h"
#include "DataRecorder.h"
#include "Parameters.h"

#include <netcdf>

FileReader::FileReader( ) {

}

FileReader::~FileReader( ) {

}

bool FileReader::ReadFiles( ) {

    std::string inputFilePath = Constants::cConfigurationDirectory + Constants::cSimulationControlParametersFileName;
    bool completedSuccessfully = ReadMetadataFile( inputFilePath );

    if( completedSuccessfully == true ) {
        DataRecorder::Get( )->AddMetadataFilePath( inputFilePath );
        Parameters::Get( )->Initialise( mMetadata );
        inputFilePath = Constants::cConfigurationDirectory + Constants::cOutputControlParametersFileName;
        completedSuccessfully = ReadMetadataFile( inputFilePath );
    }

    if( completedSuccessfully == true ) {
        DataRecorder::Get( )->AddMetadataFilePath( inputFilePath );
        DataRecorder::Get( )->Initialise( mMetadata );
        inputFilePath = Constants::cConfigurationDirectory + Constants::cEnvironmentalDataLayersFileName;
        completedSuccessfully = ReadMetadataFile( inputFilePath );
        ReadNetCDFFiles( );
    }

    if( completedSuccessfully == true ) {
        DataRecorder::Get( )->AddMetadataFilePath( inputFilePath );
    }

    return completedSuccessfully;
}

void FileReader::ClearMetadata( ) {
    for( unsigned int xIndex = 0; xIndex < mMetadata.size( ); ++xIndex ) {
        mMetadata[ xIndex ].clear( );
    }
    mMetadata.clear( );
}

bool FileReader::ReadMetadataFile( const std::string& filePath ) {

    bool completedSuccessfully = false;

    ClearMetadata( );

    Logger::Get( )->LogMessage( "Reading text file \"" + filePath + "\"..." );
    std::ifstream fileStream;
    fileStream.open( filePath.c_str( ) );

    if( fileStream.is_open( ) ) {
        std::string readLine;
        unsigned int lineCount = 0;

        while( std::getline( fileStream, readLine ) ) {
            if( lineCount > 0 && readLine[ 0 ] != Constants::cTextFileCommentCharacter ) {
                mMetadata.push_back( Convertor::Get( )->StringToWords( readLine, Constants::cDataDelimiterValue ) );
            }
            ++lineCount;
        }
        completedSuccessfully = true;
    } else {
        Logger::Get( )->LogMessage( "File path \"" + filePath + "\" is invalid." );
    }
    fileStream.close( );

    return completedSuccessfully;
}

bool FileReader::ReadNetCDFFiles( ) {

    bool completedSuccessfully = false;

    Types::InputDataPointer initialInputData = new InputData( );

    for( unsigned int environmentalDataFileIndex = 0; environmentalDataFileIndex < mMetadata.size( ); ++environmentalDataFileIndex ) {

        std::string filePath = mMetadata[ environmentalDataFileIndex ][ Constants::eFilePath ];
        Logger::Get( )->LogMessage( "Reading NetCDF file \"" + filePath + "\"..." );

        try {
            netCDF::NcFile inputNcFile( filePath, netCDF::NcFile::read ); // Open the file for read access
            std::multimap< std::string, netCDF::NcVar > multiMap = inputNcFile.getVars( );

            // Outer variable loop
            for( std::multimap<std::string, netCDF::NcVar>::iterator it = multiMap.begin( ); it != multiMap.end( ); ++it ) {

                std::string variableName = ( *it ).first;
                netCDF::NcVar variableNcVar = ( *it ).second;
                std::vector< netCDF::NcDim > varDims = variableNcVar.getDims( );

                Types::UnsignedIntegerVector variableDimensions;
                unsigned int variableSize = 0;

                // Inner variable dimension loop
                for( unsigned int dimIndex = 0; dimIndex < varDims.size( ); ++dimIndex ) {
                    variableDimensions.push_back( varDims[ dimIndex ].getSize( ) );

                    if( dimIndex == 0 ) {
                        variableSize = varDims[ dimIndex ].getSize( );
                    } else {
                        variableSize *= varDims[ dimIndex ].getSize( );
                    }
                }

                float* variableData = new float[ variableSize ];
                variableNcVar.getVar( variableData );
                bool isDefault = variableName == mMetadata[ environmentalDataFileIndex ][ Constants::eDefaultVariableName ];
                initialInputData->AddVariableToDatum( mMetadata[ environmentalDataFileIndex ][ Constants::eInternalName ], variableName, variableDimensions, variableSize, variableData, isDefault );
            }

            completedSuccessfully = true;

        } catch( netCDF::exceptions::NcException& e ) {
            e.what( );
            Logger::Get( )->LogMessage( "ERROR> File path \"" + filePath + "\" is invalid." );
        }
    }

    DataLayerSet::Get( )->SetDataLayers( initialInputData );

    return completedSuccessfully;
}
