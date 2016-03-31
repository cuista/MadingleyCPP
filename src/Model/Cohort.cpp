#include <Cohort.h>
#include <limits.h>
#include <GridCell.h>
#include <MadingleyModelInitialisation.h>
#include <Dispersal.h>

#include "Parameters.h"
/** \file Cohort.cc
 * \brief the Cohort implementation file
 */
unsigned Cohort::mNextID = 0;
//----------------------------------------------------------------------------------------------

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
    mLocation = &gcl;
    mDestination = mLocation;
    mID = mNextID; //MB added to track this object.
    mNextID++;
    nextCohortID++;
}
//----------------------------------------------------------------------------------------------

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
    mLocation = actingCohort.mLocation;
    mDestination = mLocation;
    mID = mNextID; //MB added to track this object.
    mNextID++;
    nextCohortID++;

}
//----------------------------------------------------------------------------------------------

bool Cohort::isMature( ) {
    return (mMaturityTimeStep < std::numeric_limits<unsigned>::max( ) );
}
//----------------------------------------------------------------------------------------------
Types::CohortVector Cohort::newCohorts;
Types::Double2DMap Cohort::Deltas;
//----------------------------------------------------------------------------------------------

void Cohort::zeroDeltas( ) {
    // Initialize delta abundance sorted list with appropriate processes

    Deltas["abundance"]["mortality"] = 0.0;

    // Initialize delta biomass sorted list with appropriate processes
    Deltas["biomass"]["metabolism"] = 0.0;
    Deltas["biomass"]["predation"] = 0.0;
    Deltas["biomass"]["herbivory"] = 0.0;
    Deltas["biomass"]["reproduction"] = 0.0;

    // Initialize delta reproductive biomass vector with appropriate processes

    Deltas["reproductivebiomass"]["reproduction"] = 0.0;

    // Initialize organic pool delta vector with appropriate processes
    Deltas["organicpool"]["herbivory"] = 0.0;
    Deltas["organicpool"]["predation"] = 0.0;
    Deltas["organicpool"]["mortality"] = 0.0;

    // Initialize respiratory CO2 pool delta vector with appropriate processes
    Deltas["respiratoryCO2pool"]["metabolism"] = 0.0;
}
//----------------------------------------------------------------------------------------------    

double Cohort::Realm( ) {
    return mLocation->Realm( );
}
//----------------------------------------------------------------------------------------------

void Cohort::TryLivingAt( GridCell* _destination ) {
    if( _destination != 0 && _destination->Realm( ) == Realm( ) )mDestination = _destination;
}
//----------------------------------------------------------------------------------------------

GridCell& Cohort::Here( ) {
    return *mLocation;
}

bool Cohort::isMoving( ) {
    return mLocation != mDestination;
}
//----------------------------------------------------------------------------------------------

void Cohort::Move( ) {
    mLocation->Move( *this );
}
//----------------------------------------------------------------------------------------------

bool Cohort::isMarine( ) {
    return mLocation->isMarine( );
}
//----------------------------------------------------------------------------------------------

bool Cohort::isPlanktonic( MadingleyModelInitialisation& params ) {
    return ( isMarine( ) && ( ( mIndividualBodyMass <= Parameters::Get( )->GetPlanktonSizeThreshold( ) ) || ( params.CohortFunctionalGroupDefinitions.GetTraitNames( "Mobility", mFunctionalGroupIndex ) == "planktonic" ) ) );
}
//----------------------------------------------------------------------------------------------

string Cohort::dispersalType( MadingleyModelInitialisation& params ) {
    string dispersalName;
    if( isPlanktonic( params ) ) {
        // Advective dispersal
        dispersalName = "advective";
    }// Otherwise, if mature do responsive dispersal
    else if( isMature( ) ) {
        dispersalName = "responsive";
    }// If the cohort is immature, run diffusive dispersal
    else {
        dispersalName = "diffusive";
    }
    return dispersalName;
}
