#include "Metabolism.h"

Metabolism::Metabolism( std::string globalModelTimeStepUnit ) {
    // Add the basic endotherm metabolism implementation to the list of implementations
    MetabolismEndotherm* MetabolismEndothermImplementation = new MetabolismEndotherm( globalModelTimeStepUnit );
    Implementations[ "basic endotherm" ] = MetabolismEndothermImplementation;

    // Add the basic ectotherm metabolism implementation to the list of implementations
    MetabolismEctotherm* MetabolismEctothermImplementation = new MetabolismEctotherm( globalModelTimeStepUnit );
    Implementations[ "basic ectotherm" ] = MetabolismEctothermImplementation;
}

Metabolism::~Metabolism( ) {
    delete Implementations[ "basic endotherm" ];
    delete Implementations[ "basic ectotherm" ];
}

void Metabolism::InitializeEcologicalProcess( GridCell& gcl, MadingleyInitialisation& params, std::string implementationKey ) {

}

void Metabolism::RunEcologicalProcess( GridCell& gcl, Cohort& actingCohort, unsigned currentTimestep, ThreadLockedParallelVariables& partial, unsigned currentMonth, MadingleyInitialisation& params ) {
    if( params.mCohortFunctionalGroupDefinitions.GetTraitNames( "Heterotroph/Autotroph", actingCohort.mFunctionalGroupIndex ) == "heterotroph" ) {
        if( params.mCohortFunctionalGroupDefinitions.GetTraitNames( "Endo/Ectotherm", actingCohort.mFunctionalGroupIndex ) == "endotherm" ) {
            Implementations[ "basic endotherm" ]->RunMetabolism( actingCohort, currentTimestep, currentMonth );
        } else {
            Implementations[ "basic ectotherm" ]->RunMetabolism( actingCohort, currentTimestep, currentMonth );
        }
    }
}
