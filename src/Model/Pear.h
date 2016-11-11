#ifndef PEAR
#define	PEAR

#include "CohortMerge.h"

class Pear { // FIX - Rename this class?
public:
    Pear( );
    Pear( Cohort*, Cohort*, double );

    Cohort* mCohortA;
    Cohort* mCohortB;
    double mDistance;

    struct pearComparator {
        bool operator()( const Pear& u, const Pear& v ) {
            return (u.mDistance < v.mDistance );
        }
    };
};

#endif

