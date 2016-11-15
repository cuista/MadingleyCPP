#ifndef TMETABOLISMENDOTHERM_H
#define TMETABOLISMENDOTHERM_H
/** \file TMetabolismEndotherm.h
 * \brief the TMetabolismEndotherm header file
 */

//
//namespace Madingley
//{

/** \brief A formulation of the metabolism process for Endothermic organisms 
\remarks Functional form and parameters taken from fitted relationship in Brown's (2004) Metabolic Theory of Ecology.
Currently mass assigned to reproductive potential is not metabolised
Assumes that endothermic organisms metabolise at 37degC, and that they can adapt physiologicaly to do this without extra costs*/
class MetabolismEndotherm: public MetabolismImplementation {
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief Time units associated with the formulation of metabolism */
    string TimeUnitImplementation;
    /** \brief Exponent describing the mass-dependency of metabolic rate*/
    double MetabolismMassExponent;
    /** \brief Normalization constant for field metabolic rate  (independent of mass and temperature)*/
    double NormalizationConstant;
    //
    /** \brief The activation energy of metabolism*/
    double ActivationEnergy;
    //
    /** \brief Boltzmann's constant*/
    double BoltzmannConstant;
    //
    /** \brief Scalar to convert energy in kJ to energy in grams mass*/
    double EnergyScalar;
    //
    /** \brief Scalar value for endotherm body temperature*/
    double EndothermBodyTemperature;
    /** \brief Scalar to convert from the time units used by this metabolism implementation to the global model time step units */
    double DeltaT;
    /** \brief Constant to convert temperature in degrees Celsius to temperature in Kelvin */
    double TemperatureUnitsConvert;
    /** \brief Instance of the class to perform general functions */
    UtilityFunctions Utilities;
    /** \brief Constructor for metabolism: assigns all parameter values */
public:
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    MetabolismEndotherm( string globalModelTimeStepUnit ) {
        // Initialise ecological parameters for metabolism
        InitialiseMetabolismParameters( );

        // Calculate the scalar to convert from the time step units used by this implementation of metabolism to the global  model time step units
        DeltaT = Utilities.ConvertTimeUnits( globalModelTimeStepUnit, TimeUnitImplementation );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Run metabolism for the acting cohort
    @param actingCohort The position of the acting cohort in the jagged array of grid cell cohorts 
    @param currentTimestep The current model time step 
    @param currentMonth The current model month */
    void RunMetabolism( Cohort& actingCohort, unsigned currentTimestep, unsigned currentMonth ) {
        // Calculate metabolic loss for an individual and add the value to the delta biomass for metabolism
        Cohort::mMassFluxes["biomass"]["metabolism"] = -CalculateIndividualMetabolicRate( actingCohort.mIndividualBodyMass,
                Environment::Get( "Temperature", actingCohort.GetCurrentCell( ) ) + TemperatureUnitsConvert ) * DeltaT;

        // If metabolic loss is greater than individual body mass after herbivory and predation, then set equal to individual body mass
        Cohort::mMassFluxes["biomass"]["metabolism"] = max( Cohort::mMassFluxes["biomass"]["metabolism"], -( actingCohort.mIndividualBodyMass + Cohort::mMassFluxes["biomass"]["predation"] + Cohort::mMassFluxes["biomass"]["herbivory"] ) );

        // Add total metabolic loss for all individuals in the cohort to delta biomass for metabolism in the respiratory CO2 pool
        Cohort::mMassFluxes["respiratoryCO2pool"]["metabolism"] = -Cohort::mMassFluxes["biomass"]["metabolism"] * actingCohort.mCohortAbundance;

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Initialises values for all ecological parameters for metabolism
    \remarks Metabolism exponent and normalization constant calculated based on Nagy et al (1999) field metabolic rates.
    Use the Brown (2004) functional form and take the activation energy for metabolism from there
    The scalar to convert kJ to grams mass currently a very rough estimate based on the calorific values
    of fat, protein and carbohydrate */
    void InitialiseMetabolismParameters( ) {
        TimeUnitImplementation = "day";

        // Parameters from fitting to Nagy 1999 Field Metabolic Rates for mammals and birds, and assuming that these endotherms are metabolising with a body temperature of 310K (37C)
        MetabolismMassExponent = 0.7;
        NormalizationConstant = 9.0809083973E+11;
        ActivationEnergy = 0.69; // includes endotherms in hibernation and torpor
        BoltzmannConstant = 8.617e-5;

        // Currently a very rough estimate based on calorific values of fat, protein and carbohydrate - assumes organism is metabolising mass of 1/4 protein, 1/4 carbohydrate and 1/2 fat 
        EnergyScalar = 1 / 27.25;


        // Set the constant to convert temperature in degrees Celsius to Kelvin
        TemperatureUnitsConvert = 273.0;

        // Assume all endotherms have a constant body temperature of 37degC
        EndothermBodyTemperature = 37.0 + TemperatureUnitsConvert;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate metabolic loss in grams for an individual 
    @param individualBodyMass The body mass of individuals in the acting cohort 
    @param temperature The ambient temperature, in degrees Kelvin 
    @return The metabolic loss for an individual*/
    double CalculateIndividualMetabolicRate( double individualBodyMass, double temperature ) {
        // Calculate metabolic loss in kJ
        double MetabolicLosskJ = NormalizationConstant * pow( individualBodyMass, MetabolismMassExponent ) *
                exp( -( ActivationEnergy / ( BoltzmannConstant * EndothermBodyTemperature ) ) );

        // Return metabolic loss in grams
        return MetabolicLosskJ * EnergyScalar;

    }
    //----------------------------------------------------------------------------------------------
};
#endif
