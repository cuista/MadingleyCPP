#ifndef TSENESCENCEMORTALITY_H
#define TSENESCENCEMORTALITY_H
/** \file TSenescenceMortality.h
 * \brief the TSenescenceMortality header file
 */

/** \brief A formulation of the process of senescence mortality*/
class SenescenceMortality: public Mortality {
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief The time units associated with this implementation of dispersal*/
    const string TimeUnitImplementation = "Day";
    /** \brief Cohort senescence mortality rate scalar: the rate of individuals dying in a time step when they reach maturity */
    const double MortalityRate = 0.003;
    /** \brief Scalar to convert from the time step units used by this formulation of dispersal to global model time step units */
    double DeltaT;
    /** \brief Include Utility class */
    UtilityFunctions Utilities;
public:
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief Constructor for senscence mortality: assigns all parameter values */
    SenescenceMortality( string globalModelTimeStepUnit ) {
        // Calculate the scalar to convert from the time step units used by this implementation of mortality to the global model time step units
        DeltaT = Utilities.ConvertTimeUnits( globalModelTimeStepUnit, TimeUnitImplementation );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the rate of individuals in a cohort that die from senescence mortality in a model time step 
    @param actingCohort The position of the acting cohort in the jagged array of grid cell cohorts 
    @param currentTimestep The current model time step 
    @return The rate of individuals in the cohort that die from senescence mortality*/
    double CalculateMortalityRate( Cohort& actingCohort, double bodyMassIncludingChangeThisTimeStep, unsigned currentTimestep ) {
        // Calculate the age (in model time steps) that the cohort reached maturity
        double TimeToMaturity = actingCohort.mMaturityTimeStep - actingCohort.mBirthTimeStep;

        // Calculate how many model time steps since the cohort reached maturity
        double AgePostMaturity = currentTimestep - actingCohort.mMaturityTimeStep;

        // Calculate the time since maturity as a fraction of the time that it took the cohort to reach maturity
        double FractionalAgePostMaturity = AgePostMaturity / ( TimeToMaturity + 1 );

        // Calculate the mortality rate per mortality formulation time step as a function of the exponential of the previous fraction
        double AgeRelatedMortalityRate = MortalityRate * exp( FractionalAgePostMaturity );

        // Convert the mortality rate from formulation time step units to model time step units
        return AgeRelatedMortalityRate * DeltaT;
    }
    //----------------------------------------------------------------------------------------------
};
#endif
