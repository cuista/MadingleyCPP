#include "Pear.h"

Pear::Pear( ) {

}

Pear::Pear( Cohort* _a, Cohort* _b, double r ) {
    if( r > 0.5 ) {
        a = _a;
        b = _b;
    } else {
        a = _b;
        b = _a;
    }
    dist = CohortMerge::CalculateDistance( *a, *b );
}


