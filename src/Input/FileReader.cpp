#include "FileReader.h"

#include "Logger.h"
#include "Convertor.h"
#include "DataGrid.h"
#include "Constants.h"

#include <netcdf>

FileReader::FileReader( ) {
}

FileReader::~FileReader( ) {
    mFilePath.clear( );
}

void FileReader::ReadNetCDFFile( const std::string& filePath, const unsigned int variableIndex ) {

    Logger::Get( )->LogString( "Reading \"" + filePath + "\"..." );

    netCDF::NcFile inputNcFile( filePath, netCDF::NcFile::read ); // Open the file for read access

    switch( variableIndex ) {
        case Constants::eLandMask:
        {
            Logger::Get( )->LogString( "Extracting longitude and latitude data from \"" + Constants::cInputFileNames[ variableIndex ] + "\"..." );

            netCDF::NcVar longitudeNcVar = inputNcFile.getVar( Constants::cLongitudeVariableName );
            netCDF::NcVar latitudeNcVar = inputNcFile.getVar( Constants::cLatitudeVariableName );
            netCDF::NcVar isOceanNcVar = inputNcFile.getVar( Constants::cVariableNames[ variableIndex ] );

            unsigned int lengthLongitudeArray = longitudeNcVar.getDim( 0 ).getSize( );
            unsigned int lengthLatitudeArray = latitudeNcVar.getDim( 0 ).getSize( );

            ////////////////////////////////////////////////////////////////////
            double* longitudeArray = new double[ lengthLongitudeArray ];
            double* latitudeArray = new double[ lengthLatitudeArray ];
            int* isOceanArray = new int[ lengthLongitudeArray * lengthLatitudeArray ];
            ////////////////////////////////////////////////////////////////////

            longitudeNcVar.getVar( longitudeArray );
            latitudeNcVar.getVar( latitudeArray );
            isOceanNcVar.getVar( isOceanArray );

            DataGrid::Get( )->InitialiseGridCells( isOceanArray, longitudeArray, latitudeArray, lengthLongitudeArray, lengthLatitudeArray );

            ////////////////////////////////////////////////////////////////////
            //delete [] isOceanArray;
            //delete [] longitudeArray;
            //delete [] latitudeArray;
            ////////////////////////////////////////////////////////////////////

            break;
        }
        case Constants::eWaterCapacity:
        {
            netCDF::NcVar dataNcVar = inputNcFile.getVar( Constants::cVariableNames[ variableIndex ] );

            ////////////////////////////////////////////////////////////////////
            double* dataArray = new double[ DataGrid::Get( )->GetLengthLongitudeVector( ) * DataGrid::Get( )->GetLengthLatitudeVector( ) ];
            ////////////////////////////////////////////////////////////////////

            dataNcVar.getVar( dataArray );
            DataGrid::Get( )->AddDataWithoutTime( dataArray );

            ////////////////////////////////////////////////////////////////////
            //delete [] dataArray;
            ////////////////////////////////////////////////////////////////////

            break;
        }
        case Constants::eDiurnalTemperatureRange:
        {
            Logger::Get( )->LogString( "Extracting time data from \"" + Constants::cInputFileNames[ variableIndex ] + "\"..." );

            netCDF::NcVar timeNcVar = inputNcFile.getVar( Constants::cTimeVariableName );
            netCDF::NcVar dataNcVar = inputNcFile.getVar( Constants::cVariableNames[ variableIndex ] );

            unsigned int lengthTimeArray = dataNcVar.getDim( 0 ).getSize( );

            ////////////////////////////////////////////////////////////////////
            unsigned int* timeArray = new unsigned int[ lengthTimeArray ];
            double* dataArray = new double[ DataGrid::Get( )->GetLengthLongitudeVector( ) * DataGrid::Get( )->GetLengthLatitudeVector( ) * lengthTimeArray ];
            ////////////////////////////////////////////////////////////////////

            timeNcVar.getVar( timeArray );
            dataNcVar.getVar( dataArray );
            DataGrid::Get( )->InitialiseGridCellVectors( timeArray, lengthTimeArray );
            DataGrid::Get( )->AddDataWithTime( dataArray, variableIndex );

            ////////////////////////////////////////////////////////////////////
            //delete [] timeArray;
            //delete [] dataArray;
            ////////////////////////////////////////////////////////////////////

            break;
        }

        default:
        {
            netCDF::NcVar dataNcVar = inputNcFile.getVar( Constants::cVariableNames[ variableIndex ] );

            ////////////////////////////////////////////////////////////////////
            double* dataArray = new double[ DataGrid::Get( )->GetLengthLongitudeVector( ) * DataGrid::Get( )->GetLengthLatitudeVector( ) * DataGrid::Get( )->GetLengthTimeVector( ) ];
            ////////////////////////////////////////////////////////////////////

            dataNcVar.getVar( dataArray );
            DataGrid::Get( )->AddDataWithTime( dataArray, variableIndex );

            ////////////////////////////////////////////////////////////////////
            //delete [] dataArray;
            ////////////////////////////////////////////////////////////////////

            break;
        }
    }
}
