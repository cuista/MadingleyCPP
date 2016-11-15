#ifndef DISPERSALIMPLEMENTATION
#define DISPERSALIMPLEMENTATION

#include "Grid.h"
#include "Cohort.h"
#include "NonStaticSimpleRNG.h"

/** \brief Base Class for implementations of the ecological process of dispersal */
class DispersalImplementation {
public:
    DispersalImplementation( );

    /** \brief Run the dispersal implementation */
    virtual void RunDispersal( Grid&, Cohort&, const unsigned& );

    void ResetRandom( );

    void newCell( Grid&, double&, double&, double&, double&, Cohort& );

    /** \brief Include Utility class */
    UtilityFunctions mUtilities;
    /** \brief An instance of the simple random number generator class */
    NonStaticSimpleRNG mRandomNumber1;
    NonStaticSimpleRNG mRandomNumber2;
};

#endif
