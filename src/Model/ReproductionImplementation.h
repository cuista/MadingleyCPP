#ifndef REPRODUCTIONIMPLEMENTATION
#define REPRODUCTIONIMPLEMENTATION

#include "MadingleyModelInitialisation.h"
#include <map>

/** \brief Interface for implementations of the ecological process of reproduction */
class ReproductionImplementation {
public:

    /** \brief Generate new cohorts from reproductive potential mass
    @param gridCell The current grid cell 
    @param actingCohort The position of the acting cohort in the jagged array of grid cell cohorts 
    @param currentTimestep The current model time step 
    @param partial Thread-locked variables 
    @param iteroparous Whether the acting cohort is iteroparous, as opposed to semelparous 
    @param currentMonth The current model month */
    virtual void RunReproductionEvents( GridCell&, Cohort&, unsigned, ThreadLockedParallelVariables&, bool, unsigned, MadingleyModelInitialisation& ) {
        std::cout << "ReproductionImplementation RunReproductionEvents should be virtual: you probably don't want to be here" << std::endl;
    }

    /** \brief Assigns surplus body mass to reproductive potential mass
    @param gridCell The current grid cell 
    @param actingCohort The position of the acting cohort in the jagged array of grid cell cohorts 
    @param currentTimestep The current model time step 
    @param trackProcesses An instance of ProcessTracker to hold diagnostics for reproduction */
    virtual void RunReproductiveMassAssignment( GridCell& gcl, Cohort& actingCohort, unsigned currentTimestep, MadingleyModelInitialisation& params ) {
        std::cout << "IReproductionImplementation RunReproductiveMassAssignment should be virtual: you probably don't want to be here" << std::endl;
    }
};
#endif
