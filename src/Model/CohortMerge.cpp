#include "CohortMerge.h"
#include "Pear.h"

CohortMerge::CohortMerge( ) {
    if( Parameters::Get( )->GetDrawRandomly( ) == true ) {
        unsigned seed = std::chrono::system_clock::now( ).time_since_epoch( ).count( );
        mRandomNumber.SetSeed( seed );
    } else {
        mRandomNumber.SetSeed( 4000 );
    }
}
//----------------------------------------------------------------------------------------------

/** \brief Calculate the distance between two cohorts in multi-dimensional trait space (body mass, adult mass, juvenile mass)
@param Cohort1 The first cohort to calculate distance to 
@param Cohort2 The cohort to compare to 
@returns The relative distance in trait space
 */
double CohortMerge::CalculateDistance( Cohort& cohort1, Cohort& cohort2 ) {
    double AdultMassDistance = ( cohort1.mAdultMass - cohort2.mAdultMass ) / cohort1.mAdultMass;
    double JuvenileMassDistance = ( cohort1.mJuvenileMass - cohort2.mJuvenileMass ) / cohort1.mJuvenileMass;
    double CurrentMassDistance = ( cohort1.mIndividualBodyMass - cohort2.mIndividualBodyMass ) / cohort1.mIndividualBodyMass;

    return (( AdultMassDistance * AdultMassDistance ) + ( JuvenileMassDistance * JuvenileMassDistance ) + ( CurrentMassDistance * CurrentMassDistance ) );
}

int CohortMerge::MergeToReachThresholdFast( GridCell& gcl, MadingleyModelInitialisation& params ) {
    // Set of lists of shortest distances in each functional group
    // set is automatically sorted - multiset allows for elements with the same distance
    multiset< Pear, Pear::pearComparator > SortedDistances;
    // How many cohorts to remove to hit the threshold
    unsigned MergeCounter = 0;

    int NumberToRemove = gcl.GetNumberOfCohorts( ) - Parameters::Get( )->GetMaximumNumberOfCohorts( );

    if( NumberToRemove > 0 ) {
        //Loop through functional groups
        for( unsigned ff = 0; ff < gcl.mGridCellCohorts.size( ); ff++ ) {
            if( gcl.mGridCellCohorts[ff].size( ) > 1 ) {
                // Loop through cohorts within functional groups
                for( int cc = 0; cc < gcl.mGridCellCohorts[ff].size( ) - 1; cc++ ) {
                    // Loop through comparison cohorts
                    for( int dd = cc + 1; dd < gcl.mGridCellCohorts[ff].size( ); dd++ ) {
                        Pear PairwiseDistance( &gcl.mGridCellCohorts[ff][cc], &gcl.mGridCellCohorts[ff][dd], mRandomNumber.GetUniform( ) );
                        SortedDistances.insert( PairwiseDistance );
                    }
                }
            }
        }

        auto I = SortedDistances.begin( );
        while( MergeCounter < NumberToRemove && I != SortedDistances.end( ) ) {
            Cohort& CohortToMergeFrom = *( I->mCohortA );
            Cohort& CohortToMergeTo = *( I->mCohortB );

            if( CohortToMergeFrom.mCohortAbundance > 0 && CohortToMergeTo.mCohortAbundance > 0 ) {
                // Add the abundance of the second cohort to that of the first

                CohortToMergeTo.mCohortAbundance += CohortToMergeFrom.mCohortAbundance * CohortToMergeFrom.mIndividualBodyMass / CohortToMergeTo.mIndividualBodyMass;
                // Add the reproductive potential mass of the second cohort to that of the first
                CohortToMergeTo.mIndividualReproductivePotentialMass += CohortToMergeFrom.mIndividualReproductivePotentialMass * CohortToMergeFrom.mCohortAbundance / CohortToMergeTo.mCohortAbundance;
                // Set the abundance of the second cohort to zero
                CohortToMergeFrom.mCohortAbundance = 0.0;
                // Designate both cohorts as having merged
                CohortToMergeTo.mMerged = true;
                CohortToMergeFrom.mMerged = true;
                MergeCounter++;
            }
            ++I;
        }
    }
    return MergeCounter;
}
