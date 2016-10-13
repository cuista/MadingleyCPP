#ifndef TADVECTIVEDISPERSAL_H
#define TADVECTIVEDISPERSAL_H

#include <assert.h>
#include "IDispersalImplementation.h"
#include "UtilityFunctions.h"
#include "Environment.h"
#include "Parameters.h"
#include "Types.h"
/** \file TAdvectiveDispersal.h
 * \brief the TAdvectiveDispersal header file
 */

/** \brief A formulation of the process of dispersal */
class AdvectiveDispersal : public IDispersalImplementation {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------

    /** \brief The horizontal diffusivity parameter (m^2/s) */
    const double mHorizontalDiffusivity = 100;
    /** \brief The length of the time-step for advective dispersal, in hours*/
    const unsigned mAdvectiveModelTimeStepLengthHours = 18;
    /** \brief Horizontal diffusivity in km^2/advective-dispersal-time-step*/
    const double mHorizontalDiffusivityKmSqPerADTimeStep = mHorizontalDiffusivity / ( 1000 * 1000 ) * 60 * 60 * mAdvectiveModelTimeStepLengthHours;
    /** \brief Time unit scalar to apply to advective dispersal*/
    double mAdvectionTimeStepsPerModelTimeStep;
    /** \brief The time units associated with this implementation of dispersal*/
    const std::string mTimeUnitImplementation = "month";
    /** \brief Factor to convert velocity from m/s to km/month*/
    double mVelocityUnitConversion;
    /** \brief Scalar to convert from the time step units used by this formulation of dispersal to global model time step units */
    double mDeltaT;
    /** \brief Include Utility class */
    UtilityFunctions mUtilities;

    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief Constructor for dispersal: assigns all parameter values */
    AdvectiveDispersal( ) {
        // Calculate the scalar to convert from the time step units used by this implementation of dispersal to the global model time step units
        mDeltaT = mUtilities.ConvertTimeUnits( Parameters::Get( )->GetTimeStepUnits( ), mTimeUnitImplementation );

        // Initialise the advective dispersal temporal scaling to adjust between time steps appropriately
        mAdvectionTimeStepsPerModelTimeStep = mUtilities.ConvertTimeUnits( Parameters::Get( )->GetTimeStepUnits( ), "day" ) * 24 / mAdvectiveModelTimeStepLengthHours;

        // Convert velocity from m/s to km/month. Note that if the _TimeUnitImplementation changes, this will also have to change.
        mVelocityUnitConversion = 60 * 60 * 24 * mUtilities.ConvertTimeUnits( Parameters::Get( )->GetTimeStepUnits( ), "day" ) * mDeltaT / 1000;

        // Set the seed for the random number generator
        unsigned seed = 14141;
        if( Parameters::Get( )->GetDrawRandomly( ) == true ) {
            seed = std::chrono::system_clock::now( ).time_since_epoch( ).count( );
        }
        mRandomNumber1.SetSeed( seed );

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Run advective dispersal
    @param cellIndex The longitudinal and latitudinal indices of the focal grid cell 
    @param gridForDispersal The model grid to run dispersal for 
    @param cohortToDisperse The cohort to run dispersal for 
    @param actingCohortFunctionalGroup The functional group index of the acting cohort 
    @param actingCohortNumber The position of the acting cohort within the functional group in the array of grid cell cohorts 
    @param currentMonth The current model month 
     */
    void RunDispersal( ModelGrid& gridForDispersal, Cohort& cohortToDisperse, const unsigned& currentMonth ) {

        // Loop through a number of times proportional to the rescaled dispersal
        for( int mm = 0; mm < mAdvectionTimeStepsPerModelTimeStep; mm++ ) {
            // Get the probability of dispersal and return a candidate cell
            CalculateDispersalProbability( gridForDispersal, cohortToDisperse, currentMonth );
        }
    }
    //----------------------------------------------------------------------------------------------

    /** \brief    Convert dispersal speed from m per second to km per dispersal time step (currently 18h)
    @param dispersalSpeed The dispersal speed in m per second 
    @return The dispersal speed in kilometres per time step*/
    inline const double RescaleDispersalSpeed( const double& dispersalSpeed ) const {
        //            // Units are metres per second; need to convert to kilometres per global time step (currently one month) - use VelocityUnitConversion for this.
        //            // Also rescale based on the time step of the advective dispersal model - currently 18h
        return dispersalSpeed * mVelocityUnitConversion / mAdvectionTimeStepsPerModelTimeStep;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief    Calculates the probability of advective dispersal given the grid cell
    @param madingleyGrid The model grid 
    @param c a cohort
    @return a grid cell*/
    void CalculateDispersalProbability( ModelGrid& madingleyGrid, Cohort& c, const unsigned& currentMonth ) {
        // Advective speed in u (longitudinal) direction
        double uAdvectiveSpeed;

        // Advective speed in v (latitudinal) direction
        double vAdvectiveSpeed;

        // Distance travelled in u (longitudinal) direction
        double uDistanceTravelled;

        // Distance travelled in v (latitudinal) direction
        double vDistanceTravelled;

        // U and V components of the diffusive velocity
        Types::DoubleVector diffusiveUandVComponents( 2 );

        // Length in km of a cell boundary latitudinally
        double latCellLength;

        // Length in km of a cell boundary longitudinally
        double lonCellLength;

        // Cell area, in kilometres squared
        // double cellArea;

        // A variable to track whether a named data layer exists
        // bool varExists;

        // Get the u speed and the v speed from the cell data
        uAdvectiveSpeed = Environment::Get( "uVel", *( c.mDestination ) );
        assert( uAdvectiveSpeed > -9999 );

        vAdvectiveSpeed = Environment::Get( "vVel", *( c.mDestination ) );
        assert( vAdvectiveSpeed > -9999 );
        // Calculate the diffusive movement speed, with a direction chosen at random
        diffusiveUandVComponents = CalculateDiffusion( );
        // Calculate the distance travelled in this dispersal (not global) time step. both advective and diffusive speeds need to have been converted to km / advective model time step
        uDistanceTravelled = RescaleDispersalSpeed( uAdvectiveSpeed ) + diffusiveUandVComponents[0];
        vDistanceTravelled = RescaleDispersalSpeed( vAdvectiveSpeed ) + diffusiveUandVComponents[1];

        // Check that the u distance travelled and v distance travelled are not greater than the cell length

        latCellLength = c.GetDestination( ).GetCellHeight( );
        lonCellLength = c.GetDestination( ).GetCellWidth( );
        if( abs( uDistanceTravelled ) > lonCellLength ) cout << "BIG U " << uAdvectiveSpeed << endl;
        assert( abs( uDistanceTravelled ) <= lonCellLength && "u velocity greater than cell width" );
        assert( abs( vDistanceTravelled ) <= latCellLength && "v velocity greater than cell width" );
        GridCell* nc = newCell( madingleyGrid, uDistanceTravelled, vDistanceTravelled, lonCellLength, latCellLength, c.mDestination );

        if( nc != 0 ) {
            c.TryLivingAt( nc );
        }

    }
    //----------------------------------------------------------------------------------------------

    /** \brief    Get a randomly directed diffusion vector. This is derived from the LTRANS model formulation, which itself is derived from Visser 1997 (MEPS)
    We assume that the standard deviation of the random draw is 1.0

    @return A two element array, where the first element is the diffusion component in the u direction, and the second component is the
    diffusion component in the v direction*/
    Types::DoubleVector CalculateDiffusion( ) {
        // Create the array with which to send the output
        Types::DoubleVector outputsUandV( 2 );

        // Note that this formulation drops the delta t because we set the horizontal diffusivity to be at the same temporal
        // scale as the time step

        outputsUandV[ 0 ] = mRandomNumber1.GetNormal( ) * sqrt( ( 2.0 * mHorizontalDiffusivityKmSqPerADTimeStep ) );
        outputsUandV[ 1 ] = mRandomNumber1.GetNormal( ) * sqrt( ( 2.0 * mHorizontalDiffusivityKmSqPerADTimeStep ) );

        return outputsUandV;
    }
    //----------------------------------------------------------------------------------------------
};

#endif
