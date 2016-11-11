#include "EcologyCohort.h"

EcologyCohort::EcologyCohort( ) {
    // Declare and attach eating formulations
    Eating *EatingFormulation = new Eating( Parameters::Get( )->GetTimeStepUnits( ) );
    mEatingFormulations["Basic eating"] = EatingFormulation;
    // Declare and attach metabolism formulations
    Metabolism *MetabolismFormulation = new Metabolism( Parameters::Get( )->GetTimeStepUnits( ) );
    mMetabolismFormulations["Basic metabolism"] = MetabolismFormulation;
    // Declare and attach mortality formulations
    Reproduction *ReproductionFormulation = new Reproduction( Parameters::Get( )->GetTimeStepUnits( ), Parameters::Get( )->GetDrawRandomly( ) );
    mReproductionFormulations["Basic reproduction"] = ReproductionFormulation;
    // Declare and attach mortality formulations
    Mortality *MortalityFormulation = new Mortality( Parameters::Get( )->GetTimeStepUnits( ) );
    mMortalityFormulations["Basic mortality"] = MortalityFormulation;
}

void EcologyCohort::InitialiseEating( GridCell& gcl, MadingleyModelInitialisation& params ) {
    // Initialise eating formulations - has to be redone every step?
    mEatingFormulations["Basic eating"]->InitializeEcologicalProcess( gcl, params, "revised predation" );
    mEatingFormulations["Basic eating"]->InitializeEcologicalProcess( gcl, params, "revised herbivory" );
}

EcologyCohort::~EcologyCohort( ) {
    delete mEatingFormulations["Basic eating"];
    delete mMetabolismFormulations["Basic metabolism"];
    delete mMortalityFormulations["Basic mortality"];
    delete mReproductionFormulations["Basic reproduction"];
}

void EcologyCohort::RunWithinCellEcology( GridCell& gcl, Cohort& actingCohort, unsigned currentTimestep, ThreadLockedParallelVariables& partial, unsigned currentMonth, MadingleyModelInitialisation& params ) {
    // RUN EATING
    if( actingCohort.mIndividualBodyMass > 0 ) {
        mEatingFormulations["Basic eating"]->RunEcologicalProcess( gcl, actingCohort, currentTimestep, partial, currentMonth, params );
        // RUN METABOLISM - THIS TIME TAKE THE METABOLIC LOSS TAKING INTO ACCOUNT WHAT HAS BEEN INGESTED THROUGH EATING
        mMetabolismFormulations["Basic metabolism"]->RunEcologicalProcess( gcl, actingCohort, currentTimestep, partial, currentMonth, params );
        // RUN REPRODUCTION - TAKING INTO ACCOUNT NET BIOMASS CHANGES RESULTING FROM EATING AND METABOLISING
        mReproductionFormulations["Basic reproduction"]->RunEcologicalProcess( gcl, actingCohort, currentTimestep, partial, currentMonth, params );
        // RUN MORTALITY - TAKING INTO ACCOUNT NET BIOMASS CHANGES RESULTING FROM EATING, METABOLISM AND REPRODUCTION
        mMortalityFormulations["Basic mortality"]->RunEcologicalProcess( gcl, actingCohort, currentTimestep, partial, currentMonth, params );
    }
}

void EcologyCohort::UpdateEcology( GridCell& gcl, Cohort& actingCohort, unsigned currentTimestep ) {
    // Apply the results of within-cell ecological processes
    mApplyEcologicalProcessResults.UpdateAllEcology( gcl, actingCohort, currentTimestep );
}

