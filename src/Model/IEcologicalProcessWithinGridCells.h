#ifndef IECOLOGICALPROCESSWITHINGRIDCELLS_H
#define IECOLOGICALPROCESSWITHINGRIDCELLS_H

#include <MadingleyModelInitialisation.h>
#include <ThreadLocked.h> 

/** \file IEcologicalProcessWithinGridCells.h
 * \brief the IEcologicalProcessWithinGridCells header file
 */

/** \brief Interface for ecological process code */


class IEcologicalProcessWithinGridCell {
public:
    //----------------------------------------------------------------------------------------------
    /** \brief Run the ecological process
    @param gridCell The current grid cell 
    @param actingCohort The  acting cohort  
   @param currentTimestep The current model time step 
    @param partial Thread-locked variables 
    @param currentMonth The current model month 
    @param params The instance of the MadingleyModelInitialisation class for this simulation 
     */
    virtual void RunEcologicalProcess(GridCell& gcl,
            Cohort& actingCohort, 
            unsigned currentTimestep,
            ThreadLockedParallelVariables& partial,
            unsigned currentMonth, MadingleyModelInitialisation& params) {
       cout<<"Top level IEcologicalWithinGridCell RunEcologicalProcess process called: should be virtual so this is probably not what you want!"<<endl ;
    }
    //----------------------------------------------------------------------------------------------
    /** \brief Initialises an implementation of the ecological process
    @param gridCell The current grid cell
    @params params parameters for the model 
    @param implementationKey The name of the specific implementation of this process to initialize 
     */
    virtual void InitializeEcologicalProcess(GridCell& gcl, MadingleyModelInitialisation& params, string implementationKey) {
        ;
    }
    //----------------------------------------------------------------------------------------------
    /** \brief Virtual destructor */
    virtual ~IEcologicalProcessWithinGridCell() {
        ;
    }
};

#endif
