#include "Eating.h"

Eating::Eating( string globalModelTimeStepUnit ) {
    // Add the revised herbivory implementation to the list of implementations
    RevisedHerbivory *RevisedHerbivoryImplementation = new RevisedHerbivory( globalModelTimeStepUnit );
    mImplementations["revised herbivory"] = RevisedHerbivoryImplementation;
    //Add the revised predation implementation to the list of implementations
    RevisedPredation *RevisedPredationImplementation = new RevisedPredation( globalModelTimeStepUnit );
    mImplementations["revised predation"] = RevisedPredationImplementation;
    mTotalTimeToEatForOmnivores = 0;
}

Eating::~Eating( ) {
    delete mImplementations["revised herbivory"];
    delete mImplementations["revised predation"];
}

void Eating::InitializeEcologicalProcess( GridCell& gcl, MadingleyModelInitialisation& params, string implementationKey ) {
    // Initialize the implementation of the eating process
    mImplementations[implementationKey]->InitializeEatingPerTimeStep( gcl, params );
}

void Eating::RunEcologicalProcess( GridCell& gcl, Cohort& actingCohort, unsigned currentTimestep, ThreadLockedParallelVariables& partial, unsigned currentMonth, MadingleyModelInitialisation& params ) {

    // Get the nutrition source (herbivory, carnivory or omnivory) of the acting cohort
    string NutritionSource = params.mCohortFunctionalGroupDefinitions.GetTraitNames( "Nutrition source", actingCohort.mFunctionalGroupIndex );
    map<string, int> vores;
    vores["herbivore"] = 0;
    vores["carnivore"] = 1;
    vores["omnivore" ] = 2;

    // Switch to the appropriate eating process(es) given the cohort's nutrition source
    switch( vores[ NutritionSource ] ) {
        case 0://"herbivore":

            // Get the assimilation efficiency for herbivory for this cohort from the functional group definitions
            mImplementations[ "revised herbivory" ]->AssimilationEfficiency = params.mCohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup( "herbivory assimilation", actingCohort.mFunctionalGroupIndex );

            // Get the proportion of time spent eating for this cohort from the functional group definitions
            mImplementations[ "revised herbivory" ]->ProportionTimeEating = actingCohort.mProportionTimeActive;

            // Calculate the potential biomass available from herbivory
            if( gcl.IsMarine( ) )
                mImplementations[ "revised herbivory" ]->GetEatingPotentialMarine( gcl, actingCohort, params );
            else
                mImplementations[ "revised herbivory" ]->GetEatingPotentialTerrestrial( gcl, actingCohort, params );

            // Run herbivory to apply changes in autotroph biomass from herbivory and add biomass eaten to the delta arrays
            mImplementations[ "revised herbivory" ]->RunEating( gcl, actingCohort, currentTimestep, params );

            break;

        case 1://"carnivore":

            // Get the assimilation efficiency for predation for this cohort from the functional group definitions
            mImplementations[ "revised predation" ]->AssimilationEfficiency = params.mCohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup( "carnivory assimilation", actingCohort.mFunctionalGroupIndex );
            mImplementations[ "revised predation" ]->ProportionTimeEating = actingCohort.mProportionTimeActive;

            // Calculate the potential biomass available from predation
            if( gcl.IsMarine( ) )
                mImplementations[ "revised predation" ]->GetEatingPotentialMarine( gcl, actingCohort, params );
            else
                mImplementations[ "revised predation" ]->GetEatingPotentialTerrestrial( gcl, actingCohort, params );
            // Run predation to apply changes in prey biomass from predation and add biomass eaten to the delta arrays
            mImplementations[ "revised predation" ]->RunEating( gcl, actingCohort, currentTimestep, params );

            break;

        case 2://"omnivore":

            // Get the assimilation efficiency for predation for this cohort from the functional group definitions 
            mImplementations[ "revised predation" ]->AssimilationEfficiency = params.mCohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup( "carnivory assimilation", actingCohort.mFunctionalGroupIndex );

            // Get the assimilation efficiency for herbivory for this cohort from the functional group definitions
            mImplementations[ "revised herbivory" ]->AssimilationEfficiency = params.mCohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup( "herbivory assimilation", actingCohort.mFunctionalGroupIndex );

            // Get the proportion of time spent eating and assign to both the herbivory and predation implementations
            //                   double ProportionTimeEating = actingCohort.ProportionTimeActive;
            mImplementations[ "revised predation" ]->ProportionTimeEating = actingCohort.mProportionTimeActive;
            mImplementations[ "revised herbivory" ]->ProportionTimeEating = actingCohort.mProportionTimeActive;

            // Calculate the potential biomass available from herbivory
            if( gcl.IsMarine( ) )
                mImplementations[ "revised herbivory" ]->GetEatingPotentialMarine( gcl, actingCohort, params );
            else
                mImplementations[ "revised herbivory" ]->GetEatingPotentialTerrestrial( gcl, actingCohort, params );

            // Calculate the potential biomass available from predation
            if( gcl.IsMarine( ) )
                mImplementations[ "revised predation" ]->GetEatingPotentialMarine( gcl, actingCohort, params );
            else
                mImplementations[ "revised predation" ]->GetEatingPotentialTerrestrial( gcl, actingCohort, params );

            // Calculate the total handling time for all expected kills from predation and expected plant matter eaten in herbivory
            mTotalTimeToEatForOmnivores = mImplementations[ "revised herbivory" ]->TimeUnitsToHandlePotentialFoodItems + mImplementations[ "revised predation" ]->TimeUnitsToHandlePotentialFoodItems;

            // Assign this total time to the relevant variables in both herbviory and predation, so that actual amounts eaten are calculated correctly
            mImplementations[ "revised herbivory" ]->TimeUnitsToHandlePotentialFoodItems = mTotalTimeToEatForOmnivores;
            mImplementations[ "revised predation" ]->TimeUnitsToHandlePotentialFoodItems = mTotalTimeToEatForOmnivores;

            // Run predation to update prey cohorts and delta biomasses for the acting cohort
            mImplementations[ "revised predation" ]->RunEating( gcl, actingCohort, currentTimestep, params );

            // Run herbivory to update autotroph biomass and delta biomasses for the acting cohort
            mImplementations[ "revised herbivory" ]->RunEating( gcl, actingCohort, currentTimestep, params );

            break;

        default:
            // For nutrition source that are not supported, throw an error
            Logger::Get( )->LogMessage( "The model currently does not contain an eating model for nutrition source:" + NutritionSource );
            exit( 1 );
            break;

    }
    // Check that the biomasses from predation and herbivory in the deltas is a number
    assert( !std::isnan( Cohort::mMassFluxes["biomass"]["predation"] ) && "BiomassFromEating is NaN" );
    assert( !std::isnan( Cohort::mMassFluxes["biomass"]["herbivory"] ) && "BiomassFromEating is NaN" );
}
