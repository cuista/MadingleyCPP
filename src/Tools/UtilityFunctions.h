#ifndef UTILITYFUNCTIONS_H
#define UTILITYFUNCTIONS_H
#include <math.h>
#include <algorithm>
#include <vector>
#include <map>
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock
#include <assert.h>
#include <iostream>

#include "Convertor.h"
#include "Parameters.h"
/** \brief Generic functions */

using namespace std;

class UtilityFunctions {
public:
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief If longitudinal cell coordinates run from 0 to 360, the convert to -180 to 180 values
    @param lons The longitudinal coorindates of the cells in the model grid
     */
    void ConvertToM180To180( vector<double>& lons ) {
        // Loop over longitudinal coordinates of the model grid cells
        for( unsigned jj = 0; jj < lons.size( ); jj++ ) {
            // If longitudinal coorindates exceed 180, then subtrarct 360 to correct the coorindates
            if( lons[jj] >= 180.0 ) {
                lons[jj] -= 360.0;
            }
        }
        // Re-sort the longitudinal coordinates
        sort( lons.begin( ), lons.end( ) );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Generate a random order in which cohorts will be subjected to ecological processes
    @param cohortNumber The number of cohorts in the current grid cell
    @return A vector of randomly ordered integers corresponding to the cohorts in the grid cell - will change with every run
     */
    vector<unsigned> RandomlyOrderedCohorts( unsigned cohortNumber ) {
        //A vector to hold indices of cohorts in order
        vector<unsigned> RandomOrderCohorts( cohortNumber );
        for( unsigned i = 0; i < cohortNumber; i++ ) RandomOrderCohorts[i] = i;
        // Return the randomly ordered vector of cohort indices - randomly off system clock...c++11 style
        unsigned seed = std::chrono::system_clock::now( ).time_since_epoch( ).count( );
        shuffle( RandomOrderCohorts.begin( ), RandomOrderCohorts.end( ), std::default_random_engine( seed ) );
        return RandomOrderCohorts;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Generate a non-random order in which cohorts will be subjected to ecological processes
    @param cohortNumber The number of cohorts in the current grid cell
    @param currentTimeStep The current time step of the model
    @return A vector of non-randomly ordered integers corresponding to the cohorts in the grid cell -will be systematic in some way...
     */
    vector<unsigned> NonRandomlyOrderedCohorts( unsigned cohortNumber, unsigned currentTimeStep ) {

        //A vector to hold indices of cohorts in order
        vector<unsigned> RandomOrderCohorts( cohortNumber );
        for( unsigned i = 0; i < cohortNumber; i++ ) RandomOrderCohorts[i] = i;
        //Shuffle ordered list for random number generation, using the current time step as a deterministic seed to 
        // ensure a repeatable order of cohorts
        shuffle( RandomOrderCohorts.begin( ), RandomOrderCohorts.end( ), std::default_random_engine( currentTimeStep ) );
        return RandomOrderCohorts;
    }
    //----------------------------------------------------------------------------------------------

    unsigned GetCurrentMonth( unsigned currentTimestep ) {
        unsigned Month;

        double DaysInYear = 360.0;
        double MonthsInYear = 12.0;
        double DaysInWeek = 7.0;

        //C++ can only use intergers or enums in a switch, so make a map to convert.
        map<string, int> units;

        units["year" ] = 0;
        units["month"] = 1;
        units["week" ] = 2;
        units["day" ] = 3;

        //lowercase the string - a bit clunky...but then C++ strings are a bit
        switch( units[ Parameters::Get( )->GetTimeStepUnits( ) ] ) {
            case 0://year
                Month = 0;
                break;
            case 1://month
                Month = currentTimestep % 12;
                break;
            case 2://week
                Month = ( unsigned )floor( currentTimestep / ( ( DaysInYear / MonthsInYear ) / DaysInWeek ) ) % 12;
                break;
            case 3://day
                Month = ( unsigned )floor( currentTimestep / ( DaysInYear / MonthsInYear ) ) % 12;
                break;
            default://should the program bomb out at this point?
                cout << "Requested model time units not currently supported" << endl;
                Month = 100;
                break;

        }

        return Month;

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculates factors to convert between different time units
    @param fromUnit Time unit to convert from
    @param toUnit Time unit to convert to
    @return Factor to convert between time units
     */
    double ConvertTimeUnits( string fromUnit, string toUnit ) {
        //transform( fromUnit.begin( ), fromUnit.end( ), fromUnit.begin( ), ::tolower );
        toUnit = Convertor::Get( )->ToLowercase( toUnit );
        // Variable to hold the conversion factor
        double ConversionValue;
        double DaysInYear = 360.0;
        double MonthsInYear = 12.0;
        double DaysInWeek = 7.0;

        //C++ can only use intergers or enums in a switch, so make a map to convert.
        map<string, int> units;

        units["year"] = 0;
        units["month"] = 1;
        units["bimonth"] = 2;
        units["week"] = 3;
        units["day"] = 4;
        units["second"] = 5;

        // Determine which combination of time units is being requested and return the appropriate scaling factor
        switch( units[fromUnit] ) {
            case 0:// "year":
                switch( units[toUnit] ) {
                    case 0:// "year":
                        ConversionValue = 1.0;
                        break;
                    case 1://"month":
                        ConversionValue = MonthsInYear;
                        break;
                    case 2://"bimonth":
                        ConversionValue = MonthsInYear * 2;
                        break;
                    case 3://"week":
                        ConversionValue = DaysInYear / DaysInWeek;
                        break;
                    case 4://"day":
                        ConversionValue = DaysInYear;
                        break;
                    default:
                        cout << "Requested combination of time units not currently supported" << endl;
                        ConversionValue = 0;
                        break;
                }
                break;
            case 1://"month":
                switch( units[toUnit] ) {
                    case 0:// "year":
                        ConversionValue = 1.0 / MonthsInYear;
                        break;
                    case 1://"month":
                        ConversionValue = 1.0;
                        break;
                    case 2://"bimonth":
                        ConversionValue = 2.0;
                        break;
                    case 3://"week":
                        ConversionValue = ( DaysInYear / MonthsInYear ) / DaysInWeek;
                        break;
                    case 4://"day":
                        ConversionValue = ( DaysInYear / MonthsInYear );
                        break;
                    case 5://"second":
                        ConversionValue = ( DaysInYear / MonthsInYear ) * 24.0 * 60.0 * 60.0;
                        break;
                    default:
                        cout << "Requested combination of time units not currently supported" << endl;
                        ConversionValue = 0;
                        break;
                }
                break;
            case 2://"bimonth":
                switch( units[toUnit] ) {
                    case 0:// "year":
                        ConversionValue = 1.0 / ( MonthsInYear * 2 );
                        break;
                    case 1://"month":
                        ConversionValue = 1 / 2.0;
                        break;
                    case 2://"bimonth":
                        ConversionValue = 1.0;
                        break;
                    case 3://"week":
                        ConversionValue = ( DaysInYear / ( MonthsInYear * 2 ) ) / DaysInWeek;
                        break;
                    case 4://"day":
                        ConversionValue = ( DaysInYear / ( MonthsInYear * 2 ) );
                        break;
                    case 5://"second":
                        ConversionValue = ( DaysInYear / ( MonthsInYear * 2 ) ) * 24.0 * 60.0 * 60.0;
                        break;
                    default:
                        cout << "Requested combination of time units not currently supported" << endl;
                        ConversionValue = 0;
                        break;
                }
                break;

            case 3: //"week":
                switch( units[toUnit] ) {
                    case 0:// "year":
                        ConversionValue = DaysInWeek / DaysInYear;
                        break;
                    case 1://"month":
                        ConversionValue = DaysInWeek / ( DaysInYear / MonthsInYear );
                        break;
                    case 2://"bimonth":
                        ConversionValue = DaysInWeek / ( DaysInYear / ( MonthsInYear * 2 ) );
                        break;
                    case 3://"week":
                        ConversionValue = 1.0;
                        break;
                    case 4://"day":
                        ConversionValue = DaysInWeek;
                        break;
                    case 5://"second":
                        ConversionValue = DaysInWeek * 24.0 * 60.0 * 60.0;
                        break;
                    default:
                        cout << "Requested combination of time units not currently supported" << endl;
                        ConversionValue = 0;
                        break;
                }
                break;
            case 4: //"day":
                switch( units[toUnit] ) {
                    case 0:// "year":
                        ConversionValue = 1.0 / DaysInYear;
                        break;
                    case 1://"month":
                        ConversionValue = 1.0 / ( DaysInYear / MonthsInYear );
                        break;
                    case 2://"bimonth":
                        ConversionValue = 1.0 / ( DaysInYear / ( MonthsInYear * 2 ) );
                        break;
                    case 3://"week":
                        ConversionValue = 1.0 / DaysInWeek;
                        break;
                    case 4://"day":
                        ConversionValue = 1.0;
                        break;
                    default:
                        cout << "Requested combination of time units not currently supported" << endl;
                        ConversionValue = 0;
                        break;
                }
                break;
            default:
                cout << "Requested combination of time units not currently supported" << endl;
                ConversionValue = 0;
                break;
        }

        // Return the conversion factor
        return ConversionValue;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief For a given cohort index, return a vector pair of values corresponding to the cohort's location in the jagged array of grid cell cohorts
    @param valueToFind The index of the cohort (values range between zero and the number of cohorts in the jagged arrray)
    @param arrayToSearch The jaggged array of cohorts, where rows correspond to functional groups, and columns to cohorts within functional groups
    @param totalNumberOfCohorts The total number of cohorts in the grid cell
    @return The position of the specified cohort in the jagged array of grid cell cohorts, where the first value is the row index (functional group) and the second value is the column index (position within functional group)</returns>
     */
    vector<int> FindJaggedArrayIndex( unsigned valueToFind, vector< vector < unsigned> > arrayToSearch, unsigned totalNumberOfCohorts ) {
        // Create a vector to hold the location of the cohort in the jagged array
        vector<int> ValueLocation( 2 );

        // Check to make sure that specified cohort index is not greater than the total number of cohorts
        assert( valueToFind < totalNumberOfCohorts && "Value searched for in jagged array is bigger than the biggest value in the jagged array" );

        // Variables to hold the row and colum indices of the cohort in the jaggged array
        int RowIndex = 0;
        int ColumnIndex = 0;

        // Loop over rows (functional groups) and locate the one in which the specified cohort is located
        while( arrayToSearch[RowIndex].size( ) == 0 || valueToFind > arrayToSearch[RowIndex][arrayToSearch[RowIndex].size( ) - 1] ) {
            RowIndex++;
        }

        // Add the located row to the vector of values to return
        ValueLocation[0] = RowIndex;

        // Loop over columns (cohorts within the functional group) and locate the one in which the specified cohort is located
        while( valueToFind != arrayToSearch[RowIndex][ColumnIndex] ) {
            ColumnIndex++;
        }

        // Add the located column to the vector of values to return
        ValueLocation[1] = ColumnIndex;

        // Return the vector of two values correpsonding to the located position in the jagged array of grid cell cohorts
        return ValueLocation;

    }
    //----------------------------------------------------------------------------------------------

    /** \briefConverts values per square km to per square degree, given cell latitude
    @param valueToConvert The value per square km
    @param latitude The latitude of the grid cell
    @return The specified value converted to per square degree 
     */
    double ConvertSqMToSqDegrees( double valueToConvert, double latitude ) {
        // Convert the value to per square degree using the cosine of latitude and assuming cell dimensions of 110km by 110km at the Equator
        return valueToConvert * 110000.0 * 110000.0 * cos( DegreesToRadians( latitude ) );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculates the probability of a particular value under a log-normal distribution with specified mean and standard deviation
    @param xValue The value to return the probability of under the log-normal distribtuion, in identity space
    @param meanIdentity The mean of the log-normal distribution, in identity space
    @param standardDeviation The standard deviation of the log-normal distribution, in log space
    @return The probability of the specified value under the specified log-normal distribution
     */
    double LogNormalPDF( double xValue, double meanIdentity, double standardDeviation ) {
        const double PI = acos( -1. );
        // Calculate the mean of the log-normal distribution in log space
        double meanLog = log( meanIdentity );
        // Calculate and return the probability of the specified value under the specified log-normal distribution
        return (1 / sqrt( 2 * PI * pow( standardDeviation, 2 ) ) )*exp( -( pow( log( xValue ) - meanLog, 2 ) / ( 2 * pow( standardDeviation, 2 ) ) ) );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculates the probability of a particular value under a normal distribution with specified mean and standard deviation
    @param xValue The value to return the probability of under the normal distribtuion
    @param meanValue The mean of the normal distribution
    @param standardDeviation The standard deviation of the normal distribution
    @return The probability of the specified value under the specified normal distribution
     */
    double NormalPDF( double xValue, double meanValue, double standardDeviation ) {
        const double PI = acos( -1. );
        // Calculate and return the probability of the specified value under the specified normal distribution
        return (1 / sqrt( 2 * PI * pow( standardDeviation, 2 ) ) ) * exp( -( pow( xValue - meanValue, 2 ) / ( 2 * pow( standardDeviation, 2 ) ) ) );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the area of a grid cell in square km, given its dimensions and geographical position
    @param latitude The latitude of the bottom-left corner of the grid cell
    @param lonCellSize The longitudinal dimension of the grid cell
    @param latCellSize The latitudinal dimension of the grid cell
    @return The area in square km of the grid cell
     */
    double CalculateGridCellArea( double latitude, double cellSize ) {
        const double PI = acos( -1. );
        // Convert from degrees to radians
        double latitudeRad = DegreesToRadians( latitude );

        // Equatorial radius in metres
        double EquatorialRadius = 6378137;

        // Polar radius in metres
        double PolarRadius = 6356752.3142;

        // Angular eccentricity
        double AngularEccentricity = acos( DegreesToRadians( PolarRadius / EquatorialRadius ) );

        // First eccentricity squared
        //double ESquared = pow(sin(DegreesToRadians(AngularEccentricity)), 2);

        // Flattening
        //double Flattening = 1 - cos(DegreesToRadians(AngularEccentricity));

        // Temporary value to save computations
        double TempVal = pow( ( EquatorialRadius * cos( latitudeRad ) ), 2 ) + pow( ( PolarRadius * sin( latitudeRad ) ), 2 );

        // Meridional radius of curvature
        double MPhi = pow( EquatorialRadius * PolarRadius, 2 ) / pow( TempVal, 1.5 );

        // Normal radius of curvature
        double NPhi = pow( EquatorialRadius, 2 ) / sqrt( TempVal );

        // Length of latitude (km)
        double LatitudeLength = PI / 180 * MPhi / 1000;

        // Length of longitude (km)
        double LongitudeLength = PI / 180 * cos( latitudeRad ) * NPhi / 1000;

        // Return the cell area in km^2
        return LatitudeLength * cellSize * LongitudeLength * cellSize;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the length of a degree of latitude at a particular latitude /
    @param latitude The latitude of the bottom-left corner of the grid cell
    @return The length of a degree of latitude in kilometres*/
    double CalculateLengthOfDegreeLatitude( float latitude ) {
        const double PI = acos( -1. );
        // Convert from degrees to radians
        double latitudeRad = DegreesToRadians( latitude );

        // Equatorial radius in metres
        double EquatorialRadius = 6378137;

        // Polar radius in metres
        double PolarRadius = 6356752.3142;

        // Angular eccentricity
        //double AngularEccentricity = acos(DegreesToRadians(PolarRadius / EquatorialRadius));

        // First eccentricity squared
        //double ESquared = pow(sin(DegreesToRadians(AngularEccentricity)), 2);

        // Flattening
        //double Flattening = 1 - cos(DegreesToRadians(AngularEccentricity));

        // Temporary value to save computations
        double TempVal = pow( ( EquatorialRadius * cos( latitudeRad ) ), 2 ) + pow( ( PolarRadius * sin( latitudeRad ) ), 2 );

        // Meridional radius of curvature
        double MPhi = pow( EquatorialRadius * PolarRadius, 2 ) / pow( TempVal, 1.5 );

        // Length of latitude (km)
        return PI / 180 * MPhi / 1000;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the length of a degree of longitude at a particular latitude
    @param latitude The latitude of the bottom-left corner of the grid cell
    @return The length of a degree of longitude in kilometres
     */
    double CalculateLengthOfDegreeLongitude( float latitude ) {
        const double PI = acos( -1. );
        // Convert from degrees to radians
        double latitudeRad = DegreesToRadians( latitude );

        // Equatorial radius in metres
        double EquatorialRadius = 6378137;

        // Polar radius in metres
        double PolarRadius = 6356752.3142;

        // Temporary value to save computations
        double TempVal = pow( ( EquatorialRadius * cos( latitudeRad ) ), 2 ) + pow( ( PolarRadius * sin( latitudeRad ) ), 2 );

        // Normal radius of curvature
        double NPhi = pow( EquatorialRadius, 2 ) / sqrt( TempVal );

        // Length of longitude (km)
        return PI / 180 * cos( latitudeRad ) * NPhi / 1000;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Convert from degrees to radians
    @param degrees The value in degrees to convert
    @return The value converted to radians</returns>
     */
    double DegreesToRadians( double degrees ) {
        const double PI = acos( -1. );
        return (degrees * PI / 180.0 );
    }
    //----------------------------------------------------------------------------------------------
};
#endif