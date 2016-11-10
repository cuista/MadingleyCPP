#ifndef TRESPONSIVEDISPERSAL_H
#define TRESPONSIVEDISPERSAL_H

#include "IDispersalImplementation.h"
#include "UtilityFunctions.h"
/** \file TResponsiveDispersal.h
 * \brief the TResponsiveDispersal header file
 */

/** \brief A formulation of the process of responsive dispersal */
class ResponsiveDispersal : public IDispersalImplementation {
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief The time units associated with this implementation of dispersal */
    const std::string mTimeUnitImplementation = "month";

    /** \brief Density threshold below which adult individuals may move to look for other adults of the same cohort

    \remarks The density scales with cohort weight via: Min Density = DensityThresholdScaling / MatureMass (g) */
    const double mDensityThresholdScaling = 50000;
    /** \brief Scalar relating dispersal speed to individual body mass */
    const double mDispersalSpeedBodyMassScalar = 0.0278;
    /** \brief Body-mass exponent of the relationship between disperal speed and individual body mass */
    const double mDispersalSpeedBodyMassExponent = 0.48;

    /** \brief The proportion of body mass loss at which the cohort will try to disperse every time during a time step */
    const double mStarvationDispersalBodyMassThreshold = 0.8;

    /** \brief Scalar to convert from the time step units used by this formulation of dispersal to global model time step units */
    double mDeltaT;

public:
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief Assigns all parameter values for repsonsive dispersal */
    ResponsiveDispersal( ) {

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

    /** \brief Run responsive dispersal
    @param cellIndices The longitudinal and latitudinal indices of the current grid cell 
    @param gridForDispersal The model grid to run dispersal for 
    @param cohortToDisperse The cohort for which to apply the dispersal process 
    @param actingCohortFunctionalGroup The functional group index of the acting cohort 
    @param actingCohortNumber The position of the acting cohort within the functional group in the array of grid cell cohorts 
    @param currentMonth The current model month 
     */
    void RunDispersal( ModelGrid& gridForDispersal, Cohort& cohortToDisperse, const unsigned& currentMonth ) {
        // Starvation driven dispersal takes precedence over density driven dispersal (i.e. a cohort can't do both). Also, the delta 
        // arrays only allow each cohort to perform one type of dispersal each time step
        bool cohortDispersed = false;

        // Check for starvation-driven dispersal
        cohortDispersed = CheckStarvationDispersal( gridForDispersal, cohortToDisperse );

        if( !cohortDispersed ) {
            // Check for density driven dispersal
            CheckDensityDrivenDispersal( gridForDispersal, cohortToDisperse );
        }
    }

    //----------------------------------------------------------------------------------------------

    bool CheckStarvationDispersal( ModelGrid& gridForDispersal, Cohort& cohortToDisperse ) {
        // A boolean to check whether a cohort has dispersed
        bool cohortHasDispersed = false;

        // Check for starvation driven dispersal
        // What is the present body mass of the cohort?
        // Note that at present we are just tracking starvation for adults
        double individualBodyMass = cohortToDisperse.mIndividualBodyMass;
        double adultMass = cohortToDisperse.mAdultMass;

        // Assume a linear relationship between probability of dispersal and body mass loss, up to _StarvationDispersalBodyMassThreshold
        // at which point the cohort will try to disperse every time step
        if( individualBodyMass < adultMass ) {
            double proportionalPresentMass = individualBodyMass / adultMass;

            // If the body mass loss is greater than the starvation dispersal body mass threshold, then the cohort tries to disperse
            if( proportionalPresentMass < mStarvationDispersalBodyMassThreshold ) {
                // Cohort tries to disperse

                CalculateDispersalProbability( gridForDispersal, cohortToDisperse, CalculateDispersalSpeed( adultMass ) );

                // Note that regardless of whether or not it succeeds, if a cohort tries to disperse, it is counted as having dispersed for the purposes of not then allowing it to disperse
                // based on its density.
                cohortHasDispersed = true;
                // Otherwise, the cohort has a chance of trying to disperse proportional to its mass lass
            } else {

                // Cohort tries to disperse with a particular probability
                // Draw a random number
                //std::uniform_real_distribution<double> randomNumber( 0.0, 1.0 );
                double randomValue = mRandomNumber1.GetUniform( );
                if( ( ( 1.0 - proportionalPresentMass ) / ( 1.0 - mStarvationDispersalBodyMassThreshold ) ) > randomValue ) {

                    CalculateDispersalProbability( gridForDispersal, cohortToDisperse, CalculateDispersalSpeed( adultMass ) );
                    cohortHasDispersed = true;
                }
            }
        }
        return cohortHasDispersed;
    }
    //----------------------------------------------------------------------------------------------

    void CheckDensityDrivenDispersal( ModelGrid& gridForDispersal, Cohort& cohortToDisperse ) {
        // Check the population density
        double numberOfIndividuals = cohortToDisperse.mCohortAbundance;

        // Get the cell area, in kilometres squared
        double cellArea = cohortToDisperse.mCell->GetCellArea( );

        // If below the density threshold
        if( ( numberOfIndividuals / cellArea ) < mDensityThresholdScaling / cohortToDisperse.mAdultMass ) {
            // Check to see if it disperses (based on the same movement scaling as used in diffusive movement)
            // Calculate dispersal speed for that cohort
            double dispersalSpeed = CalculateDispersalSpeed( cohortToDisperse.mAdultMass );

            // Cohort tries to disperse
            CalculateDispersalProbability( gridForDispersal, cohortToDisperse, dispersalSpeed );
        }
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the average diffusive dispersal speed of individuals in a cohort given their body mass
    @param bodyMass The current body mass of an individual in the cohort 
    @return The (average) dispersal speed in kilometres per month*/
    double CalculateDispersalSpeed( double bodyMass ) {
        return mDispersalSpeedBodyMassScalar * pow( bodyMass, mDispersalSpeedBodyMassExponent );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief   Calculates the probability of responsive dispersal given average individual dispersal speed and grid cell
    @param madingleyGrid The model grid
    @param c the dispersing cohort
    @param dispersalSpeed The average dispersal speed of individuals in the acting cohort 
     */
    void CalculateDispersalProbability( ModelGrid& madingleyGrid, Cohort& c, double dispersalSpeed ) {
        double latCellLength = c.mCell->GetCellHeight( );
        double lonCellLength = c.mCell->GetCellWidth( );

        // Pick a direction at random
        //std::uniform_real_distribution<double> randomNumber( 0.0, 1.0 );
        double randomDirection = mRandomNumber1.GetUniform( )* 2 * acos( -1. );

        // Calculate the u and v components given the dispersal speed
        double uSpeed = dispersalSpeed * cos( randomDirection );
        double vSpeed = dispersalSpeed * sin( randomDirection );

        // Check that the whole cell hasn't moved out (i.e. that dispersal speed is not greater than cell length). 
        // This could happen if dispersal speed was high enough; indicates a need to adjust the time step, or to slow dispersal
        if( uSpeed > lonCellLength )cout << "Dispersal Big U " << uSpeed << endl;
        if( vSpeed > latCellLength )cout << "Dispersal Big V " << vSpeed << endl;

        assert( ( ( uSpeed < lonCellLength ) && ( vSpeed < latCellLength ) ) && "Dispersal probability should always be <= 1" );

        newCell( madingleyGrid, uSpeed, vSpeed, lonCellLength, latCellLength, c );


    }
};
#endif
