#ifndef MORTALITY
#define MORTALITY

#include "MortalityImplementation.h"
#include "EcologicalProcessWithinGridCell.h"
#include "MortalityBackground.h"
#include "TSenescenceMortality.h"
#include "TStarvationMortality.h"
#include "Cohort.h"

#include <limits>

/** \brief  Performs mortality */
class Mortality : public EcologicalProcessWithinGridCell {
public:
    /** \brief Constructor for Mortality: fills the list with available implementations of mortality
    @param globalModelTimeStepUnit The time step for the global model */
    Mortality( std::string globalModelTimeStepUnit );

    /** \brief Destructor cleans up pointers */
    ~Mortality( );

    /** \brief Initialize an implementation of mortality. This is only in here to satisfy the requirements of IEcologicalProcessAcrossGridCells
    @param gcl The current grid cell 
    @param params Needed for groups in the model 
    @param implementationKey The name of the implementation of mortality to initialize */
    void InitializeEcologicalProcess( GridCell&, MadingleyInitialisation&, std::string );

    /** \brief Run mortality
    @param gcl The current grid cell 
    @param actingCohort The position of the acting cohort in the jagged array of grid cell cohorts 
    @param currentTimestep The current model time step 
    @param partial Thread-locked variables 
    @param currentMonth The current model month
    @param params params */
    void RunEcologicalProcess( GridCell&, Cohort&, unsigned, ThreadLockedParallelVariables&, unsigned, MadingleyInitialisation& );

private:
    /** \brief The available implementations of the mortality process */
    std::map< std::string, MortalityImplementation* > mImplementations;

};
#endif
