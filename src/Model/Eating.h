#ifndef EATING_H
#define EATING_H
#include <IEatingImplementation.h>
#include <IEcologicalProcessWithinGridCells.h>
#include <random>
#include <TRevisedPredation.h>
#include <TRevisedHerbivory.h>
/** \file Eating.h
 * \brief the Eating header file
 */

/** \brief Performs eating */
class Eating : public IEcologicalProcessWithinGridCell {
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief The available implementations of the eating process */
    map<string, IEatingImplementation*> Implementations;
    /** \brief Tracks the total time to handle all potential food for omnivores */
    double TotalTimeToEatForOmnivores;
    /** \brief An instance of the simple random number generator class */
    std::default_random_engine RandomNumberGenerator;
public:
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------
    /** \brief Constructor for Eating: fills the list of available implementations of eating */
    Eating(string globalModelTimeStepUnit) {
        // Add the revised herbivory implementation to the list of implementations
        RevisedHerbivory *RevisedHerbivoryImplementation = new RevisedHerbivory(globalModelTimeStepUnit);
        Implementations["revised herbivory"] = RevisedHerbivoryImplementation;
        //Add the revised predation implementation to the list of implementations
        RevisedPredation *RevisedPredationImplementation = new RevisedPredation(globalModelTimeStepUnit);
        Implementations["revised predation"] = RevisedPredationImplementation;
    }
    //----------------------------------------------------------------------------------------------
    /** \brief Destructor to remove pointer storage */
    ~Eating() {
        delete Implementations["revised herbivory"];
        delete Implementations["revised predation"];
    }
    //----------------------------------------------------------------------------------------------
    /** \briefInitializes an implementation of eating 
    @param gcl The current grid cell
    @param params The model parameter set 
    @param implementationKey The name of the implementation of eating to initialize 
    \remarks Eating needs to be initialized every time step */
    void InitializeEcologicalProcess(GridCell& gcl, MadingleyModelInitialisation& params, string implementationKey) {
        // Initialize the implementation of the eating process
        Implementations[implementationKey]->InitializeEatingPerTimeStep(gcl,params);
    }
    //----------------------------------------------------------------------------------------------
    /** \brief Run eating 
    @param gcl The current grid cell 
    @param actingCohort The position of the acting cohort in the jagged array of grid cell cohorts 
    @param currentTimestep The current model time step 
    @param partial Thread-locked variables 
    @param currentMonth The current model month
    @params params The Params */
    void RunEcologicalProcess(GridCell& gcl,
            Cohort& actingCohort, 
            unsigned currentTimestep,
            ThreadLockedParallelVariables& partial,
            unsigned currentMonth, MadingleyModelInitialisation& params){
        // Get the nutrition source (herbivory, carnivory or omnivory) of the acting cohort
        string NutritionSource = params.CohortFunctionalGroupDefinitions.GetTraitNames("Nutrition source", actingCohort.mFunctionalGroupIndex);
        map<string, int> vores;
        vores["herbivore"] = 0;
        vores["carnivore"] = 1;
        vores["omnivore" ] = 2;

        // Switch to the appropriate eating process(es) given the cohort's nutrition source
        switch (vores[NutritionSource]) {
            case 0://"herbivore":

                // Get the assimilation efficiency for herbivory for this cohort from the functional group definitions
                Implementations["revised herbivory"]->AssimilationEfficiency =
                        params.CohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup
                        ("herbivory assimilation", actingCohort.mFunctionalGroupIndex);

                // Get the proportion of time spent eating for this cohort from the functional group definitions
                Implementations["revised herbivory"]->ProportionTimeEating = actingCohort.mProportionTimeActive;

                // Calculate the potential biomass available from herbivory
                if (gcl.IsMarine())
                    Implementations["revised herbivory"]->GetEatingPotentialMarine
                        (gcl, actingCohort,params);
                else

                    Implementations["revised herbivory"]->GetEatingPotentialTerrestrial
                        (gcl, actingCohort,params);

                // Run herbivory to apply changes in autotroph biomass from herbivory and add biomass eaten to the delta arrays
                Implementations["revised herbivory"]->RunEating
                        (gcl,actingCohort,currentTimestep,  params);

                break;

            case 1://"carnivore":

                // Get the assimilation efficiency for predation for this cohort from the functional group definitions
                Implementations["revised predation"]->AssimilationEfficiency =
                        params.CohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup
                        ("carnivory assimilation", actingCohort.mFunctionalGroupIndex);

                Implementations["revised predation"]->ProportionTimeEating = actingCohort.mProportionTimeActive;

                // Calculate the potential biomass available from predation
                if (gcl.IsMarine())
                    Implementations["revised predation"]->GetEatingPotentialMarine
                        (gcl, actingCohort,params);
                else
                    Implementations["revised predation"]->GetEatingPotentialTerrestrial
                        (gcl, actingCohort,params);
                // Run predation to apply changes in prey biomass from predation and add biomass eaten to the delta arrays
                Implementations["revised predation"]->RunEating
                        (gcl,actingCohort, currentTimestep,  params);


                break;

            case 2://"omnivore":

                // Get the assimilation efficiency for predation for this cohort from the functional group definitions 
                Implementations["revised predation"]->AssimilationEfficiency =
                        params.CohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup
                        ("carnivory assimilation", actingCohort.mFunctionalGroupIndex);

                // Get the assimilation efficiency for herbivory for this cohort from the functional group definitions
                Implementations["revised herbivory"]->AssimilationEfficiency =
                        params.CohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup
                        ("herbivory assimilation", actingCohort.mFunctionalGroupIndex);

                // Get the proportion of time spent eating and assign to both the herbivory and predation implementations
                //                   double ProportionTimeEating = actingCohort.ProportionTimeActive;
                Implementations["revised predation"]->ProportionTimeEating = actingCohort.mProportionTimeActive;
                Implementations["revised herbivory"]->ProportionTimeEating = actingCohort.mProportionTimeActive;

                // Calculate the potential biomass available from herbivory
                if (gcl.IsMarine())
                    Implementations["revised herbivory"]->GetEatingPotentialMarine
                        (gcl, actingCohort,params);
                else
                    Implementations["revised herbivory"]->GetEatingPotentialTerrestrial
                        (gcl, actingCohort,params);

                // Calculate the potential biomass available from predation
                if (gcl.IsMarine())
                    Implementations["revised predation"]->GetEatingPotentialMarine
                        (gcl, actingCohort,params);
                else
                    Implementations["revised predation"]->GetEatingPotentialTerrestrial
                        (gcl, actingCohort,params);

                // Calculate the total handling time for all expected kills from predation and expected plant matter eaten in herbivory
                TotalTimeToEatForOmnivores =
                        Implementations["revised herbivory"]->TimeUnitsToHandlePotentialFoodItems +
                        Implementations["revised predation"]->TimeUnitsToHandlePotentialFoodItems;

                // Assign this total time to the relevant variables in both herbviory and predation, so that actual amounts eaten are calculated correctly
                Implementations["revised herbivory"]->TimeUnitsToHandlePotentialFoodItems = TotalTimeToEatForOmnivores;
                Implementations["revised predation"]->TimeUnitsToHandlePotentialFoodItems = TotalTimeToEatForOmnivores;

                // Run predation to update prey cohorts and delta biomasses for the acting cohort
                Implementations["revised predation"]->RunEating
                        (gcl,actingCohort,currentTimestep, params);

                // Run herbivory to update autotroph biomass and delta biomasses for the acting cohort
                Implementations["revised herbivory"]->RunEating
                        (gcl,actingCohort,currentTimestep, params);

                break;

            default:

                // For nutrition source that are not supported, throw an error
                cout << "The model currently does not contain an eating model for nutrition source:" << NutritionSource << endl;
                exit(1);
                break;

        }
        // Check that the biomasses from predation and herbivory in the deltas is a number
        assert(!std::isnan(Cohort::mMassFluxes["biomass"]["predation"]) && "BiomassFromEating is NaN");
        assert(!std::isnan(Cohort::mMassFluxes["biomass"]["herbivory"]) && "BiomassFromEating is NaN");

    }
    //----------------------------------------------------------------------------------------------
};
#endif
