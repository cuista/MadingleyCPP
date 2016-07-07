#ifndef TRESPONSIVEDISPERSAL_H
#define TRESPONSIVEDISPERSAL_H
#include <IDispersalImplementation.h>
#include <UtilityFunctions.h>
#include <random>
#include <chrono>
#include <assert.h>
#include <math.h>
#include <vector>
/** \file TResponsiveDispersal.h
 * \brief the TResponsiveDispersal header file
 */

/** \brief A formulation of the process of responsive dispersal */
class ResponsiveDispersal: public IDispersalImplementation {
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief The time units associated with this implementation of dispersal */
    const string TimeUnitImplementation = "month";

    /** \brief Density threshold below which adult individuals may move to look for other adults of the same cohort

    \remarks The density scales with cohort weight via: Min Density = DensityThresholdScaling / MatureMass (g) */
    const double DensityThresholdScaling = 50000;
    /** \brief Scalar relating dispersal speed to individual body mass */
    const double DispersalSpeedBodyMassScalar = 0.0278;
    /** \brief Body-mass exponent of the relationship between disperal speed and individual body mass */
    const double DispersalSpeedBodyMassExponent = 0.48;

    /** \brief The proportion of body mass loss at which the cohort will try to disperse every time during a time step */
    const double StarvationDispersalBodyMassThreshold = 0.8;

    /** \brief Scalar to convert from the time step units used by this formulation of dispersal to global model time step units */
    double DeltaT;

public:
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief Assigns all parameter values for repsonsive dispersal */
    ResponsiveDispersal( ) {

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
        bool CohortDispersed = false;

        // Check for starvation-driven dispersal
        CohortDispersed = CheckStarvationDispersal( gridForDispersal, cohortToDisperse );

        if( !CohortDispersed ) {
            // Check for density driven dispersal
            CheckDensityDrivenDispersal( gridForDispersal, cohortToDisperse );
        }
    }

    //----------------------------------------------------------------------------------------------

    bool CheckStarvationDispersal( ModelGrid& gridForDispersal, Cohort& cohortToDisperse ) {
        // A boolean to check whether a cohort has dispersed
        bool CohortHasDispersed = false;

        // Check for starvation driven dispersal
        // What is the present body mass of the cohort?
        // Note that at present we are just tracking starvation for adults
        double IndividualBodyMass = cohortToDisperse.mIndividualBodyMass;
        double AdultMass = cohortToDisperse.mAdultMass;

        // Assume a linear relationship between probability of dispersal and body mass loss, up to _StarvationDispersalBodyMassThreshold
        // at which point the cohort will try to disperse every time step
        if( IndividualBodyMass < AdultMass ) {
            double ProportionalPresentMass = IndividualBodyMass / AdultMass;

            // If the body mass loss is greater than the starvation dispersal body mass threshold, then the cohort tries to disperse
            if( ProportionalPresentMass < StarvationDispersalBodyMassThreshold ) {
                // Cohort tries to disperse

                CalculateDispersalProbability( gridForDispersal, cohortToDisperse, CalculateDispersalSpeed( AdultMass ) );

                // Note that regardless of whether or not it succeeds, if a cohort tries to disperse, it is counted as having dispersed for the purposes of not then allowing it to disperse
                // based on its density.
                CohortHasDispersed = true;
                // Otherwise, the cohort has a chance of trying to disperse proportional to its mass lass
            } else {

                // Cohort tries to disperse with a particular probability
                // Draw a random number
                //std::uniform_real_distribution<double> randomNumber( 0.0, 1.0 );
                double RandomValue = randomNumber.GetUniform();
                if( ( ( 1.0 - ProportionalPresentMass ) / ( 1.0 - StarvationDispersalBodyMassThreshold ) ) > RandomValue ) {

                    CalculateDispersalProbability( gridForDispersal, cohortToDisperse, CalculateDispersalSpeed( AdultMass ) );

                    CohortHasDispersed = true;
                }
            }

        }
        return CohortHasDispersed;
    }
    //----------------------------------------------------------------------------------------------

    void CheckDensityDrivenDispersal( ModelGrid& gridForDispersal, Cohort& cohortToDisperse ) {
        // Check the population density
        double NumberOfIndividuals = cohortToDisperse.mCohortAbundance;

        // Get the cell area, in kilometres squared
        double CellArea = cohortToDisperse.mCurrentLocation->GetCellArea( );

        // If below the density threshold
        if( ( NumberOfIndividuals / CellArea ) < DensityThresholdScaling / cohortToDisperse.mAdultMass ) {
            // Check to see if it disperses (based on the same movement scaling as used in diffusive movement)
            // Calculate dispersal speed for that cohort
            double DispersalSpeed = CalculateDispersalSpeed( cohortToDisperse.mAdultMass );

            // Cohort tries to disperse
            CalculateDispersalProbability( gridForDispersal, cohortToDisperse, DispersalSpeed );

        }
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the average diffusive dispersal speed of individuals in a cohort given their body mass
    @param bodyMass The current body mass of an individual in the cohort 
    @return The (average) dispersal speed in kilometres per month*/
    double CalculateDispersalSpeed( double bodyMass ) {
        return DispersalSpeedBodyMassScalar * pow( bodyMass, DispersalSpeedBodyMassExponent );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief   Calculates the probability of responsive dispersal given average individual dispersal speed and grid cell
    @param madingleyGrid The model grid
    @param c the dispersing cohort
    @param dispersalSpeed The average dispersal speed of individuals in the acting cohort 
     */
    void CalculateDispersalProbability( ModelGrid& madingleyGrid, Cohort& c, double dispersalSpeed ) {
        double LatCellLength = c.mCurrentLocation->GetCellHeight( );
        double LonCellLength = c.mCurrentLocation->GetCellWidth( );

        // Pick a direction at random
        //std::uniform_real_distribution<double> randomNumber( 0.0, 1.0 );
        double RandomDirection = randomNumber.GetUniform()* 2 * acos( -1. );

        // Calculate the u and v components given the dispersal speed
        double uSpeed = dispersalSpeed * cos( RandomDirection );
        double vSpeed = dispersalSpeed * sin( RandomDirection );

        // Check that the whole cell hasn't moved out (i.e. that dispersal speed is not greater than cell length). 
        // This could happen if dispersal speed was high enough; indicates a need to adjust the time step, or to slow dispersal
        if( uSpeed > LonCellLength )cout << "Dispersal Big U " << uSpeed << endl;
        if( vSpeed > LatCellLength )cout << "Dispersal Big V " << vSpeed << endl;

        assert( ( ( uSpeed < LonCellLength ) && ( vSpeed < LatCellLength ) ) && "Dispersal probability should always be <= 1" );

        GridCell* nc =newCell( madingleyGrid, uSpeed, vSpeed, LonCellLength, LatCellLength, c.mDestination );

        if (nc!=0)c.TryLivingAt(nc);

    }

};
#endif
