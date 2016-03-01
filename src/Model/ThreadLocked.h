/* 
 * File:   ThreadLocked.h
 * Author: mb425
 *
 * Created on 02 October 2015, 11:25
 */

/** \brief Hold some variables that need protection in original multi-threaded code */
#ifndef THREADLOCKED_H
#define	THREADLOCKED_H

class ThreadLockedParallelVariables {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------
    /** \brief Thread-local variable to track the extinction of cohorts */
    int Extinctions;
    /** \brief Thread-local variable to track the production of cohorts */
    int Productions;
    /** \brief Variable to track the number of cohorts combined */
    int Combinations;
    //----------------------------------------------------------------------------------------------
    /** \brief Thread-locked variable to track the cohort ID to assign to newly produced cohorts */
    long long NextCohortIDThreadLocked;
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------
    /** This class just needs a constructor*/
    ThreadLockedParallelVariables(int E, int P, int C, long long N) : Extinctions(E), Productions(P), Combinations(C), NextCohortIDThreadLocked(N) {
        ;
    }
};
#endif	/* THREADLOCKED_H */

