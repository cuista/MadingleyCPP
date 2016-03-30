#ifndef IMETABOLISMIMPLEMENTATION_H
#define IMETABOLISMIMPLEMENTATION_H

/** \file IMetabolismImplementation.h
 * \brief the IMetabolismImplementation header file
 */

/** \brief Interface for implementations of the ecological process of metabolism */
class IMetabolismImplementation {
public:
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the biomass lost through metabolism and update the relevant deltas for the acting cohort
    @param actingCohort The position of the acting cohort in the jagged array of grid cell cohorts 
    @param currentTimestep The current model time step 
    @param currentMonth The current month in the model */
    virtual void RunMetabolism( Cohort& actingCohort, unsigned currentTimestep, unsigned currentMonth ) {
        ;
    }
    //----------------------------------------------------------------------------------------------
};
#endif
