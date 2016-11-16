#ifndef REPRODUCTION
#define REPRODUCTION

#include "EcologicalProcessWithinGridCell.h"
#include "TReproductionBasic.h"

/** \brief Performs reproduction */
class Reproduction : public EcologicalProcessWithinGridCell {
public:
    /** \brief The available implementations of the reproduction process */
    std::map< std::string, ReproductionImplementation* > mImplementations;

    /**  \brief Constructor for Reproduction: fills the list of available implementations of reproduction */
    Reproduction( std::string, bool );

    /** Destructor ensure we tidy everything up */
    ~Reproduction( );

    /** \brief Initialize an implementation of reproduction. This is only in here to satisfy the requirements of IEcologicalProcessWithinGridCells
    @param gcl The current grid cell 
    @param params The definitions for functional groups in the model, among other things 
    @param implementationKey The name of the reproduction implementation to initialize */
    void InitializeEcologicalProcess( GridCell& gcl, MadingleyInitialisation& params, std::string implementationKey ) ;

    /** \brief Run reproduction
    @param gcl The current grid cell 
    @param actingCohort The acting cohort 
    @param currentTimeStep The current model time step 
    @param partial Thread-locked variables for the parallelised version 
    @param currentMonth The current model month 
    @params Parameters defined here */
    void RunEcologicalProcess( GridCell&, Cohort&, unsigned, ThreadLockedParallelVariables&, unsigned, MadingleyInitialisation& );
};
#endif
