#include "FileWriter.h"

#include "Logger.h"
#include "Constants.h"
#include "DateTime.h"
#include "Convertor.h"
#include "Parameters.h"
#include "DataRecorder.h"
#include "BasicDatum.h"
#include "GridDatum.h"

#include <netcdf>
#include <sys/stat.h> // For mkdir
#include <fstream>      // For std::ifstream and std::ofstream

FileWriter::~FileWriter( ) {
}

FileWriter::FileWriter( ) {
}

bool FileWriter::WriteFiles( ) {

    InitialiseOutputDirectory( );
    WriteMetadataFiles( );

    return WriteNetCDFFiles( );
}

void FileWriter::InitialiseOutputDirectory( ) {

    mOutputDirectory = Constants::cOutputBaseDirectory;
    mkdir( mOutputDirectory.c_str( ), Constants::cOutputFolderPermissions );
    mOutputDirectory.append( DateTime::Get( )->GetFormattedDateAndTime( Constants::cDataSetNameFormat, false ) );
    mkdir( mOutputDirectory.c_str( ), Constants::cOutputFolderPermissions );
    mOutputDirectory.append( Convertor::Get( )->ToString( Constants::cFolderDelimiter ) );
}

void FileWriter::WriteMetadataFiles( ) {

    Types::StringVector metadataFilePaths = DataRecorder::Get( )->GetMetadataFilePaths( );

    for( unsigned stringIndex = 0; stringIndex < metadataFilePaths.size( ); ++stringIndex ) {

        std::ifstream sourceFileStream( metadataFilePaths[ stringIndex ].c_str( ), std::ios::in );

        std::string outputFilePath = mOutputDirectory;
        Types::StringVector inputFilePathComponents = Convertor::Get( )->StringToWords( metadataFilePaths[ stringIndex ], Constants::cFolderDelimiter );
        outputFilePath.append( inputFilePathComponents[ inputFilePathComponents.size( ) - 1 ] );

        std::ofstream destinationFileStream( outputFilePath.c_str( ), std::ios::out );

        destinationFileStream << sourceFileStream.rdbuf( );
    }
}

bool FileWriter::WriteNetCDFFiles( ) const {
    bool completedSuccessfully = WriteBasicOutputs( );

    if( completedSuccessfully == true ) {
        completedSuccessfully = WriteGridOutputs( );
    }

    return completedSuccessfully;
}

bool FileWriter::WriteBasicOutputs( ) const {
    bool completedSuccessfully = false;

    std::string filePath = mOutputDirectory + Constants::cBasicOutputsFileName;

    try {
        netCDF::NcFile basicOutputsNcFile( filePath, netCDF::NcFile::replace ); // Creates file
        netCDF::NcDim timeNcDim = basicOutputsNcFile.addDim( Constants::cTimeVariableNames[ 0 ], Parameters::Get( )->GetLengthOfSimulationInTimeSteps( ) ); // Creates dimension
        netCDF::NcVar timeNcVar = basicOutputsNcFile.addVar( Constants::cTimeVariableNames[ 0 ], netCDF::ncFloat, timeNcDim ); // Creates variable
        timeNcVar.putVar( Parameters::Get( )->GetTimeStepArray( ) );
        timeNcVar.putAtt( Constants::cUnitsString, Constants::cTimeVariableUnits );

        std::vector<netCDF::NcDim> ncDims;
        ncDims.push_back( timeNcDim );

        Types::BasicDatumMap basicDatumMap = DataRecorder::Get( )->GetBasicDatumMap( );

        for( Types::BasicDatumMap::iterator iter = basicDatumMap.begin( ); iter != basicDatumMap.end( ); ++iter ) {
            netCDF::NcVar basicDatumNcVar = basicOutputsNcFile.addVar( iter->second->GetName( ), netCDF::ncFloat, ncDims );
            basicDatumNcVar.putAtt( Constants::cUnitsString, iter->second->GetUnits( ) );
            basicDatumNcVar.putVar( iter->second->GetData( ) );
        }

        completedSuccessfully = true;

    } catch( netCDF::exceptions::NcException& e ) {
        e.what( );
        Logger::Get( )->LogMessage( "ERROR> File path \"" + filePath + "\" is invalid." );
    }

    return completedSuccessfully;
}

bool FileWriter::WriteGridOutputs( ) const {
    bool completedSuccessfully = false;

    std::string filePath = mOutputDirectory + Constants::cGridOutputsFileName;

    try {
        netCDF::NcFile gridOutputsNcFile( filePath, netCDF::NcFile::replace ); // Creates file

        netCDF::NcDim longitudeDim = gridOutputsNcFile.addDim( Constants::cLongitudeVariableNames[ 0 ], Parameters::Get( )->GetLengthUserLongitudeArray( ) );
        netCDF::NcVar longitudeNcVar = gridOutputsNcFile.addVar( Constants::cLongitudeVariableNames[ 0 ], netCDF::ncFloat, longitudeDim );
        longitudeNcVar.putVar( Parameters::Get( )->GetUserLongitudeArray( ) );
        longitudeNcVar.putAtt( Constants::cUnitsString, Constants::cLongitudeVariableUnits );

        netCDF::NcDim latitudeDim = gridOutputsNcFile.addDim( Constants::cLatitudeVariableNames[ 0 ], Parameters::Get( )->GetLengthUserLatitudeArray( ) );
        netCDF::NcVar latitudeNcVar = gridOutputsNcFile.addVar( Constants::cLatitudeVariableNames[ 0 ], netCDF::ncFloat, latitudeDim );
        latitudeNcVar.putVar( Parameters::Get( )->GetUserLatitudeArray( ) );
        latitudeNcVar.putAtt( Constants::cUnitsString, Constants::cLatitudeVariableUnits );

        netCDF::NcDim timeNcDim = gridOutputsNcFile.addDim( Constants::cTimeVariableNames[ 0 ], Parameters::Get( )->GetLengthOfSimulationInTimeSteps( ) );
        netCDF::NcVar timeNcVar = gridOutputsNcFile.addVar( Constants::cTimeVariableNames[ 0 ], netCDF::ncFloat, timeNcDim );
        timeNcVar.putVar( Parameters::Get( )->GetTimeStepArray( ) );
        timeNcVar.putAtt( Constants::cUnitsString, Constants::cTimeVariableUnits );

        std::vector<netCDF::NcDim> ncDims;
        ncDims.push_back( timeNcDim );
        ncDims.push_back( latitudeDim );
        ncDims.push_back( longitudeDim );
        
        Types::GridDatumMap gridDatumMap = DataRecorder::Get( )->GetGridDatumMap( );

        for( Types::GridDatumMap::iterator iter = gridDatumMap.begin( ); iter != gridDatumMap.end( ); ++iter ) {
            netCDF::NcVar gridDatumNcVar = gridOutputsNcFile.addVar( iter->second->GetName( ), netCDF::ncFloat, ncDims );
            gridDatumNcVar.putAtt( Constants::cUnitsString, iter->second->GetUnits( ) );
            gridDatumNcVar.putVar( iter->second->GetData( ) );
        }

        completedSuccessfully = true;

    } catch( netCDF::exceptions::NcException& e ) {
        e.what( );
        Logger::Get( )->LogMessage( "ERROR> File path \"" + filePath + "\" is invalid." );
    }

    return completedSuccessfully;
}
