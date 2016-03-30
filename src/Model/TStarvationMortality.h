#ifndef TSTARVATIONMORTALITY_H
#define TSTARVATIONMORTALITY_H
/** \file TStarvationMortality.h
 * \brief the TStarvationMortality header file
 */

/** \brief A formulation of the process of starvation mortality */
class StarvationMortality: public IMortalityImplementation {
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief The time units associated with this implementation of dispersal*/
    const string TimeUnitImplementation = "Day";
    /** \brief The inflection point of the curve describing the relationship between body mass and mortality rate */
    double _LogisticInflectionPoint = 0.6;
    /** \brief The steepness of the curve describing the relationship between body mass and mortality rate */
    double _LogisticScalingParameter = 0.05;
    /** \brief The asymptote of the curve describing the relationship between body mass and mortality rate */
    double _MaximumStarvationRate = 1;
    /** \brief Scalar to convert from the time step units used by this formulation of dispersal to global model time step units */
    double DeltaT;
    /** \brief Include Utility class */
    UtilityFunctions Utilities;
public:
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief Constructor for starvation mortality: assigns all parameter values */
    StarvationMortality( string globalModelTimeStepUnit ) {
        //Calculate the scalar to convert from the time step units used by this implementation of mortality to the global model time step units
        DeltaT = Utilities.ConvertTimeUnits( globalModelTimeStepUnit, TimeUnitImplementation );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the proportion of individuals in a cohort that die from starvation mortality each time step
    @param actingCohort The position of the acting cohort in the jagged array of grid cell cohorts 
    @param bodyMassIncludingChangeThisTimeStep Body mass including change from other ecological functions this time step; should not exceed adult mass 
    @param currentTimestep The current model time step 
    @return The proportion of individuals in the cohort that die from starvation mortality*/
    double CalculateMortalityRate( Cohort& actingCohort, double bodyMassIncludingChangeThisTimeStep, unsigned currentTimestep ) {
        // Calculate the starvation rate of the cohort given individual body masses compared to the maximum body
        // mass ever achieved
        double MortalityRate = CalculateStarvationRate( actingCohort, bodyMassIncludingChangeThisTimeStep );

        // Convert the mortality rate from formulation time step units to model time step units
        return MortalityRate * DeltaT;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief
    Calculates the rate of starvation mortality given current body mass and the maximum body mass ever achieved. Note that metabolic costs are already included in the deltas passed in
    the body mass including change this time step, so no change in body mass should mean no starvation (as metabolic costs have already been met)

    @param actingCohort The position of the acting cohort in the jagged array of grid cell cohorts 
    @param bodyMassIncludingChangeThisTimeStep Body mass including change from other ecological functions this time step; should not exceed adult mass 
    @return The starvation mortality rate in mortality formulation time step units*/
    double CalculateStarvationRate( Cohort& actingCohort, double bodyMassIncludingChangeThisTimeStep ) {
        if( bodyMassIncludingChangeThisTimeStep < actingCohort.MaximumAchievedBodyMass ) {
            // Calculate the first part of the relationship between body mass and mortality rate
            double k = -( bodyMassIncludingChangeThisTimeStep - _LogisticInflectionPoint * actingCohort.
                    MaximumAchievedBodyMass ) / ( _LogisticScalingParameter * actingCohort.MaximumAchievedBodyMass );

            // Calculate mortality rate
            return _MaximumStarvationRate / ( 1 + exp( -k ) );
        } else
            return 0;
    }
    //----------------------------------------------------------------------------------------------
};
#endif
