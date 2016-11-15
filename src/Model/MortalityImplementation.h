#ifndef MORTALITYIMPLEMENTATION
#define MORTALITYIMPLEMENTATION

/** \brief Interface for implementations of the ecological process of mortality */
class MortalityImplementation {
public:
    /** \brief Calculate the proportion of individuals in a cohort that die through a particular type of mortality in a model time step
    @param actingCohort The position of the acting cohort in the jagged array of grid cell cohorts 
    @param bodyMassIncludingChangeThisTimeStep The body mass that individuals in this cohort will have at the end of this time step 
    @param currentTimestep The current model time step 
    @return The number of individuals lost to a cohort through mortality */
    virtual double CalculateMortalityRate( Cohort&, double, unsigned ) {
        std::cout << "If this got called you ended up in a virtual function! MortalityImplementation CalculateMortalityRate";
    }
};
//}
#endif
