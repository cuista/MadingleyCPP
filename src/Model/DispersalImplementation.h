#ifndef DISPERSALIMPLEMENTATION
#define DISPERSALIMPLEMENTATION

#include "ModelGrid.h"
#include "Cohort.h"
#include "NonStaticSimpleRNG.h"

/** \brief Base Class for implementations of the ecological process of dispersal */
class DispersalImplementation {
public:
    DispersalImplementation( );

    /** \brief Run the dispersal implementation */
    virtual void RunDispersal( ModelGrid&, Cohort&, const unsigned& );

    void ResetRandom( );

    void newCell( ModelGrid&, double&, double&, double&, double&, Cohort& );

    /** \brief Include Utility class */
    UtilityFunctions mUtilities;
    /** \brief An instance of the simple random number generator class */
    NonStaticSimpleRNG mRandomNumber1;
    NonStaticSimpleRNG mRandomNumber2;
};

#endif
