#ifndef IEATINGIMPLEMENTATION_H
#define IEATINGIMPLEMENTATION_H
#include <FunctionalGroupDefinitions.h>
//#include <ProcessTracker.h>
#include <MadingleyModelInitialisation.h>
#include <map>
/** \file IEatingImplementation.h
 * \brief the IEatingImplementation header file
 */


//namespace Madingley
//{

/** \brief //    /// Interface for implementations of the ecological process of eating */
class IEatingImplementation {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------

    /** \brief Assimilation efficiency of food mass into acting cohort mass*/
    double AssimilationEfficiency;
    /** \brief Proportion of time spent eating*/
    double ProportionTimeEating;
    /** \brief Time to handle all prey cohorts or plant mass encountered*/
    double TimeUnitsToHandlePotentialFoodItems;
    /** \brief List of functional group indices to act on*/
    vector<int> FunctionalGroupIndicesToEat;
    /** \brief The total biomass eaten by the acting cohort */
    double TotalBiomassEatenByCohort;
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief Initialises eating implementation each time step
    @param gcl The current grid cell 
    @param madingleyStockDefinitions The definitions for stocks in the model  */
    virtual void InitializeEatingPerTimeStep( GridCell& gcl, MadingleyModelInitialisation& params ) {
        ;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the potential biomass that could be gained through eating for marine cells
    @param gcl The current grid cell 
    @param params The current model settings
     */
    virtual void GetEatingPotentialMarine( GridCell& gcl, Cohort& actingCohort,
            MadingleyModelInitialisation& params ) {
        ;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the potential biomass that could be gained through eating for terrestrial cells
    @param gcl The current grid cell 
    @param actingCohort The position of the acting cohort in the jagged array of cohorts 
    @param params The current model thingies  */
    virtual void GetEatingPotentialTerrestrial( GridCell& gcl, Cohort& actingCohort,
            MadingleyModelInitialisation& params ) {
        ;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the actual biomass eaten from each cohort or sotck, apply changes from eating to the cohorts or stocks eaten, and update deltas for the acting cohort
    @param gcl The current grid cell 
    @param currentTimestep The current model time step 
    @param params the actual model settings  */
    virtual void RunEating( GridCell& gcl, Cohort& actingCohort,
            unsigned currentTimestep,
            MadingleyModelInitialisation& params ) {
        cout << "Top level IEatingImplementation RunEating process called: should be virtual so this is probably not what you want!" << endl;

    }
    //----------------------------------------------------------------------------------------------

    IEatingImplementation( ) {
        ;
    }
    //----------------------------------------------------------------------------------------------

    IEatingImplementation( string globalModelTimeStepUnit ) {
        cout << "Virtual IEatingImplementation constructor called:This is probably a mistake" << endl;
    }
    //----------------------------------------------------------------------------------------------

    virtual ~IEatingImplementation( ) {
        ;
    }
};
#endif
