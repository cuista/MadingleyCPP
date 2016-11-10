#ifndef REPRODUCTION_H
#define REPRODUCTION_H
#include <IReproductionImplementation.h>
#include <IEcologicalProcessWithinGridCells.h>
#include <TReproductionBasic.h>

/** \file Reproduction.h
 * \brief the Reproduction header file
 */

/** \brief Performs reproduction */
class Reproduction: public IEcologicalProcessWithinGridCell {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief The available implementations of the reproduction process */
    map<string, IReproductionImplementation*> Implementations;
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /**  \brief Constructor for Reproduction: fills the list of available implementations of reproduction */
    Reproduction( string globalModelTimeStepUnit, bool drawRandomly ) {
        // Add the basic reproduction implementation to the list of implementations
        ReproductionBasic* ReproductionImplementation = new ReproductionBasic( globalModelTimeStepUnit, drawRandomly );
        Implementations["reproduction basic"] = ReproductionImplementation;
    }
    //----------------------------------------------------------------------------------------------

    /** Destructor ensure we tidy everything up */
    ~Reproduction( ) {
        delete Implementations["reproduction basic"];
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Initialize an implementation of reproduction. This is only in here to satisfy the requirements of IEcologicalProcessWithinGridCells

    @param gcl The current grid cell 
    @param params The definitions for functional groups in the model, among other things 
    @param implementationKey The name of the reproduction implementation to initialize 
     */
    void InitializeEcologicalProcess( GridCell& gcl, MadingleyModelInitialisation& params, string implementationKey ) {
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Run reproduction
    @param gcl The current grid cell 
    @param actingCohort The acting cohort 
    @param currentTimeStep The current model time step 
    @param partial Thread-locked variables for the parallelised version 
    @param currentMonth The current model month 
    @params Parameters defined here */
    void RunEcologicalProcess( GridCell& gcl,
            Cohort& actingCohort,
            unsigned currentTimestep,
            ThreadLockedParallelVariables& partial,
            unsigned currentMonth, MadingleyModelInitialisation& params ) {

        // Holds the reproductive strategy of a cohort
        bool _Iteroparous = (params.mCohortFunctionalGroupDefinitions.GetTraitNames( "reproductive strategy", actingCohort.mFunctionalGroupIndex ) == "iteroparity");

        // Assign mass to reproductive potential
        Implementations["reproduction basic"]->RunReproductiveMassAssignment( gcl, actingCohort, currentTimestep, params );

        // Run reproductive events. Note that we can't skip juveniles here as they could conceivably grow to adulthood and get enough biomass to reproduce in a single time step
        // due to other ecological processes
        Implementations["reproduction basic"]->RunReproductionEvents( gcl, actingCohort, currentTimestep, partial, _Iteroparous, currentMonth, params );
    }
    //----------------------------------------------------------------------------------------------
};
#endif
