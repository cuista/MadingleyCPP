#ifndef COHORTMERGE_H
#define COHORTMERGE_H
#include <Cohort.h>
#include <set>

#include "MadingleyModelInitialisation.h"
/** \file CohortMerge.h
 * \brief The CohortMerge header file
 */
//namespace Madingley
//{

/** \brief Merges cohorts with similar properties
 */

class CohortMerge {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief An instance of the simple random number generator class */
    std::default_random_engine RandomNumberGenerator;

    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------
    /** \brief Set the seed for the random number generator
    @param DrawRandomly  
     */
    void SetRandom(bool DrawRandomly) {

        if (DrawRandomly) {
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            RandomNumberGenerator.seed(seed);
        } else {
            RandomNumberGenerator.seed(4000);
        }
    }
    //----------------------------------------------------------------------------------------------
    /** \brief Calculate the distance between two cohorts in multi-dimensional trait space (body mass, adult mass, juvenile mass)
    @param Cohort1 The first cohort to calculate distance to 
    @param Cohort2 The cohort to compare to 
    @returns The relative distance in trait space
     */
    static double CalculateDistance(Cohort& Cohort1, Cohort& Cohort2) {
        double AdultMassDistance = (Cohort1.AdultMass - Cohort2.AdultMass) / Cohort1.AdultMass;
        double JuvenileMassDistance = (Cohort1.JuvenileMass - Cohort2.JuvenileMass) / Cohort1.JuvenileMass;
        double CurrentMassDistance = (Cohort1.IndividualBodyMass - Cohort2.IndividualBodyMass) / Cohort1.IndividualBodyMass;

        return ((AdultMassDistance * AdultMassDistance) + (JuvenileMassDistance * JuvenileMassDistance) +
                (CurrentMassDistance * CurrentMassDistance));
    }
    //----------------------------------------------------------------------------------------------
    class Pear{
    public:
        Cohort *a,*b;
        double dist;
        Pear();
        Pear(Cohort* _a,Cohort* _b, double r){
            if (r>0.5){a=_a;b=_b;} else {a=_b;b=_a;}
            dist=CohortMerge::CalculateDistance(*a,*b);
        }
    };
    struct pearComparator {
        bool operator()(const Pear& u,const Pear& v) {
            return (u.dist < v.dist);
        }
    };
   
    //----------------------------------------------------------------------------------------------
/** \brief
    Merge cohorts until below a specified threshold number of cohorts in each grid cell

    @param gridCellCohorts The cohorts within this grid cell 
    @param TotalNumberOfCohorts The total number of cohorts in this grid cell 
    @param TargetCohortThreshold The target threshold to reduce the number of cohorts to 
    @return The number of cohorts that have been merged
     */
    int MergeToReachThresholdFast(GridCell& gcl, MadingleyModelInitialisation& params) {

        // Set of lists of shortest distances in each functional group
        // set is automatically sorted - multiset allows for elements with the same distance
        multiset< Pear, pearComparator > SortedDistances;
        // How many cohorts to remove to hit the threshold
        unsigned MergeCounter = 0;
        std::uniform_real_distribution<double> randomNumber(0.0, 1.0);

        int NumberToRemove = gcl.GetNumberOfCohorts() - params.MaxNumberOfCohorts;
        if (NumberToRemove > 0) {

            //Loop through functional groups
            for (unsigned ff = 0; ff < gcl.GridCellCohorts.size(); ff++) {
                if (gcl.GridCellCohorts[ff].size() > 1) {
                    // Loop through cohorts within functional groups
                    for (int cc = 0; cc < gcl.GridCellCohorts[ff].size() - 1; cc++) {
                        // Loop through comparison cohorts
                        for (int dd = cc + 1; dd < gcl.GridCellCohorts[ff].size(); dd++) {
                            Pear PairwiseDistance(&gcl.GridCellCohorts[ff][cc], &gcl.GridCellCohorts[ff][dd],randomNumber(RandomNumberGenerator));
                            SortedDistances.insert(PairwiseDistance);
                        }
                    }
                }
            }

            auto I = SortedDistances.begin();
            while (MergeCounter < NumberToRemove && I != SortedDistances.end()) {
                Cohort& CohortToMergeFrom = *(I->a);
                Cohort& CohortToMergeTo = *(I->b);

                if (CohortToMergeFrom.CohortAbundance > 0 && CohortToMergeTo.CohortAbundance > 0) {
                    // Add the abundance of the second cohort to that of the first

                    CohortToMergeTo.CohortAbundance += CohortToMergeFrom.CohortAbundance * CohortToMergeFrom.IndividualBodyMass / CohortToMergeTo.IndividualBodyMass;
                    // Add the reproductive potential mass of the second cohort to that of the first
                    CohortToMergeTo.IndividualReproductivePotentialMass += CohortToMergeFrom.IndividualReproductivePotentialMass * CohortToMergeFrom.CohortAbundance / CohortToMergeTo.CohortAbundance;
                    // Set the abundance of the second cohort to zero
                    CohortToMergeFrom.CohortAbundance = 0.0;
                    // Designate both cohorts as having merged
                    CohortToMergeTo.Merged = true;
                    CohortToMergeFrom.Merged = true;
                    MergeCounter++;
                }
                ++I;
            }
        }

        return MergeCounter;

    }
    //----------------------------------------------------------------------------------------------
    };

#endif
