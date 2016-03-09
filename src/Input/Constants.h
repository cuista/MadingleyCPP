#ifndef CONSTANTS
#define	CONSTANTS

namespace Constants {

    const int cMissingValue = -9999;

    const std::string cLatitudeVariableName = "lat";
    const std::string cLongitudeVariableName = "lon";
    const std::string cTimeVariableName = "time";

    enum eVariableNamesEnum {
        eLandMask,
        eWaterCapacity,
        eDiurnalTemperatureRange,
        eGroundFrostFrequency,
        ePrecipitation,
        eNearSurfaceTemperature,
        eNPP,
        eOceanNPP,
        eSST,
        eUSpeed,
        eVSpeed,
    };

    const std::string cVariableNames[ ] = {
        "realm",
        "awc",
        "dtr",
        "frs",
        "pre",
        "tmp",
        "npp",
        "npp",
        "temp",
        "uo",
        "vo"
    };

    const std::string cInputFileNames[ ] = {
        "realm_classification.nc4",
        "Terrestrial/available_water_capacity.nc4",
        "Terrestrial/diurnal_temperature_range.nc4",
        "Terrestrial/ground_frost_frequency.nc4",
        "Terrestrial/precipitation.nc4",
        "Terrestrial/near-surface_temperature.nc4",
        "Terrestrial/terrestrial_net_primary_productivity.nc4",
        "Marine/Modelled/intpp.nc4",
        "Marine/Modelled/thetao.nc4",
        "Marine/Modelled/uo.nc4",
        "Marine/Modelled/vo.nc4"
    };

    const unsigned int cNumberOfInputFiles = 11;
}

#endif

