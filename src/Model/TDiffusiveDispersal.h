#ifndef TDIFFUSIVEDISPERSAL_H
#define TDIFFUSIVEDISPERSAL_H
#include <IDispersalImplementation.h>
#include <UtilityFunctions.h>
#include <random>
#include <chrono>
#include <assert.h>
#include <math.h>
/** \file TDiffusiveDispersal.h
 * \brief the TDiffusiveDispersal header file
 */

/** \brief A formulation of the process of dispersal */
class DiffusiveDispersal: public IDispersalImplementation {
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief The time units associated with this implementation of dispersal */
    const string TimeUnitImplementation = "month";
    /** \brief Scalar relating dispersal speed to individual body mass */
    const double DispersalSpeedBodyMassScalar = 0.0278;
    /** \brief Body-mass exponent of the relationship between disperal speed and individual body mass */
    const double DispersalSpeedBodyMassExponent = 0.48;
    /** \brief Scalar to convert from the time step units used by this formulation of dispersal to global model time step units */
    double DeltaT;

    /** \brief Include Utility class */
    UtilityFunctions Utilities;


public:
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief    Constructor for dispersal: assigns all parameter values
     */
    DiffusiveDispersal( ) {

        // Calculate the scalar to convert from the time step units used by this implementation of dispersal to the global model time step units
        DeltaT = Utilities.ConvertTimeUnits( Parameters::Get( )->GetTimeStepUnits( ), TimeUnitImplementation );

        // Set the seed for the random number generator
        if( Parameters::Get( )->GetDrawRandomly( ) ) {
            unsigned seed = std::chrono::system_clock::now( ).time_since_epoch( ).count( );
            RandomNumberGenerator.seed( seed );
        } else {
            RandomNumberGenerator.seed( 14141 );
        }
    }
    //----------------------------------------------------------------------------------------------

    /** \brief    Run diffusive dispersal
    @param cellIndices List of indices of active cells in the model grid 
    @param gridForDispersal The model grid to run dispersal for 
    @param cohortToDisperse The cohort for which to run the dispersal process for 
    @param actingCohortFunctionalGroup The functional group index of the acting cohort 
    @param actingCohortNumber The position of the cohort within the functional group in the array of grid cell cohorts 
    @param currentMonth The current model month */
    void RunDispersal( ModelGrid& gridForDispersal, Cohort& cohortToDisperse, const unsigned& currentMonth ) {
        // Calculate dispersal speed for the cohort         
        double DispersalSpeed = CalculateDispersalSpeed( cohortToDisperse.mIndividualBodyMass );

        CalculateDispersalProbability( gridForDispersal, cohortToDisperse, DispersalSpeed );

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculates the average diffusive dispersal speed of individuals in a cohort given their body mass
    @param bodyMass The current body mass of individuals in the cohort 
    @return The average dispersal speed, in km per month*/
    double CalculateDispersalSpeed( double bodyMass ) {
        return DispersalSpeedBodyMassScalar * pow( bodyMass, DispersalSpeedBodyMassExponent );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculates the probability of diffusive dispersal given average individual dispersal speed
    @param madingleyGrid The model grid 
    @param C a cohort
    @param dispersalSpeed The average speed at which individuals in this cohort move around their environment, in km per month 
     */
    void CalculateDispersalProbability( ModelGrid& madingleyGrid, Cohort& c, double dispersalSpeed ) {
        // Check that the u speed and v speed are not greater than the cell length. If they are, then rescale them; this limits the max velocity
        // so that cohorts cannot be advected more than one grid cell per time step
        double LatCellLength = c.mLocation->CellHeightKm;
        double LonCellLength = c.mLocation->CellWidthKm;

        // Pick a direction at random
        std::uniform_real_distribution<double> randomNumber( 0.0, 1.0 );
        double RandomDirection = randomNumber( RandomNumberGenerator )* 2 * acos( -1. );


        // Calculate the u and v components given the dispersal speed
        double uSpeed = dispersalSpeed * cos( RandomDirection );
        double vSpeed = dispersalSpeed * sin( RandomDirection );
        c.TryLivingAt( newCell( madingleyGrid, uSpeed, vSpeed, LatCellLength, LonCellLength, c.mLocation ) );
    }
    //----------------------------------------------------------------------------------------------
};
#endif
