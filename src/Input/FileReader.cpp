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
    
    // Input parameters
    bool success = Parameters::Get( )->Initialise( ReadAndReturnTextFile( Constants::cConfigurationDirectory + Constants::cInputParametersFileName ) );
    
    // Output variables
    if( success == true )
        success = DataRecorder::Get( )->Initialise( ReadAndReturnTextFile( Constants::cConfigurationDirectory + Constants::cOutputVariablesFileName ) );
    
    // Input data
    if( success == true )
        success = ReadInputDataFiles( ReadAndReturnTextFile( Constants::cConfigurationDirectory + Constants::cInputDataFileName ) );

    return success;
}

Types::StringMatrix FileReader::ReadAndReturnTextFile( const std::string& filePath ) {

    Types::StringMatrix metadata;

    DataRecorder::Get( )->AddMetadataFilePath( filePath );

    Logger::Get( )->LogMessage( "Reading text file \"" + filePath + "\"..." );
    std::ifstream fileStream;
    fileStream.open( filePath.c_str( ) );

    if( fileStream.is_open( ) ) {
        std::string readLine;
        unsigned int lineCount = 0;

        while( std::getline( fileStream, readLine ) ) {
            if( lineCount > 0 && readLine[ 0 ] != Constants::cTextFileCommentCharacter ) {
                metadata.push_back( Convertor::Get( )->StringToWords( readLine, Constants::cDataDelimiterValue ) );
            }
            ++lineCount;
        }
    } else {
        Logger::Get( )->LogMessage( "File path \"" + filePath + "\" is invalid." );
    }
    fileStream.close( );

    return metadata;
}

bool FileReader::ReadInputDataFiles( const Types::StringMatrix& inputFileMetadata ) {

    if( inputFileMetadata.size( ) > 0 ) {
        Types::InputDataPointer initialInputData = new InputData( );

        for( unsigned int environmentalDataFileIndex = 0; environmentalDataFileIndex < inputFileMetadata.size( ); ++environmentalDataFileIndex ) {

            std::string filePath = inputFileMetadata[ environmentalDataFileIndex ][ Constants::eFilePath ];
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
                    bool isDefault = variableName == inputFileMetadata[ environmentalDataFileIndex ][ Constants::eDefaultVariableName ];
                    initialInputData->AddVariableToDatum( inputFileMetadata[ environmentalDataFileIndex ][ Constants::eInternalName ], variableName, variableDimensions, variableSize, variableData, isDefault );
                }

            } catch( netCDF::exceptions::NcException& e ) {
                e.what( );
                Logger::Get( )->LogMessage( "ERROR> File path \"" + filePath + "\" is invalid." );
            }
        }

        DataLayerSet::Get( )->SetDataLayers( initialInputData );
        return true;
    } else {
        return false;
    }
}
