#ifndef TBACKGROUNDMORTALITY_H
#define TBACKGROUNDMORTALITY_H
/** \file TBackgroundMortality.h
 * \brief the TBackgroundMortality header file
 */

/** \brief A formulation of the process of background mortality, i.e. mortality from disease, accidents and other random events*/
class BackgroundMortality: public MortalityImplementation {
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief The time units associated with this implementation of dispersal*/
    const string TimeUnitImplementation = "Day";
    /** \brief Cohort background mortality rate - the proportion of individuals dying in a time step */
    const double MortalityRate = 0.001;
    /** \brief Scalar to convert from the time step units used by this formulation of dispersal to global model time step units */
    double DeltaT;
    /** \brief Include Utility class */
    UtilityFunctions Utilities;
public:
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief Constructor for background mortality: assigns all parameter values*/
    BackgroundMortality( string globalModelTimeStepUnit ) {
        // Calculate the scalar to convert from the time step units used by this implementation of mortality to the global model time step units
        DeltaT = Utilities.ConvertTimeUnits( globalModelTimeStepUnit, TimeUnitImplementation );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the rate of individuals in a cohort that die from background mortality in a model time step
    @param actingCohort The position of the acting cohort in the jagged array of grid cell cohorts 
    @param currentTimestep The current model time step 
    @return The rate of individuals in the cohort that die from background mortality
     */
    double CalculateMortalityRate( Cohort& actingCohort, double bodyMassIncludingChangeThisTimeStep, unsigned currentTimestep ) {
        // Convert from mortality rate per mortality formulation time step to mortality rate per model time step
        return MortalityRate * DeltaT;
    }
};
#endif
