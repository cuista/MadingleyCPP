#ifndef METABOLISMIMPLEMENTATION
#define METABOLISMIMPLEMENTATION

/** \brief Interface for implementations of the ecological process of metabolism */
class MetabolismImplementation {
public:

    /** \brief Calculate the biomass lost through metabolism and update the relevant deltas for the acting cohort
    @param actingCohort The position of the acting cohort in the jagged array of grid cell cohorts 
    @param currentTimestep The current model time step 
    @param currentMonth The current month in the model */
    virtual void RunMetabolism( Cohort&, unsigned, unsigned ) {
        std::cout << "If this got called you ended up in a virtual function! MetabolismImplementation RunMetabolism";
    }
};
#endif
