#ifndef TMETABOLISMECTOTHERMH
#define TMETABOLISMECTOTHERMH

#include "Cohort.h"

/** \file TMetabolismEctotherm.h
 * \brief the TMetabolismEctotherm header file
 */

/** \brief A formulation of the metabolism process for Ectothermic organisms 
\remarks Functional form and parameters taken from fitted relationship in Brown's (2004) Metabolic Theory of Ecology.
 Currently mass assigned to reproductive potential is not metabolised
 Assumes that ectothermic organisms have a body temperature equal to the ambient temperature,
 therefore metabolising at that ambient temperature*/
class MetabolismEctotherm: public MetabolismImplementation {
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief Time units associated with the formulation of metabolism */
    string TimeUnitImplementation;
    /** \brief Exponent describing the mass-dependency of field metabolic rate*/
    double MetabolismMassExponent;
    /** \brief Exponent describing the mass-dependency of basal metabolic rate*/
    double BasalMetabolismMassExponent;
    /** \brief Normalization constant for field metabolic rate  (independent of mass and temperature)*/
    double NormalizationConstant;
    /** \brief Normalization constatnt for basal metabolic rate  (independent of mass and temperature)*/
    double NormalizationConstantBMR;
    /** \brief The activation energy of metabolism*/
    double ActivationEnergy;
    /** \brief Boltzmann's constant*/
    double BoltzmannConstant;
    /** \brief Scalar to convert energy in kJ to energy in grams mass*/
    double EnergyScalar;
    /** \brief The distance of the Max critical temp from the ambient temperature*/
    double WarmingTolerance;
    /** \brief Distance of the Optimal performance temperature from ambient temperature*/
    double ThermalSafetyMargin;
    /** \brief Optimal performance temperature*/
    double Topt;
    /** \brief Maximum critical temperature*/
    double CTmax;
    /** \brief Minimum critical temperature*/
    double CTmin;
    /** \brief The ambient temperature*/
    double AmbientTemp;
    /** \brief The diurnal temperature range*/
    double DTR;
    /** \brief Scalar to convert from the time units used by this metabolism implementation to the global model time step units */
    double DeltaT;
    /** \brief Constant to convert temperature in degrees Celsius to temperature in Kelvin */
    double TemperatureUnitsConvert;
    /** \brief Instance of the class to perform general functions */
    UtilityFunctions Utilities;
    /** \brief Whether the proportion of time that the cohort is active has been recalculated this time step */
    bool ProportionTimeActiveCalculatedThisTimestep;

public:
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief   Constructor for metabolism: assigns all parameter values
     */
    MetabolismEctotherm( string globalModelTimeStepUnit ) {

        // Initialise ecological parameters for metabolism
        InitialiseMetabolismParameters( );

        // Calculate the scalar to convert from the time step units used by this implementation of metabolism to the global  model time step units
        DeltaT = Utilities.ConvertTimeUnits( globalModelTimeStepUnit, TimeUnitImplementation );

        ProportionTimeActiveCalculatedThisTimestep = false;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Run metabolism for the acting cohort
    @param actingCohort The position of the acting cohort in the jagged array of grid cell cohorts 
    @param currentTimestep The current model time step 
    @param currentMonth The current model month */
    void RunMetabolism( Cohort& actingCohort, unsigned currentTimestep, unsigned currentMonth ) {


        // Calculate metabolic loss for an individual and add the value to the delta biomass for metabolism
        Cohort::mMassFluxes["biomass"]["metabolism"] = -CalculateIndividualMetabolicRate( actingCohort.mIndividualBodyMass,
                Environment::Get( "Temperature", actingCohort.GetCurrentCell( ) ) + TemperatureUnitsConvert, actingCohort.mProportionTimeActive ) * DeltaT;

        // If metabolic loss is greater than individual body mass after herbivory and predation, then set equal to individual body mass
        Cohort::mMassFluxes["biomass"]["metabolism"] = max( Cohort::mMassFluxes["biomass"]["metabolism"], -( actingCohort.mIndividualBodyMass + Cohort::mMassFluxes["biomass"]["predation"] + Cohort::mMassFluxes["biomass"]["herbivory"] ) );

        // Add total metabolic loss for all individuals in the cohort to delta biomass for metabolism in the respiratory CO2 pool
        Cohort::mMassFluxes["respiratoryCO2pool"]["metabolism"] = -Cohort::mMassFluxes["biomass"]["metabolism"] * actingCohort.mCohortAbundance;

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Initialises values for all ecological parameters for metabolism
    @remark
    Metabolism exponent and normalization constant calculated based on Nagy et al (1999) field metabolic rates.
    Use the Brown (2004) functional form and take the activation energy for metabolism from there
    The scalar to convert kJ to grams mass currently a very rough estimate based on the calorific values
    of fat, protein and carbohydrate
     */
    void InitialiseMetabolismParameters( ) {
        TimeUnitImplementation = "day";

        // Parameters from fitting to Nagy 1999 Field Metabolic Rates for reptiles - assumes that reptile FMR was measured with animals at their optimal temp of 30degC
        MetabolismMassExponent = 0.88;
        NormalizationConstant = 148984000000;//1.4898373851E+11;
        ActivationEnergy = 0.69; // includes endotherms in hibernation and torpor
        BoltzmannConstant = 8.617e-5;

        // BMR normalisation constant from Brown et al 2004 - original units of J/s so scale to kJ/d
        NormalizationConstantBMR = 41918272883;//exp( 20 )*60 * 60 * 24 / 1000;
        BasalMetabolismMassExponent = 0.69;

        // Currently a very rough estimate based on calorific values of fat, protein and carbohydrate - assumes organism is metabolising mass of 1/4 protein, 1/4 carbohydrate and 1/2 fat 
        EnergyScalar = 0.036697248;//1 / 27.25;


        // Set the constant to convert temperature in degrees Celsius to Kelvin
        TemperatureUnitsConvert = 273.0;

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate metabolic loss in grams for an individual
    @param individualBodyMass The body mass of individuals in the acting cohort 
    @param temperature The ambient temperature, in degrees Kelvin 
    @return The metabolic loss for an individual*/
    double CalculateIndividualMetabolicRate( double individualBodyMass, double temperature, double proportionTimeActive ) {
        // Calculate field metabolic loss in kJ
        double FieldMetabolicLosskJ = NormalizationConstant * pow( individualBodyMass, MetabolismMassExponent ) *
                exp( -( ActivationEnergy / ( BoltzmannConstant * temperature ) ) );

        double BasalMetabolicLosskJ = NormalizationConstantBMR * pow( individualBodyMass, BasalMetabolismMassExponent ) *
                exp( -( ActivationEnergy / ( BoltzmannConstant * temperature ) ) );

        // Return metabolic loss in grams

        return (( proportionTimeActive * FieldMetabolicLosskJ ) + ( ( 1 - proportionTimeActive ) * ( BasalMetabolicLosskJ ) ) ) * EnergyScalar;

        //return FieldMetabolicLosskJ * EnergyScalar;//commented out in original
    }
    //----------------------------------------------------------------------------------------------
};
#endif
