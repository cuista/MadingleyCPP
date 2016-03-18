#ifndef CONSTANTS
#define	CONSTANTS

namespace Constants {
    
    enum eSimulationControlParametersMetadata {
        eInputParameter,
        eValue
    };
    
    enum eSimulationControlParameters {
        eLengthOfSimulationInYears,
        eMinimumLongitude,
        eMaximumLongitude,
        eMinimumLatitude,
        eMaximumLatitude,
        eGridCellSize
    };
    
    enum eOutputControlParametersMetadata {
        eOutputDatumName,
        eUnits,
        eType
    };
    
    enum eEnvironmentalDataLayersMetadata {
        eInternalName,
        eFilePath,
        eDefaultVariableName
    };
    
    enum eDataLayerTypes {
        eDataLayer2D,            // Spatial: land-sea mask
        eDataLayer2DwithTime,    // Two-dimensional with time, e.g. SST.
        eDataLayer3D,            // Three-dimensional, e.g. ocean temperature.
        eDataLayer3DwithTime     // Three-dimensional with time.
    };
    
    enum eVariableTypes {
        eLongitude,
        eLatitude,
        eTime,
        eDepth,
        eOther
    };
    
    const std::string cLongitudeVariableNames[ ] = { "lon", "long", "longitude", "x" };
    const std::string cLatitudeVariableNames[ ] = { "lat", "lats", "latitude", "y" };
    const std::string cDepthVariableNames[ ] = { "dep", "depth", "z" };
    const std::string cTimeVariableNames[ ] = { "time", "month", "t" };
    
    const std::string cBasicDatumTypeName = "basic";
    const std::string cGridDatumTypeName = "grid";
    
    const std::string cConfigurationDirectory = "/home/philju/Development/C++/In Progress/MadingleyCPP/input/Model setup/";
    const std::string cEnvironmentalDataLayersFileName = "EnvironmentalDataLayers.csv";
    const std::string cOutputControlParametersFileName = "OutputControlParameters.csv";
    const std::string cSimulationControlParametersFileName = "SimulationControlParameters.csv";
    
    const std::string cBasicOutputsFileName = "BasicOutputs.nc";
    const std::string cGridOutputsFileName = "GridOutputs.nc";
    const std::string cOutputBaseDirectory = "/home/philju/Development/C++/In Progress/MadingleyCPP/output/";
    const std::string cDataSetNameFormat = "%Y-%m-%d_%H-%M-%S";
    const std::string cUnitsString = "units";
    const std::string cLongitudeVariableUnits = "degrees east";
    const std::string cLatitudeVariableUnits = "degrees north";
    const std::string cTimeVariableUnits = "months";

    const int cMissingValue = -9999;
    const int cOutputFolderPermissions = 0775;
    const int cDateTimeBufferSize = 25;
    
    const char cDataDelimiterValue = ',';
    const char cTextFileCommentCharacter = '#';
    const char cFolderDelimiter = '/';
}

#endif