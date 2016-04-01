#ifndef APPLYECOLOGY_H
#define APPLYECOLOGY_H
//#include <ProcessTracker.h>
#include <assert.h>

#include "Cohort.h"
/** \file ApplyEcology.h
 * \brief the ApplyEcology header file
 */

/** \brief Class for applying changes from the ecological processes to the properties of the acting cohort and to the environment */
class ApplyEcology {
public:
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief  Apply all updates from the ecological processes to the properties of the acting cohort and to the environment
    @param gcl The gridCell 
    @param actingCohort The acting cohort 
    @param currentTimestep The current model time step 
    @param tracker A process tracker */
    void UpdateAllEcology( GridCell& gcl, Cohort& actingCohort, unsigned currentTimestep ) {
        // Apply cohort abundance changes
        UpdateAbundance( gcl, actingCohort );
        // Apply cohort biomass changes
        UpdateBiomass( gcl, actingCohort, currentTimestep );
        // Apply changes to the environmental biomass pools
        UpdatePools( gcl );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief  Update the abundance of the acting cohort according to the delta abundances from the ecological processes
    @param gcl The current grid cell 
    @param actingCohort The acting cohort 
     */
    void UpdateAbundance( GridCell& gcl, Cohort& actingCohort ) {

        // Variable to calculate net abundance change to check that cohort abundance will not become negative
        double NetAbundanceChange = 0.0;

        // Loop over all abundance deltas
        for( auto& d: Cohort::mMassFluxes["abundance"] ) {
            // Update net abundance change
            NetAbundanceChange += d.second;
        }
        // Check that cohort abundance will not become negative
        assert( ( actingCohort.mCohortAbundance + NetAbundanceChange ) >= 0 && "Cohort abundance < 0" );

        //Loop over all keys in the abundance deltas sorted list
        for( auto& d: Cohort::mMassFluxes["abundance"] ) {
            // Update the abundance of the acting cohort
            actingCohort.mCohortAbundance += d.second;
        }

    }
    //----------------------------------------------------------------------------------------------

    /** \brief  Update the individual and reproductive body masses of the acting cohort according to the delta biomasses from the ecological processes
    @param gridCell The current grid cell 
    @param actingCohort The acting cohort  
    @param currentTimestep The current model time step 
     */
    void UpdateBiomass( GridCell& gcl, Cohort& actingCohort, unsigned currentTimestep ) {

        // Variable to calculate net biomass change to check that cohort individual body mass will not become negative
        double NetBiomass = 0.0;

        // Loop over all biomass deltas
        for( auto& d: Cohort::mMassFluxes["biomass"] ) {
            // Update net biomass change
            NetBiomass += d.second;
        }
        double BiomassCheck = 0.0;
        bool NetToBeApplied = true;
        // If cohort abundance is greater than zero, then check that the calculated net biomass will not make individual body mass become negative
        if( actingCohort.mCohortAbundance > 0 ) {

            BiomassCheck = actingCohort.mIndividualBodyMass + NetBiomass;
            if( BiomassCheck < 0 ) {
                cout << "Biomass going negative, acting cohort: " << actingCohort.mFunctionalGroupIndex << ", " << actingCohort.mID;
                exit( 1 );
            }
        }

        //Loop over all keys in the abundance deltas sorted list
        for( auto& d: Cohort::mMassFluxes["biomass"] ) {
            // If cohort abundance is zero, then set cohort individual body mass to zero and reset the biomass delta to zero, 
            // otherwise update cohort individual body mass and reset the biomass delta to zero
            if( actingCohort.mCohortAbundance == 0 ) {
                actingCohort.mIndividualBodyMass = 0.0;
            } else {
                if( NetToBeApplied ) {
                    actingCohort.mIndividualBodyMass = actingCohort.mIndividualBodyMass + NetBiomass;
                    NetToBeApplied = false;
                }
            }
        }

        // Check that individual body mass is still greater than zero
        assert( actingCohort.mIndividualBodyMass >= 0 && "biomass < 0" );

        // If the current individual body mass is the largest that has been achieved by this cohort, then update the maximum achieved
        // body mass tracking variable for the cohort
        if( actingCohort.mIndividualBodyMass > actingCohort.mMaximumAchievedBodyMass )
            actingCohort.mMaximumAchievedBodyMass = actingCohort.mIndividualBodyMass;

        // Variable to calculate net reproductive biomass change to check that cohort individual body mass will not become negative
        double NetReproductiveBiomass = 0.0;

        // Loop over all reproductive biomass deltas
        for( auto& d: Cohort::mMassFluxes["reproductivebiomass"] ) {
            // Update net reproductive biomass change
            NetReproductiveBiomass += d.second;
        }

        //Loop over all keys in the abundance deltas sorted list
        for( auto& d: Cohort::mMassFluxes["reproductivebiomass"] ) {
            // If cohort abundance is zero, then set cohort reproductive body mass to zero and reset the biomass delta to zero, 
            // otherwise update cohort reproductive body mass and reset the biomass delta to zero
            if( actingCohort.mCohortAbundance == 0 ) {
                actingCohort.mIndividualReproductivePotentialMass = 0.0;
            } else {
                actingCohort.mIndividualReproductivePotentialMass += d.second;
            }
        }
        //Note that maturity time step is set in TReproductionBasic
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Update the organic and respiratory biomass pools according to the relevant deltas from the ecological processes

    @param gcl The current grid cell */
    void UpdatePools( GridCell& gcl ) {

        // Loop over all keys in the organic pool deltas sorted list
        for( auto &D: Cohort::mMassFluxes["organicpool"] ) {
            // Check that the delta value is not negative
            if( D.second < 0 )cout << "organic pool " << D.first << " " << D.second << endl;
            //assert(D.second >= 0.0 && "A delta value for the organic pool is negative " );
            // Update the organic pool biomass
            Environment::Get( "Organic Pool", gcl ) += D.second;
            //Reset the delta value to zero
        }

        // Loop over all keys in the respiratory pool deltas sorted list
        for( auto &D: Cohort::mMassFluxes["respiratoryCO2pool"] ) {
            // Check that the delta value is not negative
            assert( D.second >= 0.0 && "A delta value for the respiratory CO2 pool is negative" );
            // Update the respiratory CO2 pool
            Environment::Get( "Respiratory CO2 Pool", gcl ) += D.second;

            // Reset the delta value to zero
        }
    }
    //----------------------------------------------------------------------------------------------
};
#endif
