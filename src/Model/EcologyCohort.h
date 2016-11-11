#ifndef ECOLOGYCOHORT
#define ECOLOGYCOHORT

#include "IEcologicalProcessWithinGridCells.h"
#include "IEatingImplementation.h"
#include "Eating.h"
#include "Reproduction.h"
#include "Mortality.h"
#include "Metabolism.h"
#include "ApplyEcology.h"

/** \brief A class to specify, initalise and run ecological processes pertaining to cohorts */
class EcologyCohort {
public:

    /** \brief Initalise the ecological processes */
    EcologyCohort( );

    void InitialiseEating( GridCell&, MadingleyModelInitialisation& );

    ~EcologyCohort( );

    /** \brief Run ecological processes that operate on cohorts within a single grid cell
    @param gcl The current grid cell 
    @param actingCohort The acting cohort 
    @param currentTimestep The current model time step 
    @param partial Thread-locked local variables 
    @param currentMonth The current model month
    @param params Things that may be needed */
    void RunWithinCellEcology( GridCell&, Cohort&, unsigned, ThreadLockedParallelVariables&, unsigned, MadingleyModelInitialisation& );

    /** \brief Update the properties of the acting cohort and of the environmental biomass pools after running the ecological processes for a cohort
    @param gridCell The current grid cell 
    @param actingCohort The acting cohort 
    @param currentTimestep The current model time step */
    void UpdateEcology( GridCell&, Cohort&, unsigned );

    /** \brief  A vector of stopwatch objects for timing the ecological processes*/
    std::vector< StopWatch > mStopWatches;
    /** \brief A sorted list of formulations of metabolism */
    std::map< std::string, IEcologicalProcessWithinGridCell* > mMetabolismFormulations;
    /** \brief A sorted list of formulations of eating */
    std::map< std::string, IEcologicalProcessWithinGridCell* > mEatingFormulations;
    /** \brief A sorted list of formulations of mortality */
    std::map< std::string, IEcologicalProcessWithinGridCell* > mMortalityFormulations;
    /** \brief A sorted list of formulations of reproduction */
    std::map< std::string, IEcologicalProcessWithinGridCell* > mReproductionFormulations;
    /** \brief An instance of apply ecology */
    ApplyEcology mApplyEcologicalProcessResults;
};
#endif
