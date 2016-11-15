#ifndef EATING
#define EATING

#include "EatingImplementation.h"
#include "EcologicalProcessWithinGridCell.h"
#include "TRevisedPredation.h"
#include "TRevisedHerbivory.h"

/** \brief Performs eating */
class Eating : public EcologicalProcessWithinGridCell {
public:

    /** \brief Constructor for Eating: fills the list of available implementations of eating */
    Eating( string );
    
    /** \brief Destructor to remove pointer storage */
    ~Eating( );

    /** \briefInitializes an implementation of eating 
    @param gcl The current grid cell
    @param params The model parameter set 
    @param implementationKey The name of the implementation of eating to initialize 
    \remarks Eating needs to be initialized every time step */
    
    void InitializeEcologicalProcess( GridCell&, MadingleyModelInitialisation&, string );
    /** \brief Run eating 
    @param gcl The current grid cell 
    @param actingCohort The position of the acting cohort in the jagged array of grid cell cohorts 
    @param currentTimestep The current model time step 
    @param partial Thread-locked variables 
    @param currentMonth The current model month
    @params params The Params */
    void RunEcologicalProcess( GridCell&, Cohort&, unsigned, ThreadLockedParallelVariables&, unsigned, MadingleyModelInitialisation& );

    /** \brief The available implementations of the eating process */
    map<string, EatingImplementation*> mImplementations;
    /** \brief Tracks the total time to handle all potential food for omnivores */
    double mTotalTimeToEatForOmnivores;
};
#endif
