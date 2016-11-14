#ifndef TDIFFUSIVEDISPERSAL_H
#define TDIFFUSIVEDISPERSAL_H

#include "DispersalImplementation.h"
#include "UtilityFunctions.h"
/** \file TDiffusiveDispersal.h
 * \brief the TDiffusiveDispersal header file
 */

/** \brief A formulation of the process of dispersal */
class DiffusiveDispersal : public DispersalImplementation {
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief The time units associated with this implementation of dispersal */
    const std::string mTimeUnitImplementation = "month";
    /** \brief Scalar relating dispersal speed to individual body mass */
    const double mDispersalSpeedBodyMassScalar = 0.0278;
    /** \brief Body-mass exponent of the relationship between disperal speed and individual body mass */
    const double mDispersalSpeedBodyMassExponent = 0.48;
    /** \brief Scalar to convert from the time step units used by this formulation of dispersal to global model time step units */
    double mDeltaT;

    /** \brief Include Utility class */
    UtilityFunctions mUtilities;


public:
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief    Constructor for dispersal: assigns all parameter values
     */
    DiffusiveDispersal( ) {

        // Calculate the scalar to convert from the time step units used by this implementation of dispersal to the global model time step units
        mDeltaT = mUtilities.ConvertTimeUnits( Parameters::Get( )->GetTimeStepUnits( ), mTimeUnitImplementation );

        // Set the seed for the random number generator
        unsigned seed = 14141;
        if( Parameters::Get( )->GetDrawRandomly( ) == true ) {
            seed = std::chrono::system_clock::now( ).time_since_epoch( ).count( );
        }
        mRandomNumber1.SetSeed( seed );
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
        double dispersalSpeed = CalculateDispersalSpeed( cohortToDisperse.mIndividualBodyMass );

        CalculateDispersalProbability( gridForDispersal, cohortToDisperse, dispersalSpeed );

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculates the average diffusive dispersal speed of individuals in a cohort given their body mass
    @param bodyMass The current body mass of individuals in the cohort 
    @return The average dispersal speed, in km per month*/
    double CalculateDispersalSpeed( double bodyMass ) {
        return mDispersalSpeedBodyMassScalar * pow( bodyMass, mDispersalSpeedBodyMassExponent );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculates the probability of diffusive dispersal given average individual dispersal speed
    @param madingleyGrid The model grid 
    @param C a cohort
    @param dispersalSpeed The average speed at which individuals in this cohort move around their environment, in km per month 
     */
    void CalculateDispersalProbability( ModelGrid& madingleyGrid, Cohort& cohort, double dispersalSpeed ) {
        // Check that the u speed and v speed are not greater than the cell length. If they are, then rescale them; this limits the max velocity
        // so that cohorts cannot be advected more than one grid cell per time step
        double latCellLength = cohort.mCurrentCell->GetCellHeight( );
        double lonCellLength = cohort.mCurrentCell->GetCellWidth( );

        // Pick a direction at random
        double randomDirection = mRandomNumber1.GetUniform( )* 2 * acos( -1. );


        // Calculate the u and v components given the dispersal speed
        double uSpeed = dispersalSpeed * cos( randomDirection );
        double vSpeed = dispersalSpeed * sin( randomDirection );
        newCell( madingleyGrid, uSpeed, vSpeed, lonCellLength, latCellLength, cohort );
    }
    //----------------------------------------------------------------------------------------------
};
#endif
