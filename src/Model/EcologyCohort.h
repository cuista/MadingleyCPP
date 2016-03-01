#ifndef ECOLOGYCOHORT_H
#define ECOLOGYCOHORT_H
#include <ApplyEcology.h>
#include <IEcologicalProcessWithinGridCells.h>
#include <IEatingImplementation.h>
#include <Eating.h>
#include <Reproduction.h>
#include <Mortality.h>
#include <Metabolism.h>
/** \file EcologyCohort.h
 * \brief the EcologyCohort header file
 */

//
//namespace Madingley
//{

/** \brief A class to specify, initalise and run ecological processes pertaining to cohorts */
class EcologyCohort {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief  A vector of stopwatch objects for timing the ecological processes*/
    vector<StopWatch> s2;
    /** \brief A sorted list of formulations of metabolism */
    map<string, IEcologicalProcessWithinGridCell*> MetabolismFormulations;
    /** \brief A sorted list of formulations of eating */
    map<string, IEcologicalProcessWithinGridCell*> EatingFormulations;
    /** \brief A sorted list of formulations of mortality */
    map<string, IEcologicalProcessWithinGridCell*> MortalityFormulations;
    /** \brief A sorted list of formulations of reproduction */
    map<string, IEcologicalProcessWithinGridCell*> ReproductionFormulations;
    /** \brief An instance of apply ecology */
    ApplyEcology ApplyEcologicalProcessResults;
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------
    /** \brief Initalise the ecological processes */
    EcologyCohort(){;}
    void setup(MadingleyModelInitialisation& params) {
        // Declare and attach eating formulations
        Eating *EatingFormulation = new Eating(params.GlobalModelTimeStepUnit);
        EatingFormulations["Basic eating"] = EatingFormulation;
        // Declare and attach metabolism formulations
        Metabolism *MetabolismFormulation = new Metabolism(params.GlobalModelTimeStepUnit);
        MetabolismFormulations["Basic metabolism"] = MetabolismFormulation;
        // Declare and attach mortality formulations
        Mortality *MortalityFormulation = new Mortality(params.GlobalModelTimeStepUnit);
        MortalityFormulations["Basic mortality"] = MortalityFormulation;
        // Declare and attach mortality formulations
        Reproduction *ReproductionFormulation = new Reproduction(params.GlobalModelTimeStepUnit, params.DrawRandomly);
        ReproductionFormulations["Basic reproduction"] = ReproductionFormulation;
    }
    void initialiseEating(GridCell& gcl, MadingleyModelInitialisation& params){
        // Initialise eating formulations - has to be redone every step?

        EatingFormulations["Basic eating"]->InitializeEcologicalProcess(gcl,params, "revised predation");

        EatingFormulations["Basic eating"]->InitializeEcologicalProcess(gcl,params, "revised herbivory");
    }
    //----------------------------------------------------------------------------------------------
    ~EcologyCohort() {
        delete EatingFormulations["Basic eating"];
        delete MetabolismFormulations["Basic metabolism"];
        delete MortalityFormulations["Basic mortality"];
        delete ReproductionFormulations["Basic reproduction"];
    }
    //----------------------------------------------------------------------------------------------
    /** \brief Run ecological processes that operate on cohorts within a single grid cell
    @param gcl The current grid cell 
    @param actingCohort The acting cohort 
    @param currentTimestep The current model time step 
    @param partial Thread-locked local variables 
    @param currentMonth The current model month
    @param params Things that may be needed */
    void RunWithinCellEcology(GridCell& gcl, Cohort& actingCohort, unsigned currentTimestep, 
            ThreadLockedParallelVariables& partial,  unsigned currentMonth, MadingleyModelInitialisation& params) {

        // RUN EATING
        EatingFormulations["Basic eating"]->RunEcologicalProcess(
                            gcl,actingCohort,currentTimestep, partial,currentMonth, params);


        // RUN METABOLISM - THIS TIME TAKE THE METABOLIC LOSS TAKING INTO ACCOUNT WHAT HAS BEEN INGESTED THROUGH EATING
        MetabolismFormulations["Basic metabolism"]->RunEcologicalProcess(
                            gcl,actingCohort,currentTimestep, partial,currentMonth, params);


        // RUN REPRODUCTION - TAKING INTO ACCOUNT NET BIOMASS CHANGES RESULTING FROM EATING AND METABOLISING
        ReproductionFormulations["Basic reproduction"]->RunEcologicalProcess(
                            gcl,actingCohort,currentTimestep, partial,currentMonth, params);


        // RUN MORTALITY - TAKING INTO ACCOUNT NET BIOMASS CHANGES RESULTING FROM EATING, METABOLISM AND REPRODUCTION
        MortalityFormulations["Basic mortality"]->RunEcologicalProcess(
                            gcl,actingCohort,currentTimestep, partial,currentMonth, params);
    }
    //----------------------------------------------------------------------------------------------
    /** \brief Update the properties of the acting cohort and of the environmental biomass pools after running the ecological processes for a cohort
    @param gridCell The current grid cell 
    @param actingCohort The acting cohort 
    @param currentTimestep The current model time step 
    */
    void UpdateEcology(GridCell& gcl, Cohort& actingCohort, unsigned currentTimestep) {
        // Apply the results of within-cell ecological processes
        ApplyEcologicalProcessResults.UpdateAllEcology(gcl, actingCohort, currentTimestep);
    }
    //----------------------------------------------------------------------------------------------
};
#endif
