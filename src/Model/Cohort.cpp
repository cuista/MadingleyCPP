#include <Cohort.h>
#include <limits.h>
#include <GridCell.h>
#include <MadingleyModelInitialisation.h>
#include <Dispersal.h>

#include "Parameters.h"

unsigned Cohort::mNextID = 0;
Types::CohortVector Cohort::mNewCohorts;
Types::Double2DMap Cohort::mMassFluxes;

Cohort::Cohort( GridCell& gcl, unsigned functionalGroupIndex, double juvenileBodyMass, double adultBodyMass, double initialBodyMass, double initialAbundance, double optimalPreyBodySizeRatio, unsigned short birthTimeStep, double proportionTimeActive, long long &nextCohortID ) {

    mFunctionalGroupIndex = functionalGroupIndex;
    mJuvenileMass = juvenileBodyMass;
    mAdultMass = adultBodyMass;
    mIndividualBodyMass = initialBodyMass;
    mCohortAbundance = initialAbundance;
    mBirthTimeStep = birthTimeStep;
    mMaturityTimeStep = std::numeric_limits<unsigned>::max( );
    mLogOptimalPreyBodySizeRatio = log( optimalPreyBodySizeRatio );
    mMaximumAchievedBodyMass = juvenileBodyMass;
    mMerged = false;
    mProportionTimeActive = proportionTimeActive;
    mCell = &gcl;
    mDestination = mCell;
    mPlace.setIndices(gcl.GetLatitudeIndex(),gcl.GetLongitudeIndex());
    mDest=mPlace;
    
    mIndividualReproductivePotentialMass= (double)0.;

    mID = mNextID; //MB added to track this object.

    mNextID++;
    nextCohortID++;
}

Cohort::Cohort( Cohort& actingCohort, double juvenileBodyMass, double adultBodyMass, double initialBodyMass, double initialAbundance, unsigned birthTimeStep, long long& nextCohortID ) {

    mFunctionalGroupIndex = actingCohort.mFunctionalGroupIndex;
    mJuvenileMass = juvenileBodyMass;
    mAdultMass = adultBodyMass;
    mIndividualBodyMass = initialBodyMass;
    mCohortAbundance = initialAbundance;
    mBirthTimeStep = birthTimeStep;
    mMaturityTimeStep = std::numeric_limits<unsigned>::max( );
    mLogOptimalPreyBodySizeRatio = actingCohort.mLogOptimalPreyBodySizeRatio;
    mMaximumAchievedBodyMass = juvenileBodyMass;
    mMerged = false;
    mProportionTimeActive = actingCohort.mProportionTimeActive;
    mCell = actingCohort.mCell;
    mPlace= actingCohort.mPlace;
    mDestination = mCell;
    mDest=actingCohort.mDest;
    mIndividualReproductivePotentialMass= (double)0.;
    mID = mNextID; //MB added to track this object.
    mNextID++;
    nextCohortID++;

}

bool Cohort::IsMature( ) {
    return (mMaturityTimeStep < std::numeric_limits<unsigned>::max( ) );
}

void Cohort::ResetMassFluxes( ) {
    // Initialize delta abundance sorted list with appropriate processes

    mMassFluxes["abundance"]["mortality"] = 0.0;

    // Initialize delta biomass sorted list with appropriate processes
    mMassFluxes["biomass"]["metabolism"] = 0.0;
    mMassFluxes["biomass"]["predation"] = 0.0;
    mMassFluxes["biomass"]["herbivory"] = 0.0;
    mMassFluxes["biomass"]["reproduction"] = 0.0;

    // Initialize delta reproductive biomass vector with appropriate processes

    mMassFluxes["reproductivebiomass"]["reproduction"] = 0.0;

    // Initialize organic pool delta vector with appropriate processes
    mMassFluxes["organicpool"]["herbivory"] = 0.0;
    mMassFluxes["organicpool"]["predation"] = 0.0;
    mMassFluxes["organicpool"]["mortality"] = 0.0;

    // Initialize respiratory CO2 pool delta vector with appropriate processes
    mMassFluxes["respiratoryCO2pool"]["metabolism"] = 0.0;
} 

double Cohort::Realm( ) {
    return mCell->Realm( );
}

void Cohort::TryLivingAt( Types::GridCellPointer destination, location& L ) {
    if( destination != 0 && destination->Realm( ) == Realm( ) ) {mDestination = destination;mDest=L;}
}
GridCell& Cohort::GetDestination(){
    return *mDestination;
}
GridCell& Cohort::GetCurrentCell( ) {
    return *mCell;
}
void Cohort::SetCurrentCell( Types::GridCellPointer gclp ) {
    mCell = gclp;
}

bool Cohort::IsMoving( ) {
    return mCell != mDestination;
}

void Cohort::Move( ) {
    mCell->MoveCohort( *this );
    mDestination=mCell;
}

bool Cohort::IsMarine( ) {
    return mCell->IsMarine( );
}

bool Cohort::IsPlanktonic( MadingleyModelInitialisation& params ) {
    return ( IsMarine( ) && ( ( mIndividualBodyMass <= Parameters::Get( )->GetPlanktonSizeThreshold( ) ) || ( params.mCohortFunctionalGroupDefinitions.GetTraitNames( "Mobility", mFunctionalGroupIndex ) == "planktonic" ) ) );
}

string Cohort::DispersalType( MadingleyModelInitialisation& params ) {
    string dispersalName;
    if( IsPlanktonic( params ) ) {
        // Advective dispersal
        dispersalName = "advective";
    }// Otherwise, if mature do responsive dispersal
    else if( IsMature( ) ) {
        dispersalName = "responsive";
    }// If the cohort is immature, run diffusive dispersal
    else {
        dispersalName = "diffusive";
    }
    return dispersalName;
}
