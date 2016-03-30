#ifndef ACTIVITY_H
#define ACTIVITY_H
#include <map>
#include <vector>
#include <FunctionalGroupDefinitions.h>
#include <Cohort.h>

#include "Maths.h"
/** \file Activity.h
 * \brief the Activity header file
 */

/** \brief Calculates the relative activity rate of a cohort */

class Activity {
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief The distance of the maximum critical temperature from the ambient temperature */
    double WarmingTolerance;
    /** \brief Distance of the optimal performance temperature from the ambient temperature */
    double ThermalSafetyMargin;
    /** \brief The optimal performance temperature */
    double Topt;
    /** \brief The maximum critical temperature */
    double CTmax;
    /** \brief The minimum critical temperature */
    double CTmin;
    /** \brief The ambient temperature */
    double AmbientTemp;
    /** \brief The diurnal temperature range */
    double DTR;

public:

    /** \brief Intercept of the linear relationship between warming tolerance of terrestrial ectotherms and annual temperature variability */
    double TerrestrialWarmingToleranceIntercept;
    /** \brief Slope of the linear relationship between warming tolerance of terrestrial ectotherms and annual temperature variability */
    double TerrestrialWarmingToleranceSlope;
    /** \brief Intercept of the linear relationship between terrestrial safety margin of terrestrial ectotherms and annual temperature variability */
    double TerrestrialTSMIntercept;
    /** \brief Slope of the linear relationship between terrestrial safety margin of terrestrial ectotherms and annual temperature variability */
    double TerrestrialTSMSlope;
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief Constructor for the Activity class: assigns parameter values */
    Activity( ) {
        // Initialise ecological parameters for predation
        InitialiseActivityParameters( );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Initialise parameters related to the activity of cohorts */
    void InitialiseActivityParameters( ) {
        // Source: Deutsch et al (2008), Impacts of climate warming on terrestrial ecototherms across latitude, PNAS.
        TerrestrialWarmingToleranceIntercept = 6.61;
        TerrestrialWarmingToleranceSlope = 1.6;
        TerrestrialTSMIntercept = 1.51;
        TerrestrialTSMSlope = 1.53;

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the proportion of time for which this cohort could be active and assign it to the cohort's properties
    @param gcl the current cell
    @param actingCohort The Cohort for which proportion of time active is being calculated 
    @param params the model parameters 
    @param currentTimestep Current timestep index 
     */
    void AssignProportionTimeActive( GridCell& gcl, Cohort& actingCohort, unsigned currentTimestep, unsigned currentMonth, MadingleyModelInitialisation& params ) {
        //Only work on heterotroph cohorts
        if( params.CohortFunctionalGroupDefinitions.GetTraitNames( "Heterotroph/Autotroph", actingCohort.FunctionalGroupIndex ) == "heterotroph" ) {
            //Check if this is an endotherm or ectotherm
            bool Endotherm = params.CohortFunctionalGroupDefinitions.GetTraitNames( "Endo/Ectotherm", actingCohort.FunctionalGroupIndex ) == "endotherm";
            if( Endotherm ) {
                //Assumes the whole timestep is suitable for endotherms to be active - actual time active is therefore the proportion specified for this functional group.
                actingCohort.ProportionTimeActive = params.CohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup( "proportion suitable time active", actingCohort.FunctionalGroupIndex );
            } else {
                //If ectotherm then use realm specific function
                if( !gcl.isMarine( ) ) {
                    actingCohort.ProportionTimeActive = CalculateProportionTimeSuitableTerrestrial( gcl, currentMonth, Endotherm ) *
                            params.CohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup( "proportion suitable time active", actingCohort.FunctionalGroupIndex );
                } else {
                    actingCohort.ProportionTimeActive = 1.0;
                }
            }
        }
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the proportion of each timestep for which this cohort is active
    For ectotherms: is a function of the critical max and min temperatures for this ectotherm cohort and also the ambient temperature and diurnal variation in this cell
    Assumes that the diurnal temperature range is symmetrical around the monthly mean temperature
    Alse assumes that the diurnal temperature profile is approximated by a sinusoidal time-series
    Source: Deutsch et al (2008), Impacts of climate warming on terrestrial ecototherms across latitude, PNAS.
    @param gcl The grid cell 
    @param currentMonth Currnent month in the model 
    @param endotherm Boolean indicating if cohort is endotherm or ectotherm (true if endotherm) 
    @return The proportion of the timestep for which this cohort could be active*/
    double CalculateProportionTimeSuitableTerrestrial( GridCell& gcl, unsigned currentMonth, bool endotherm ) {

        AmbientTemp = Environment::Get( "Temperature", gcl );
        DTR = Environment::Get( "DiurnalTemperatureRange", gcl );

        //Calculate the Warming tolerance and thermal safety margin given standard deviation of monthly temperature
        WarmingTolerance = TerrestrialWarmingToleranceSlope * Environment::Get( "SDTemperature", gcl ) + TerrestrialWarmingToleranceIntercept;
        ThermalSafetyMargin = TerrestrialTSMSlope * Environment::Get( "SDTemperature", gcl ) + TerrestrialTSMIntercept;

        Topt = ThermalSafetyMargin + Environment::Get( "AnnualTemperature", gcl );
        CTmax = WarmingTolerance + Environment::Get( "AnnualTemperature", gcl );


        double PerformanceStandardDeviation = ( CTmax - Topt ) / 12;

        CTmin = Topt - 4 * PerformanceStandardDeviation;

        return ProportionDaySuitable( );

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the proportion of the current timestep that this cohort is active for
    Is a function of the critical max and min temperatures for this ectotherm cohort and also the ambient temperature and diurnal variation in this cell
    Assumes that the diurnal temperature range is symmetrical around the monthly mean temperature
    Alse assumes that the diurnal temperature profile is approximated by a sinusoidal time-series
    Sin of form:
    //        ///T(h)=Ambient+ [DTR*(0.5*sin(omega*(h-6)))]

    @return The proportion of the day that temperatures are between CTmin and CTmax*/
    double ProportionDaySuitable( ) {

        //Calculate the diurnal maximum in the current month
        double DTmax = AmbientTemp + ( 0.5 * DTR );
        double DTmin = AmbientTemp - ( 0.5 * DTR );

        double temp = 2 * ( CTmax - AmbientTemp ) / DTR;
        if( CTmax - DTmax > 0.0 ) {
            temp = 1.0;
        } else if( CTmax - DTmin < 0.0 ) {
            temp = -1.0;
        }

        //Proportion of time for which ambient temperatures are greater than the critical upper temperature
        double POver = ( ( Maths::Get( )->Pi( ) / 2.0 ) - asin( temp ) ) / Maths::Get( )->Pi( );

        if( CTmin - DTmax > 0.0 ) {
            temp = 1.0;
        } else if( CTmin - DTmin < 0.0 ) {
            temp = -1.0;
        } else {
            temp = 2 * ( CTmin - AmbientTemp ) / DTR;
        }
        //Proportion of time for which ambient temperatures are below the critical lower temperature
        double PBelow = 1 - ( ( Maths::Get( )->Pi( ) / 2.0 ) - asin( temp ) ) / Maths::Get( )->Pi( );

        return 1 - ( POver + PBelow );
    }
    //----------------------------------------------------------------------------------------------
};
#endif
