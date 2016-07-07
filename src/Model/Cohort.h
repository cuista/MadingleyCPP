#ifndef COHORT_H
#define COHORT_H

#include "Types.h"

/** \file Cohort.h
 * \brief the Cohort header file
 */

/** \brief Class to hold properties of a single cohort */

class Cohort {
public:
    /** \brief Time step when the cohort was generated */
    unsigned mBirthTimeStep;
    /** \brief The time step at which this cohort reached maturity */
    unsigned mMaturityTimeStep;
    /** \brief A list of all cohort IDs ever associated with individuals in this current cohort */
    Types::LongVector mCohortID;
    /** \brief The mean juvenile mass of individuals in this cohort */
    double mJuvenileMass;
    /** \brief The mean mature adult mass of individuals in this cohort */
    double mAdultMass;
    /** \brief The mean body mass of an individual in this cohort */
    double mIndividualBodyMass;
    /** \brief Individual biomass assigned to reproductive potential */
    double mIndividualReproductivePotentialMass;
    /** \brief The maximum mean body mass ever achieved by individuals in this cohort */
    double mMaximumAchievedBodyMass;
    /** \brief The number of individuals in the cohort */
    double mCohortAbundance;
    /** \brief The index of the functional group that the cohort belongs to */
    unsigned mFunctionalGroupIndex;
    /** \brief Whether this cohort has ever been merged with another cohort */
    bool mMerged;
    /** \brief The proportion of the timestep for which this cohort is active */
    double mProportionTimeActive;
    /** \brief The optimal prey body size for individuals in this cohort */
    double mLogOptimalPreyBodySizeRatio;
    long long mID;
    GridCell* mCurrentLocation;
    GridCell* mDestination;
    static Types::Double2DMap mMassFluxes;
    static Types::CohortVector mNewCohorts;
    static unsigned mNextID;
    
    /** \brief Constructor for the Cohort class: assigns cohort starting properties at beginning of model run
    @param gcl The grid cell that holds this cohort 
    @param juvenileBodyMass The mean juvenile body mass of individuals in the cohort 
    @param adultBodyMass The mean mature adult body mass of individuals in the cohort 
    @param initialBodyMass The intial mean body mass of individuals in this cohort 
    @param initialAbundance The intial number of individuals in this cohort 
    @param optimalPreyBodySizeRatio The optimal prey body mass (as a percentage of this cohorts mass) for individuals in this cohort 
    @param birthTimeStep The birth time step for this cohort 
    @param nextCohortID The unique ID to assign to the next cohort created    */
    Cohort( GridCell&, unsigned, double, double, double, double, double, unsigned short, double, long long & );

    /** \brief Constructor for the Cohort class: assigns cohort starting properties on reproduction
    @param actingCohort The parent of this cohort
    @param p track position of this cohort in the list held in the cell 
    @param juvenileBodyMass The mean juvenile body mass of individuals in the cohort 
    @param adultBodyMass The mean mature adult body mass of individuals in the cohort 
    @param initialBodyMass The intial mean body mass of individuals in this cohort 
    @param initialAbundance The intial number of individuals in this cohort 
    @param birthTimeStep The birth time step for this cohort 
    @param nextCohortID The unique ID to assign to the next cohort created    */
    Cohort( Cohort&, double, double, double, double, unsigned, long long& );
    
    GridCell& GetCurrentLocation( );
    GridCell& GetDestination();
    void SetCurrentLocation( Types::GridCellPointer );
    
    bool IsMoving( );
    bool IsMature( );
    bool IsMarine( );
    bool IsPlanktonic( MadingleyModelInitialisation& );
    std::string DispersalType( MadingleyModelInitialisation& );
    double Realm( );
    void TryLivingAt( Types::GridCellPointer );
    void Move( );
    static void ResetMassFluxes( );
};

#endif
