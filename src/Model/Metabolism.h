#ifndef METABOLISM_H
#define METABOLISM_H
#include <IEcologicalProcessWithinGridCells.h>
#include <IMetabolismImplementation.h>
#include <TMetabolismHeterotroph.h>
#include <TMetabolismEndotherm.h>
#include <TMetabolismEctotherm.h>
#include <ThreadLocked.h>
/** \file Metabolism.h
 * \brief the Metabolism header file
 */

//
//namespace Madingley
//{

/** \brief  Performs metabolism */
class Metabolism: public IEcologicalProcessWithinGridCell {
public:
    /** \brief The available implementations of the metabolism process*/
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    map<string, IMetabolismImplementation*> Implementations;
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief Constructor Metabolism: fills the list of available implementations of metabolism*/
    Metabolism( string globalModelTimeStepUnit ) {
        // Add the basic endotherm metabolism implementation to the list of implementations
        MetabolismEndotherm* MetabolismEndothermImplementation = new MetabolismEndotherm( globalModelTimeStepUnit );
        Implementations["basic endotherm"] = MetabolismEndothermImplementation;

        // Add the basic ectotherm metabolism implementation to the list of implementations
        MetabolismEctotherm* MetabolismEctothermImplementation = new MetabolismEctotherm( globalModelTimeStepUnit );
        Implementations["basic ectotherm"] = MetabolismEctothermImplementation;
    }
    //----------------------------------------------------------------------------------------------

    /** Destrcutor to tidy up pointers*/
    ~Metabolism( ) {
        delete Implementations["basic endotherm"];
        delete Implementations["basic ectotherm"];
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Initializes an implementation of metabolism
    @param gcl The current grid cell 
    @param params A bunch of parameters 'n' stuff 'n' things 
    @param implementationKey The name of the implementation of metabolism to initialize  */
    void InitializeEcologicalProcess( GridCell& gcl, MadingleyModelInitialisation& params, string implementationKey ) {

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Run metabolism
    @param gcl The current grid cell 
    @param actingCohort The acting cohort  
    @param currentTimestep The current model time step 
    @param partial Thread-locked variables 
    @param currentMonth The current model month
    @param params some parameters  */
    void RunEcologicalProcess( GridCell& gcl,
            Cohort& actingCohort,
            unsigned currentTimestep,
            ThreadLockedParallelVariables& partial,
            unsigned currentMonth, MadingleyModelInitialisation& params ) {

        if( params.CohortFunctionalGroupDefinitions.GetTraitNames( "Heterotroph/Autotroph", actingCohort.mFunctionalGroupIndex ) == "heterotroph" ) {
            if( params.CohortFunctionalGroupDefinitions.GetTraitNames( "Endo/Ectotherm", actingCohort.mFunctionalGroupIndex ) == "endotherm" ) {

                Implementations["basic endotherm"]->RunMetabolism( actingCohort, currentTimestep, currentMonth );
            } else {
                Implementations["basic ectotherm"]->RunMetabolism( actingCohort, currentTimestep, currentMonth );

            }

        }

    }
    //----------------------------------------------------------------------------------------------
};
#endif
