#ifndef CONSTANTS
#define	CONSTANTS

namespace Constants {

    const int cMissingValue = -9999;

    const std::string cLatitudeVariableName = "lat";
    const std::string cLongitudeVariableName = "lon";
    const std::string cTimeVariableName = "time";

    //const std::string cVariableNames[ ] = { "mask" };
    //const std::string cInputFileNames[ ] = { "land_mask.nc4", "diurnal_temperature_range.nc4", "ground_frost_frequency.nc4", "precipitation.nc4", "near-surface_temperature.nc4" };

    //const std::string cVariableNames[ ] = { "dtr", "frs", "pre", "tmp" };
    //const std::string cInputFileNames[ ] = { "diurnal_temperature_range.nc4", "ground_frost_frequency.nc4", "precipitation.nc4", "near-surface_temperature.nc4" };


    enum eVariableNamesEnum {
        eLandMask,
        eDiurnalTemperatureRange,
        eGroundFrostFrequency,
        ePrecipitation,
        eNearSurfaceTemperature,
        eUSpeed,
        eVSpeed,
        eWaterCapacity,
        eSST,
        eNPP,
        eOceanNPP
    };

    /*
    const std::string cVariableNames[ ] = { 
	"mask",
	"dtr", 
	"frs", 
	"pre", 
	"tmp", 
	"vVel",
	"uVel",
	"AWC",
	"SST",
	"NPP",
	"OceanNPP"
	};
    */
    
    const std::string cVariableNames[ ] = { 
	"realm",
	"dtr", 
	"frs", 
	"pre", 
	"tmp", 
	"v",
	"u",
	"awc",
	"temp",
	"npp",
	"npp"
	};
     
    const std::string cInputFileNames[ ] = { 
	"realm_classification.nc4", 
	"diurnal_temperature_range.nc4", 
	"ground_frost_frequency.nc4", 
	"precipitation.nc4", 
	"near-surface_temperature.nc4",
	"averaged_v_50y_top100m_monthly.nc4",
	"averaged_u_50y_top100m_monthly.nc4",
	"AvailableWaterCapacity.nc4",
	"averaged_SST_50y_top100m_monthly.nc4",
	"NPP.nc4",
	"OceanNPP.nc4"
	};

    const unsigned int cNumberOfInputFiles = 11;
}

#endif

