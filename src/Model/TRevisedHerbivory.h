#ifndef TREVISEDHERBIVORY_H
#define TREVISEDHERBIVORY_H

#include "GridCell.h"

/** \file TRevisedHerbivory.h
 * \brief the TRevisedHerbivory header file
 */

/** \brief A revised version of the herbivory process, written November 2011 */
class RevisedHerbivory: public IEatingImplementation {
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief The time unit associated with this herbivory implementation and its parameters */
    const std::string TimeUnitImplementation = "Day";
    /** \brief Scalar to convert from the time step units used by this herbivory implementation to global model time step units*/
    double DeltaT;
    /** \brief The proportion of time that a herbivore cohort spends eating*/
    //double ProportionOfTimeEating;
    /** \brief Jagged array mirroring the grid cell stocks to store the biomasses eaten in herbivory*/
    std::vector< std::vector <double> > BiomassesEaten;
    /** \brief Jagged array mirroring the grid cell stocks to store the potential biomasses eaten (given the rate of encounter) in herbivory*/
    std::vector< std::vector <double> > PotentialBiomassesEaten;
    /** \brief List of autotroph functional group indices to be eaten in herbivory*/
    //vector<int> FunctionalGroupIndicesToEat;
    /** \brief The total biomass eaten by the acting cohort */
    //double TotalBiomassEatenByCohort;
    /** \brief Cumulative number of time units to handle all of the potential biomass eaten from all autotroph stocks*/
    //double TimeUnitsToHandlePotentialFoodItems;
    /** \brief The area (in square km) of the grid cell*/
    double CellArea;
    /** \brief The area of the current grid cell in hectares*/
    double CellAreaHectares;
    /** \brief Individual body mass of herbivores*/
    double BodyMassHerbivore;
    /** \brief  Holds the edible plant mass available */
    double EdibleMass;
    /** \brief  Holds the scaling to get from exstant autotroph biomass to the edible mass*/
    double EdibleScaling;
    /** \brief The assimilation efficiency of eaten autotroph mass into herbivore body mass */
    //double AssimilationEfficiency;
    /** \brief The scalar of the relationship between handling time and the function of herbivore mass for the terrestrial realm */
    const double HandlingTimeScalarTerrestrial = 0.7;
    /** \brief The scalar of the relationship between handling time and the function of herbivore mass for the marine realm */
    const double HandlingTimeScalarMarine = 0.7;
    /** \brief The exponent applied to herbivore mass in the handling time relationship for the terrestrial realm */
    const double HandlingTimeExponentTerrestrial = 0.7;
    /** \brief The exponent applied to herbivore mass in the handling time relationship for the marine realm */
    const double HandlingTimeExponentMarine = 0.7;
    /** \brief Reference mass of plant matter for calculating handling times */
    const double ReferenceMass = 1.0;
    /** \brief The maximum herbivory rate for a herbivore of 1 g */
    const double HerbivoryRateConstant = 1.0E-11;
    /** \brief The exponent to apply to body mass in the relationship between body mass and herbivory rate */
    const double HerbivoryRateMassExponent = 1.0;
    /** \brief The exponent applied to prey density when calculating attack rates for organisms in the terrestrial realm */
    const double AttackRateExponentTerrestrial = 2.0;
    /** \brief The exponent applied to prey density when calculating attack rates for organisms in the marine realm */
    const double AttackRateExponentMarine = 2.0;
    /** \brief Variable to hold the instantaneous fraction of the autotroph stock biomass that is eaten*/
    double InstantFractionEaten;
    /** \brief Instance of the class to perform general functions*/
    UtilityFunctions Utilities;
    //
public:
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief Constructor for herbivory: assigns all parameter values
    @param cellArea The area (in square km) of the grid cell 
    @param globalModelTimeStepUnit The time step unit used in the model */
    RevisedHerbivory( std::string globalModelTimeStepUnit ) {

        // Calculate the scalar to convert from the time step units used by this implementation of herbivory to the global model time step units
        DeltaT = Utilities.ConvertTimeUnits( globalModelTimeStepUnit, TimeUnitImplementation );

    }

    ~RevisedHerbivory( ) {
        for( auto& B: BiomassesEaten )B.clear( );
        for( auto& P: PotentialBiomassesEaten )P.clear( );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Initialises herbivory implementation each time step
    @param gcl The current grid cell 
    @param params The definitions of model parameters 
    \remark This only works if: a) herbivory is initialised in every grid cell; and b) if parallelisation is done by latitudinal strips
    It is critical to run this every time step */
    void InitializeEatingPerTimeStep( GridCell& gcl, MadingleyModelInitialisation& params ) {
        // Store the specified cell area in this instance of this herbivory implementation
        CellArea = gcl.GetCellArea( );
        CellAreaHectares = CellArea * 100;
        // Get the functional group indices of all autotroph stocks
        FunctionalGroupIndicesToEat = params.StockFunctionalGroupDefinitions.GetFunctionalGroupIndex( "Heterotroph/Autotroph", "Autotroph", false );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the potential biomass that could be gained through herbivory on each grid cell autotroph stock
    @param gcl The current grid cell 
    @param actingCohort The acting cohort 
    @param params The definitions for stuff in the model */
    void GetEatingPotentialTerrestrial( GridCell& gcl, Cohort& actingCohort, MadingleyModelInitialisation& params ) {
        // Set the total biomass eaten by the acting cohort to zero
        TotalBiomassEatenByCohort = 0.0;

        // Get the individual body mass of the acting cohort
        BodyMassHerbivore = actingCohort.mIndividualBodyMass;

        // Set the total number of units to handle all potential biomass eaten to zero
        TimeUnitsToHandlePotentialFoodItems = 0.0;

        // Initialise the jagged arrays to hold the potential and actual biomass eaten in each of the grid cell autotroph stocks
        BiomassesEaten.resize( gcl.mGridCellStocks.size( ) );
        PotentialBiomassesEaten.resize( gcl.mGridCellStocks.size( ) );

        // Loop over rows in the jagged arrays and initialise each vector
        for( int i = 0; i < gcl.mGridCellStocks.size( ); i++ ) {
            BiomassesEaten[i].resize( gcl.mGridCellStocks[i].size( ) );
            PotentialBiomassesEaten[i].resize( gcl.mGridCellStocks[i].size( ) );
        }

        // Loop over functional groups that can be eaten
        for( int FunctionalGroup: FunctionalGroupIndicesToEat ) {
            // Loop over stocks within the functional group
            for( int i = 0; i < gcl.mGridCellStocks[FunctionalGroup].size( ); i++ ) {
                // Get the mass from this stock that is available for eating (assumes only 10% is edible)
                EdibleMass = gcl.mGridCellStocks[FunctionalGroup][i].TotalBiomass * 0.1;

                // Calculate the potential biomass eaten from this stock by the acting cohort
                PotentialBiomassesEaten[FunctionalGroup][i] = CalculatePotentialBiomassEatenTerrestrial( EdibleMass, BodyMassHerbivore );

                // Add the time required to handle the potential biomass eaten from this stock to the cumulative total for all stocks
                TimeUnitsToHandlePotentialFoodItems += PotentialBiomassesEaten[FunctionalGroup][i] *
                        CalculateHandlingTimeTerrestrial( BodyMassHerbivore );

            }
        }

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the potential biomass that could be gained through herbivory on each grid cell autotroph stock
    @param gcl The current grid cell 
    @param actingCohort The acting cohort 
    @params All your base are belong to us */
    void GetEatingPotentialMarine( GridCell& gcl, Cohort& actingCohort, MadingleyModelInitialisation& params ) {
        // Set the total biomass eaten by the acting cohort to zero
        TotalBiomassEatenByCohort = 0.0;

        // Get the individual body mass of the acting cohort
        BodyMassHerbivore = actingCohort.mIndividualBodyMass;

        // Set the total number of units to handle all potential biomass eaten to zero
        TimeUnitsToHandlePotentialFoodItems = 0.0;

        // Initialise the jagged arrays to hold the potential and actual biomass eaten in each of the grid cell autotroph stocks
        BiomassesEaten.resize( gcl.mGridCellStocks.size( ) );
        PotentialBiomassesEaten.resize( gcl.mGridCellStocks.size( ) );

        // Loop over rows in the jagged arrays and initialise each vector
        for( int i = 0; i < gcl.mGridCellStocks.size( ); i++ ) {
            BiomassesEaten[i].resize( gcl.mGridCellStocks[i].size( ) );
            PotentialBiomassesEaten[i].resize( gcl.mGridCellStocks[i].size( ) );
        }

        // Loop over functional groups that can be eaten
        for( int FunctionalGroup: FunctionalGroupIndicesToEat ) {
            // Loop over stocks within the functional group
            for( int i = 0; i < gcl.mGridCellStocks[FunctionalGroup].size( ); i++ ) {
                // Get the mass from this stock that is available for eating (assumes all marine autotrophic organisms are edible)
                //EdibleMass = gridCellStocks[FunctionalGroup][i].TotalBiomass * 0.1; //MB weird line
                EdibleMass = gcl.mGridCellStocks[FunctionalGroup][i].TotalBiomass;
                // Calculate the potential biomass eaten from this stock by the acting cohort
                PotentialBiomassesEaten[FunctionalGroup][i] = CalculatePotentialBiomassEatenMarine( EdibleMass, BodyMassHerbivore );

                // Add the time required to handle the potential biomass eaten from this stock to the cumulative total for all stocks
                TimeUnitsToHandlePotentialFoodItems += PotentialBiomassesEaten[FunctionalGroup][i] *
                        CalculateHandlingTimeMarine( BodyMassHerbivore );

            }
        }

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the actual amount eaten in herbivory, apply the changes to the eaten autotroph stocks, and update deltas for the herbivore cohort
    @param gcl this grid cell 
    @param actingCohort The acting cohort 
    @param currentTimestep The current model time step 
    @param params  The model parameters */
    void RunEating( GridCell& gcl, Cohort& actingCohort,
            unsigned currentTimestep,
            MadingleyModelInitialisation& params ) {

        EdibleScaling = 1.0;
        if( !gcl.IsMarine( ) ) EdibleScaling = 0.1;

        // Loop over autotroph functional groups that can be eaten
        for( int FunctionalGroup: FunctionalGroupIndicesToEat ) {
            // Loop over stocks within the functional groups
            for( int i = 0; i < gcl.mGridCellStocks[FunctionalGroup].size( ); i++ ) {
                // Get the mass from this stock that is available for eating (assumes only 10% is edible in the terrestrial realm)
                EdibleMass = gcl.mGridCellStocks[FunctionalGroup][i].TotalBiomass * EdibleScaling;

                // Calculate the biomass actually eaten from this stock by the acting cohort
                BiomassesEaten[FunctionalGroup][i] = CalculateBiomassesEaten( PotentialBiomassesEaten[FunctionalGroup][i],
                        TimeUnitsToHandlePotentialFoodItems, actingCohort.mCohortAbundance, EdibleMass );

                // Remove the biomass eaten from the autotroph stock
                gcl.mGridCellStocks[FunctionalGroup][i].TotalBiomass -= BiomassesEaten[FunctionalGroup][i];


                // Check that the biomass eaten is not a negative value
                if( BiomassesEaten[FunctionalGroup][i] < 0 ) {
                    std::cout << "Herbivory negative for this herbivore cohort " << actingCohort.mFunctionalGroupIndex << " " << actingCohort.mID << endl;
                    exit( 1 );
                }
                // Add the biomass eaten and assimilated by an individual to the delta biomass for the acting cohort
                //MB should we be able to get here if abundance is zero?
                if( actingCohort.mCohortAbundance > 0 )Cohort::mMassFluxes["biomass"]["herbivory"] += BiomassesEaten[FunctionalGroup][i] * AssimilationEfficiency / actingCohort.mCohortAbundance;

                // Move the biomass eaten but not assimilated by an individual into the organic matter pool
                Cohort::mMassFluxes["organicpool"]["herbivory"] += BiomassesEaten[FunctionalGroup][i] * ( 1 - AssimilationEfficiency );

            }

            // Check that the delta biomass from eating for the acting cohort is not negative
            assert( Cohort::mMassFluxes["biomass"]["herbivory"] >= 0 && "Delta biomass from herbviory is negative" );

            // Calculate the total biomass eaten by the acting (herbivore) cohort
            TotalBiomassEatenByCohort = Cohort::mMassFluxes["biomass"]["herbivory"] * actingCohort.mCohortAbundance;



        }
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculates the potential biomass of an autotroph stock eaten by a herbivore cohort (terrestrial)
    @param autotrophBiomass The total biomass of the autotroph stock 
    @param herbivoreIndividualMass The individual body mass of the acting (herbivore) cohort 
    @return The potential biomass eaten by the herbivore cohort*/
    double CalculatePotentialBiomassEatenTerrestrial( double autotrophBiomass, double herbivoreIndividualMass ) {
        // Calculate the inidividual herbivory rate per unit autotroph mass-density per hectare
        double IndividualHerbivoryRate = CalculateIndividualHerbivoryRatePerHectare( herbivoreIndividualMass );

        // Calculate autotroph biomass density per hectare
        double AutotrophBiomassDensity = autotrophBiomass / CellAreaHectares;

        // Calculate the expected autotroph biomass eaten
        return IndividualHerbivoryRate * pow( AutotrophBiomassDensity, AttackRateExponentTerrestrial );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculates the potential biomass of an autotroph stock eaten by a herbivore cohort (marine)
    @param autotrophBiomass The total biomass of the autotroph stock 
    @param herbivoreIndividualMass The individual body mass of the acting (herbivore) cohort 
    @return The potential biomass eaten by the herbivore cohort*/
    double CalculatePotentialBiomassEatenMarine( double autotrophBiomass, double herbivoreIndividualMass ) {
        // Calculate the inidividual herbivory rate per unit autotroph mass-density per hectare
        double IndividualHerbivoryRate = CalculateIndividualHerbivoryRatePerHectare( herbivoreIndividualMass );

        // Calculate autotroph biomass density per hectare
        double AutotrophBiomassDensity = autotrophBiomass / CellAreaHectares;

        // Calculate the expected autotroph biomass eaten
        return IndividualHerbivoryRate * pow( AutotrophBiomassDensity, AttackRateExponentMarine );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the herbivory rate of an individual herbivore per unit autotroph mass-density per hectare
    @param herbivoreIndividualMass Herbivore individual body mass 
    @return The herbivory rate of an individual herbivore per unit autotroph mass-density per hectare*/
    double CalculateIndividualHerbivoryRatePerHectare( double herbivoreIndividualMass ) {
        // Calculate the individual herbivory rate

        return HerbivoryRateConstant * pow( herbivoreIndividualMass, ( HerbivoryRateMassExponent ) );

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the time taken for a herbivore in the marine realm to handle unit mass (1 g) of autotroph mass
    @param herbivoreIndividualMass The body mass of an individual herbivore 
    @return The time taken for a herbivore to handle unit mass (1 g) of autotroph mass*/
    double CalculateHandlingTimeMarine( double herbivoreIndividualMass ) {
        return HandlingTimeScalarMarine * pow( ( ReferenceMass / herbivoreIndividualMass ), HandlingTimeExponentMarine );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the time taken for a herbivore in the terrestrial realm to handle unit mass (1 g) of autotroph mass
    @param herbivoreIndividualMass The body mass of an individual herbivore 
    @return The time taken for a herbivore to handle unit mass (1 g) of autotroph mass*/
    double CalculateHandlingTimeTerrestrial( double herbivoreIndividualMass ) {
        return HandlingTimeScalarTerrestrial * pow( ( ReferenceMass / herbivoreIndividualMass ), HandlingTimeExponentTerrestrial );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the actual biomass eaten by a herbivore cohort from an autotroph stock 
    @param potentialBiomassEaten The potential biomass eaten by the herbivore cohort from the autotroph stock given the encounter rate 
    @param totalHandlingTime The total time that would be taken to handle all encountered autotroph biomass in all autotroph stocks 
    @param herbivoreAbundance The number of individuals in the acting herbivore cohort 
    @param autotrophBiomass The total biomass in the autotroph stock 
    @return The biomass eaten by the herbivore cohort from the autotroph stock*/
    double CalculateBiomassesEaten( double potentialBiomassEaten, double totalHandlingTime, double herbivoreAbundance, double autotrophBiomass ) {
        // Check whether there is any biomass in the autotroph stock
        if( autotrophBiomass > 0.0 ) {
            // Calculate the instantaneous fraction of the autotroph stock eaten
            InstantFractionEaten = herbivoreAbundance * ( ( potentialBiomassEaten / ( 1 + totalHandlingTime ) ) / autotrophBiomass );
        } else {
            // Set the instantaneous fraction of the autotroph stock eaten to zero
            InstantFractionEaten = 0.0;
        }

        // Return the total  biomass of the autotroph stock eaten
        return autotrophBiomass * ( 1 - exp( -InstantFractionEaten * DeltaT * ProportionTimeEating ) );
    }
    //----------------------------------------------------------------------------------------------
};

#endif
