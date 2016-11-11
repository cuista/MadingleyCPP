#ifndef PEAR
#define	PEAR

#include "CohortMerge.h"

class Pear {
public:
    Pear( );
    Pear( Cohort* _a, Cohort* _b, double r );

    Cohort* a;
    Cohort* b;
    double dist;

    struct pearComparator {
        bool operator()( const Pear& u, const Pear& v ) {
            return (u.dist < v.dist );
        }
    };
};

#endif

