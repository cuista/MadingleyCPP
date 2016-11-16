#include "Pear.h"

Pear::Pear( ) {
    mCohortA = NULL;
    mCohortB = NULL;
    mDistance = 0;
}

Pear::Pear( Cohort* cohortA, Cohort* cohortB, double r ) {
    if( r > 0.5 ) {
        mCohortA = cohortA;
        mCohortB = cohortB;
    } else {
        mCohortA = cohortB;
        mCohortB = cohortA;
    }
    mDistance = CohortMerger::CalculateDistance( *mCohortA, *mCohortB );
}


