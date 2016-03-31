#ifndef TREVISEDPREDATION_H
#define TREVISEDPREDATION_H
/** \file TRevisedPredation.h
 * \brief the TRevisedPredation header file
 */

/** \brief A revised version of the predation process, written November 2011 */
class RevisedPredation: public IEatingImplementation {
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief The time unit associated with this particular implementation of predation and its parameters */
    const std::string TimeUnitImplementation = "Day";
    /** \brief The assimilation efficiency of eaten prey mass into predator body mass*/
    //double AssimilationEfficiency;    
    /** \brief The scalar of the relationship between handling time and the function of predator and prey masses for terrestrial animals*/
    double HandlingTimeScalarTerrestrial = 0.5;
    /** \brief The exponent applied to predator mass in the handling time relationship for terrestrial animals*/
    double HandlingTimeExponentTerrestrial = 0.7;
    /** \brief The scalar of the relationship between handling time and the function of predator and prey masses for terrestrial animals*/
    const double HandlingTimeScalarMarine = 0.5;
    /** \brief The exponent applied to predator mass in the handling time relationship for terrestrial animals*/
    const double HandlingTimeExponentMarine = 0.7;
    /** \brief The reference mass property */
    const double ReferenceMass = 1.0;
    /** \brief What's this? */
    double ReferenceMassRatio;
    /** \brief Pre-calculate the specific predator handling time scaling to prevent having to do it for every prey cohort*/
    double SpecificPredatorHandlingTimeScaling;
    /** \brief The maximum kill rate for a predator of 1 g on prey of an optimal size*/
    const double KillRateConstant = 1E-6;
    /** \brief Pre-calculate the maximum kill rate for a specific predator of 1 g on prey of an optimal size*/
    double SpecificPredatorKillRateConstant;
    /** \brief The optimal ratio of prey to predator body masses for terrestrial animals*/
    double OptimalPreyPredatorMassRatioTerrestrial;
    /** \brief The optimal ratio of prey to predator body masses for marine animals*/
    double OptimalPreyPredatorMassRatioMarine;
    /** \brief Some variable or other */
    double RelativeFeedingPreference;
    /** \brief Pre-calculate the proportion of time spent eating (in appropriate time units for this class) for a specific predator*/
    double SpecificPredatorTimeUnitsEatingPerGlobalTimeStep;
    /** \brief The standard deviation in attack rates around the optimal prey to predator mass ratio*/
    const double FeedingPreferenceStandardDeviation = 0.7;
    /** \brief Prey density per hectare; */
    double PreyDensityPerHa;
    /** \brief Killing rate of an individual predator per unit prey density per hectare per time unit */
    double Alphaij;
    /** \brief Variable to hold the instantaneous fraction of the prey cohort that is eaten */
    //double InstantFractionKilled;
    /** \brief Fraction of of the prey cohort remaining given the proportion of time that the predator cohort spends eating */
    //double FractionRemaining;
    /** \brief The exponent on body mass in the relationship between body mass and attack rate*/
    const double KillRateConstantMassExponent = 1.0;
    /** \brief Scalar to convert from the time step units used by this predation implementation to global model time step units */
    double DeltaT;
    /** \brief The proportion of time that a predator cohort spends eating */
    //double ProportionOfTimeEating; //MB should it be ProportionTimeEating, as in the base class? I have assumed so...
    /** \brief Jagged array mirroring the grid cell cohorts to store the abundance gained from predation on each cohort */
    vector< vector < double> > AbundancesEaten;
    /** \brief Jagged array mirroring the grid cell cohorts to store the potential abundance gained (given the number of encounters) from predation on each cohort */
    vector< vector < double> > PotentialAbundanceEaten;
    /** \brief List of cohort functional group indices ot be eaten in predation */
    //        vector<int> FunctionalGroupIndicesToEat; //defined in base class
    /** \brief The total biomass eaten by the acting cohort  */
    //double TotalBiomassEatenByCohort; defined in base class
    /** \brief Cumulative number of time units to handle all of the potential kills from all cohorts */
    //double TimeUnitsToHandlePotentialFoodItems; defined in base class
    /** \brief The area (in square km) of the grid cell
     Cell area for the cell within which this predation object is instantiated. Extracted once to speed up calculations.
     */
    double CellArea;
    /** \brief The area of the current grid cell in hectares */
    double CellAreaHectares;
    /** \brief The proportion of biomass eaten assimilated by predators*/
    double PredatorAssimilationEfficiency;
    /** \brief The proportion of biomass eaten not assimilated by predators */
    double PredatorNonAssimilation;
    /** \brief bool to indicate if the diet of marine species is "allspecial" */
    bool DietIsAllSpecial;
    /** \brief Double to hold the log optimal prey body size ratio for the acting predator cohort */
    double PredatorLogOptimalPreyBodySizeRatio;
    /** \brief Individual body mass of the prey cohort */
    double BodyMassPrey;
    /** \brief Individual body mass of the acting (predator) cohort */
    double BodyMassPredator;
    /** \brief Abundance of the acting (predator) cohort */
    double AbundancePredator;
    double ReferenceMassRatioScalingTerrestrial;
    double ReferenceMassRatioScalingMarine;
    double PredatorAbundanceMultipliedByTimeEating;
    /** \brief Identifies which functional groups are carnivores */
    vector<bool> CarnivoreFunctionalGroups;
    /** \brief Identifies which functional groups are carnivores */
    vector<bool> OmnivoreFunctionalGroups;
    /** \brief Identifies which functional groups are carnivores */
    vector<bool> PlanktonFunctionalGroups;
    /** \brief A boolean which monitors whether or not to track individual cohorts*/
    bool Track;
    /** \brief Number of cohorts in each functional group that were present in the grid cell before this time step's new cohorts were created*/
    vector<int> NumberCohortsPerFunctionalGroupNoNewCohorts;
    /** The matrix to hold the abundance of prey items in each functional group and weight bin */
    vector< vector<double> > BinnedPreyDensities;
    //
    //        // The number of bins in which to combine prey cohorts
    //        // THIS SHOULD ALWAYS BE AN EVEN VALUE
    int NumberOfBins = 12;
    /** The mass bin number of an individual prey cohort */
    int PreyMassBinNumber;
    /** Temporary value to hold calculations */
    double TempDouble;
    /** \brief Instance of the class to perform general functions */
    UtilityFunctions Utilities;
    /** \brief An instance of the simple random number generator class */
    std::default_random_engine RandomNumberGenerator;
public:
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief Constructor for predation: assigns all parameter values
    @param cellArea The area (in square km) of the grid cell 
    @param globalModelTimeStepUnit The time step unit used in the model */
    RevisedPredation( string globalModelTimeStepUnit ) {
        // Calculate the scalar to convert from the time step units used by this implementation of predation to the global model time step units
        DeltaT = Utilities.ConvertTimeUnits( globalModelTimeStepUnit, TimeUnitImplementation );

    }

    ~RevisedPredation( ) {
        for( auto& A: AbundancesEaten )A.clear( );
        for( auto& P: PotentialAbundanceEaten )P.clear( );
        for( auto& b: BinnedPreyDensities )b.clear( );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Initialises predation implementation each time step
    @param gcl The current grid cell 
    @param params The definitions of things in the model 
    @remark This only works if: a) predation is initialised in every grid cell; and b) if parallelisation is done by latitudinal strips
    It is critical to run this every time step */
    void InitializeEatingPerTimeStep( GridCell& gcl, MadingleyModelInitialisation& params ) {
        // Store the specified cell area in this instance of this herbivory implementation
        CellArea = gcl.CellArea( );
        CellAreaHectares = CellArea * 100;
        //Get the functional group indices of all heterotroph cohorts (i.e. potential prey)
        FunctionalGroupIndicesToEat = params.CohortFunctionalGroupDefinitions.GetFunctionalGroupIndex( "Heterotroph/Autotroph", "heterotroph", false );
        // Initialise the vector to hold the number of cohorts in each functional group at the start of the time step
        NumberCohortsPerFunctionalGroupNoNewCohorts.resize( gcl.GridCellCohorts.size( ) );

        // Initialise the jagged arrays to hold the potential and actual numbers of prey eaten in each of the grid cell cohorts
        AbundancesEaten.resize( gcl.GridCellCohorts.size( ) );
        PotentialAbundanceEaten.resize( gcl.GridCellCohorts.size( ) );
        // Initialise the vector to identify carnivore cohorts
        CarnivoreFunctionalGroups.resize( FunctionalGroupIndicesToEat.size( ) );
        OmnivoreFunctionalGroups.resize( FunctionalGroupIndicesToEat.size( ) );
        PlanktonFunctionalGroups.resize( FunctionalGroupIndicesToEat.size( ) );

        // Loop over rows in the jagged arrays, initialise each vector within the jagged arrays, and calculate the current number of cohorts in 
        // each functional group
        for( int i = 0; i < gcl.GridCellCohorts.size( ); i++ ) {
            // Calculate the current number of cohorts in this functional group
            int NumCohortsThisFG = gcl.GridCellCohorts[i].size( );
            NumberCohortsPerFunctionalGroupNoNewCohorts[i] = NumCohortsThisFG;
            // Initialise the jagged arrays
            AbundancesEaten[i].resize( NumberCohortsPerFunctionalGroupNoNewCohorts[i] );
            PotentialAbundanceEaten[i].resize( NumberCohortsPerFunctionalGroupNoNewCohorts[i] );
        }

        // Loop over functional groups that are potential prey and determine which are carnivores
        for( int FunctionalGroup: FunctionalGroupIndicesToEat )
            CarnivoreFunctionalGroups[FunctionalGroup] = params.CohortFunctionalGroupDefinitions.GetTraitNames( "Nutrition source", FunctionalGroup ) == "carnivore";

        for( int FunctionalGroup: FunctionalGroupIndicesToEat )
            OmnivoreFunctionalGroups[FunctionalGroup] = params.CohortFunctionalGroupDefinitions.GetTraitNames( "Nutrition source", FunctionalGroup ) == "omnivore";

        for( int FunctionalGroup: FunctionalGroupIndicesToEat )
            PlanktonFunctionalGroups[FunctionalGroup] = params.CohortFunctionalGroupDefinitions.GetTraitNames( "Mobility", FunctionalGroup ) == "planktonic";
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the potential number of prey that could be gained through predation on each cohort in the grid cell
    @param gcl The current grid cell 
    @param actingCohort The acting cohort 
    @param The definitions for all the model parameters */
    void GetEatingPotentialMarine( GridCell& gcl, Cohort& actingCohort, MadingleyModelInitialisation& params ) {

        BinnedPreyDensities.resize( gcl.GridCellCohorts.size( ) );
        for( auto& b: BinnedPreyDensities )b.resize( NumberOfBins );

        // Set the total eaten by the acting cohort to zero
        TotalBiomassEatenByCohort = 0.0;

        // Set the total number of units to handle all potential prey individuals eaten to zero
        TimeUnitsToHandlePotentialFoodItems = 0.0;

        // Get the individual body mass of the acting (predator) cohort
        BodyMassPredator = actingCohort.mIndividualBodyMass;

        // Get the abundance of the acting (predator) cohort
        AbundancePredator = actingCohort.mCohortAbundance;

        // Pre-calculate individual values for this predator to speed things up
        SpecificPredatorKillRateConstant = KillRateConstant * pow( BodyMassPredator, ( KillRateConstantMassExponent ) );
        SpecificPredatorTimeUnitsEatingPerGlobalTimeStep = DeltaT * ProportionTimeEating;
        PredatorAssimilationEfficiency = AssimilationEfficiency;
        PredatorNonAssimilation = ( 1 - AssimilationEfficiency );

        DietIsAllSpecial = params.CohortFunctionalGroupDefinitions.GetTraitNames( "Diet", actingCohort.mFunctionalGroupIndex ) == "allspecial";

        PredatorLogOptimalPreyBodySizeRatio = actingCohort.mLogOptimalPreyBodySizeRatio;

        // If a filter feeder, then optimal body size is a value not a ratio: convert it to a ratio to ensure that all calculations work correctly
        if( DietIsAllSpecial ) {
            // Optimal body size is actually a value, not a ratio, so convert it to a ratio based on the present body size
            //MB take log of exp ??? really??
            PredatorLogOptimalPreyBodySizeRatio =
                    log( exp( actingCohort.mLogOptimalPreyBodySizeRatio ) / actingCohort.mIndividualBodyMass ); //actingCohort.LogOptimalPreyBodySizeRatio-log(actingCohort.IndividualBodyMass);
        }


        // Calculate the reference mass scaling ratio
        ReferenceMassRatioScalingMarine = HandlingTimeScalarMarine * pow( ReferenceMass / BodyMassPredator, HandlingTimeExponentMarine );

        PredatorAbundanceMultipliedByTimeEating = AbundancePredator * SpecificPredatorTimeUnitsEatingPerGlobalTimeStep;

        // Calculate the abundance of prey in each of the prey mass bins
        PopulateBinnedPreyAbundance( gcl, actingCohort, PredatorLogOptimalPreyBodySizeRatio );

        // Loop over potential prey functional groups
        for( int FunctionalGroup: FunctionalGroupIndicesToEat ) {
            // Eating operates differently for planktivores
            // This can certainly be sped up
            // Loop over cohorts within the functional group
            for( int i = 0; i < NumberCohortsPerFunctionalGroupNoNewCohorts[FunctionalGroup]; i++ ) {
                //No Cannibalism
                if( gcl.GridCellCohorts[FunctionalGroup][i].mID == actingCohort.mID ) {
                    PotentialAbundanceEaten[FunctionalGroup][i] = 0.0;
                } else {
                    if( DietIsAllSpecial ) {

                        // Get the body mass of individuals in this cohort
                        BodyMassPrey = gcl.GridCellCohorts[FunctionalGroup][i].mIndividualBodyMass;

                        // Get the bin number of this prey cohort
                        if( BodyMassPrey > 0 )PreyMassBinNumber = GetBinNumber( BodyMassPrey, BodyMassPredator, PredatorLogOptimalPreyBodySizeRatio );


                        // Check whether 
                        // The prey cohort is within the feeding range of the predator
                        // the prey cohort still exists in the model (i.e. body mass > 0)   
                        // Currently having whales etc eat everything, but preferentially feed on very small things (i.e. filter feeders)
                        if( ( BodyMassPrey > 0 ) && ( PlanktonFunctionalGroups[FunctionalGroup] ) && ( 0 < PreyMassBinNumber ) &&
                                ( PreyMassBinNumber < NumberOfBins ) ) {
                            // Calculate the potential abundance from this cohort eaten by the acting cohort
                            PotentialAbundanceEaten[FunctionalGroup][i] = CalculateExpectedNumberKilledMarine(
                                    gcl.GridCellCohorts[FunctionalGroup][i].mCohortAbundance, BodyMassPrey, PreyMassBinNumber, FunctionalGroup,
                                    BodyMassPredator, CarnivoreFunctionalGroups[FunctionalGroup], OmnivoreFunctionalGroups[FunctionalGroup],
                                    OmnivoreFunctionalGroups[actingCohort.mFunctionalGroupIndex], PredatorLogOptimalPreyBodySizeRatio );

                            // Add the time required to handle the potential abundance eaten from this cohort to the cumulative total for all cohorts
                            TimeUnitsToHandlePotentialFoodItems += PotentialAbundanceEaten[FunctionalGroup][i] *
                                    CalculateHandlingTimeMarine( BodyMassPrey );
                        } else {
                            // Assign a potential abundance eaten of zero
                            PotentialAbundanceEaten[FunctionalGroup][i] = 0.0;
                        }

                    } else {

                        // Get the body mass of individuals in this cohort
                        BodyMassPrey = gcl.GridCellCohorts[FunctionalGroup][i].mIndividualBodyMass;

                        // Get the bin number of this prey cohort
                        if( BodyMassPrey > 0 )PreyMassBinNumber = GetBinNumber( BodyMassPrey, BodyMassPredator, PredatorLogOptimalPreyBodySizeRatio );

                        // Check whether 
                        // The prey cohort is within the feeding range of the predator
                        // the prey cohort still exists in the model (i.e. body mass > 0)   
                        if( ( BodyMassPrey > 0 ) && ( 0 < PreyMassBinNumber ) && ( PreyMassBinNumber < NumberOfBins ) ) {
                            // Calculate the potential abundance from this cohort eaten by the acting cohort
                            PotentialAbundanceEaten[FunctionalGroup][i] = CalculateExpectedNumberKilledMarine(
                                    gcl.GridCellCohorts[FunctionalGroup][i].mCohortAbundance, BodyMassPrey, PreyMassBinNumber, FunctionalGroup,
                                    BodyMassPredator, CarnivoreFunctionalGroups[FunctionalGroup], OmnivoreFunctionalGroups[FunctionalGroup],
                                    OmnivoreFunctionalGroups[actingCohort.mFunctionalGroupIndex], PredatorLogOptimalPreyBodySizeRatio );

                            // Add the time required to handle the potential abundance eaten from this cohort to the cumulative total for all cohorts
                            TimeUnitsToHandlePotentialFoodItems += PotentialAbundanceEaten[FunctionalGroup][i] *
                                    CalculateHandlingTimeMarine( BodyMassPrey );
                        } else {
                            // Assign a potential abundance eaten of zero
                            PotentialAbundanceEaten[FunctionalGroup][i] = 0.0;
                        }
                    }
                }
            }

        }

        // No cannibalism; do this outside the loop to speed up the calculations
        //MB Now moved back into the loop
        //  TimeUnitsToHandlePotentialFoodItems -= PotentialAbundanceEaten[actingCohort.FunctionalGroupIndex][actingCohort.positionInList] *
        //          CalculateHandlingTimeMarine(BodyMassPredator);
        //  PotentialAbundanceEaten[actingCohort.FunctionalGroupIndex][actingCohort.positionInList] = 0.0;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Create the matrix of prey abundances in each weight bin
    @param gcl The current grid cell 
    @param actingCohort The predator cohort 
    @param logOptimalPreyBodySizeRatio How big is the prey  */
    void PopulateBinnedPreyAbundance( GridCell& gcl, Cohort& actingCohort, double logOptimalPreyBodySizeRatio ) {
        int BinNumber = 0;

        // Loop through prey functional groups
        for( auto& fg: FunctionalGroupIndicesToEat ) {
            for( auto& cohort: gcl.GridCellCohorts[fg] ) {
                // Calculate the difference between the actual body size ratio and the optimal ratio, 
                // and then divide by the standard deviation in log ratio space to determine in 
                // which bin to assign the prey item.
                if( cohort.mIndividualBodyMass > 0 ) {
                    BinNumber = GetBinNumber( cohort.mIndividualBodyMass, actingCohort.mIndividualBodyMass,
                            logOptimalPreyBodySizeRatio );

                    if( ( 0 < BinNumber ) && ( BinNumber < NumberOfBins ) ) {
                        BinnedPreyDensities[fg][ BinNumber] += cohort.mCohortAbundance / CellAreaHectares;
                    }
                }
            }
        }

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Get the bin number for a prey of a particular body mass */
    int GetBinNumber( double preyMass, double predatorMass, double predatorOptimalPreyBodySizeRatio ) {
        return ( int )( GetBinNumberFractional( preyMass, predatorMass, predatorOptimalPreyBodySizeRatio ) + ( NumberOfBins / 2 ) );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Get the fractional bin number for a prey of a particular body mass */
    double GetBinNumberFractional( double preyMass, double predatorMass, double predatorOptimalPreyBodySizeRatio ) {
        return (log( preyMass / predatorMass ) - predatorOptimalPreyBodySizeRatio ) /
                ( 0.5 * FeedingPreferenceStandardDeviation );

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the potential number of prey that could be gained through predation on each cohort in the grid cell
    @param gcl The current grid cell 
    @param actingCohort The acting cohort 
    @param params The definitions for items in the model  */
    void GetEatingPotentialTerrestrial( GridCell& gcl, Cohort& actingCohort, MadingleyModelInitialisation& params ) {

        BinnedPreyDensities.resize( gcl.GridCellCohorts.size( ) );
        for( auto& b: BinnedPreyDensities )b.resize( NumberOfBins );

        // Set the total eaten by the acting cohort to zero
        TotalBiomassEatenByCohort = 0.0;

        // Set the total number of units to handle all potential prey individuals eaten to zero
        TimeUnitsToHandlePotentialFoodItems = 0.0;

        // Get the individual body mass of the acting (predator) cohort
        BodyMassPredator = actingCohort.mIndividualBodyMass;

        // Get the abundance of the acting (predator) cohort
        AbundancePredator = actingCohort.mCohortAbundance;

        // Pre-calculate individual values for this predator
        SpecificPredatorKillRateConstant = KillRateConstant * pow( BodyMassPredator, ( KillRateConstantMassExponent ) );
        SpecificPredatorTimeUnitsEatingPerGlobalTimeStep = DeltaT * ProportionTimeEating;
        PredatorAssimilationEfficiency = AssimilationEfficiency;
        PredatorNonAssimilation = ( 1 - AssimilationEfficiency );

        // When body sizes are less than one gram, we have a flat handling time relationship to stop small things have extraordinarily short handling times
        //  if (BodyMassPredator > 1.0)
        //  {
        ReferenceMassRatioScalingTerrestrial = HandlingTimeScalarTerrestrial * pow( ReferenceMass / BodyMassPredator, HandlingTimeExponentTerrestrial );
        //  }
        //  else
        //  {
        //      ReferenceMassRatioScalingTerrestrial = HandlingTimeScalarTerrestrial * ReferenceMass / BodyMassPredator;

        //            }
        PredatorAbundanceMultipliedByTimeEating = AbundancePredator * SpecificPredatorTimeUnitsEatingPerGlobalTimeStep;


        PredatorLogOptimalPreyBodySizeRatio = actingCohort.mLogOptimalPreyBodySizeRatio;

        // Calculate the abundance of prey in each of the prey mass bins
        PopulateBinnedPreyAbundance( gcl, actingCohort, PredatorLogOptimalPreyBodySizeRatio );

        // Loop over potential prey functional groups
        for( int FunctionalGroup: FunctionalGroupIndicesToEat ) {

            // Loop over cohorts within the functional group
            for( int i = 0; i < NumberCohortsPerFunctionalGroupNoNewCohorts[FunctionalGroup]; i++ ) {
                if( gcl.GridCellCohorts[FunctionalGroup][i].mID == actingCohort.mID ) {
                    PotentialAbundanceEaten[FunctionalGroup][i] = 0.0;
                } else {
                    // Get the body mass of individuals in this cohort
                    BodyMassPrey = gcl.GridCellCohorts[FunctionalGroup][i].mIndividualBodyMass;

                    // Get the bin number of this prey cohort
                    if( BodyMassPrey > 0 )PreyMassBinNumber = GetBinNumber( BodyMassPrey, BodyMassPredator, PredatorLogOptimalPreyBodySizeRatio );

                    // Check whether the prey cohort still exists in the model (i.e. body mass > 0)            
                    if( ( BodyMassPrey > 0 ) && ( 0 < PreyMassBinNumber ) && ( PreyMassBinNumber < NumberOfBins ) ) {
                        // Calculate the potential abundance from this cohort eaten by the acting cohort
                        PotentialAbundanceEaten[FunctionalGroup][i] = CalculateExpectedNumberKilledTerrestrial(
                                gcl.GridCellCohorts[FunctionalGroup][i].mCohortAbundance, BodyMassPrey, PreyMassBinNumber, FunctionalGroup,
                                BodyMassPredator, CarnivoreFunctionalGroups[FunctionalGroup], OmnivoreFunctionalGroups[FunctionalGroup],
                                OmnivoreFunctionalGroups[actingCohort.mFunctionalGroupIndex], PredatorLogOptimalPreyBodySizeRatio );

                        // Add the time required to handle the potential abundance eaten from this cohort to the cumulative total for all cohorts
                        TimeUnitsToHandlePotentialFoodItems += PotentialAbundanceEaten[FunctionalGroup][i] *
                                CalculateHandlingTimeTerrestrial( BodyMassPrey );
                    } else {
                        // Assign a potential abundance eaten of zero
                        PotentialAbundanceEaten[FunctionalGroup][i] = 0.0;
                    }
                }
            }
        }

        // No cannibalism; do this outside the loop to speed up the calculations
        //MB Now moved back into loop
        //TimeUnitsToHandlePotentialFoodItems -= PotentialAbundanceEaten[actingCohort.FunctionalGroupIndex][actingCohort.positionInList] *
        //        CalculateHandlingTimeTerrestrial(BodyMassPredator);
        //PotentialAbundanceEaten[actingCohort.FunctionalGroupIndex][actingCohort.positionInList] = 0.0;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Apply the changes from predation to prey cohorts, and update deltas for the predator cohort
    @param gridCell The current grid cell 
    @param actingCohort The acting cohort 
    @param currentTimestep The current model time step */
    void RunEating( GridCell& gcl, Cohort& actingCohort, unsigned currentTimestep,
            MadingleyModelInitialisation& params ) {

        TempDouble = 0.0;

        // Temporary variable to hold the total time spent eating + 1. Saves an extra calculation in CalculateAbundanceEaten
        double TotalTimeUnitsToHandlePlusOne = TimeUnitsToHandlePotentialFoodItems + 1;

        // Loop over potential prey functional groups
        for( int FunctionalGroup: FunctionalGroupIndicesToEat ) {

            // Loop over cohorts within the functional group
            for( int i = 0; i < NumberCohortsPerFunctionalGroupNoNewCohorts[FunctionalGroup]; i++ ) {
                // Get the individual body mass of this cohort
                BodyMassPrey = gcl.GridCellCohorts[FunctionalGroup][i].mIndividualBodyMass;

                // Calculate the actual abundance of prey eaten from this cohort
                if( gcl.GridCellCohorts[FunctionalGroup][i].mCohortAbundance > 0 ) {


                    // Calculate the actual abundance of prey eaten from this cohort
                    AbundancesEaten[FunctionalGroup][i] = CalculateAbundanceEaten( PotentialAbundanceEaten[FunctionalGroup][i], PredatorAbundanceMultipliedByTimeEating,
                            TotalTimeUnitsToHandlePlusOne, gcl.GridCellCohorts[FunctionalGroup][i].mCohortAbundance );

                } else
                    AbundancesEaten[FunctionalGroup][i] = 0;

                // Remove number of prey eaten from the prey cohort
                gcl.GridCellCohorts[FunctionalGroup][i].mCohortAbundance -= AbundancesEaten[FunctionalGroup][i];


                // Check that the abundance eaten from this cohort is not negative
                if( AbundancesEaten[FunctionalGroup][i] < 0. ) {
                    cout << "Predation negative for this prey cohort" << actingCohort.mFunctionalGroupIndex << " " << actingCohort.mID << AbundancesEaten[FunctionalGroup][i] << endl;
                }
                // Create a temporary value to speed up the predation function
                // This is equivalent to the body mass of the prey cohort including reproductive potential mass, times the abundance eaten of the prey cohort,
                // divided by the abundance of the predator
                TempDouble += ( BodyMassPrey + gcl.GridCellCohorts[FunctionalGroup][i].mIndividualReproductivePotentialMass ) * AbundancesEaten[FunctionalGroup][i] / AbundancePredator;


            }
        }

        // Add the biomass eaten and assimilated by an individual to the delta biomass for the acting (predator) cohort
        Cohort::Deltas["biomass"]["predation"] = TempDouble * PredatorAssimilationEfficiency;

        // Move the biomass eaten but not assimilated by an individual into the organic matter pool
        Cohort::Deltas["organicpool"]["predation"] = TempDouble * PredatorNonAssimilation * AbundancePredator;

        // Check that the delta biomass from eating for the acting cohort is not negative
        assert( Cohort::Deltas["biomass"]["predation"] >= 0 && "Predation yields negative biomass" );

        // Calculate the total biomass eaten by the acting (predator) cohort
        TotalBiomassEatenByCohort = Cohort::Deltas["biomass"]["predation"] * AbundancePredator;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the potential number of individuals in a prey cohort eaten by an acting predator cohort given the number of prey detections
    @param preyAbundance The number of individuals in the prey cohort 
    @param preyIndividualMass The body mass of prey individuals 
    @param predatorIndividualMass The body mass of predator individuals 
    @param preyIsCarnivore Whether the prey cohort is a carnivore cohort 
    @param preyIsOmnivore Whether the prey cohort is an omnivore cohort 
    @param predatorIsOmnivore Whether the predator cohort is an omnivore cohort 
    @param logOptimalPreyPredatorMassRatio The log ratio of optimal prey body mass to predator body mass 
    @return The potential number of individuals in a prey cohort eaten by an acting predator cohort*/
    double CalculateExpectedNumberKilledTerrestrial( double preyAbundance, double preyIndividualMass, int preyMassBinNumber, int preyFunctionalGroup, double predatorIndividualMass, bool preyIsCarnivore, bool preyIsOmnivore, bool predatorIsOmnivore,
            double logOptimalPreyPredatorMassRatio ) {
        // Calculate the killing rate of an individual predator per unit prey density per hectare per time unit
        Alphaij = CalculateIndividualKillingRatePerHectare( preyIndividualMass, preyMassBinNumber, preyFunctionalGroup, predatorIndividualMass, logOptimalPreyPredatorMassRatio );

        // Calculate the potential number of prey killed given the number of prey detections
        return Alphaij * preyAbundance / CellAreaHectares;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the potential number of individuals in a prey cohort eaten by an acting predator cohort given the number of prey detections
    @param preyAbundance The number of individuals in the prey cohort 
    @param preyIndividualMass The body mass of prey individuals 
    @param predatorIndividualMass The body mass of predator individuals 
    @param preyIsCarnivore Whether the prey cohort is a carnivore cohort 
    @param preyIsOmnivore Whether the prey cohort is an omnivore cohort 
    @param predatorIsOmnivore Whether the predator cohort is am omnivore cohort 
    @param logOptimalPreyPredatorMassRatio The log ratio of optimal prey body mass to predator body mass 
    @return The potential number of individuals in a prey cohort eaten by an acting predator cohort*/
    double CalculateExpectedNumberKilledMarine( double preyAbundance, double preyIndividualMass, int preyMassBinNumber, int preyFunctionalGroup, double predatorIndividualMass, bool preyIsCarnivore, bool preyIsOmnivore, bool predatorIsOmnivore,
            double logOptimalPreyPredatorMassRatio ) {
        // Calculate the killing rate of an individual predator per unit prey density per hectare per time unit
        Alphaij = CalculateIndividualKillingRatePerHectare( preyIndividualMass, preyMassBinNumber, preyFunctionalGroup, predatorIndividualMass, logOptimalPreyPredatorMassRatio );

        // Calculate the potential number of prey killed given the number of prey detections
        return Alphaij * preyAbundance / CellAreaHectares;
    }

    //        // Original
    //----------------------------------------------------------------------------------------------

    /** \brief Calculates the killing rate of an individual predator per unit prey density per hectare per time unit 
    @param preyIndividualMass The body mass of individuals in the prey cohort 
    @param predatorIndividualMass The body mass of individuals in the predator cohort 
    @param logOptimalPreyPredatorMassRatio The log ratio of optimal prey body mass to predator body mass 
    @return The killing rate of an individual predator per unit prey density per hectare per time unit*/
    double CalculateIndividualKillingRatePerHectare( double preyIndividualMass, int preyMassBinNumber, int preyFunctionalGroup, double predatorIndividualMass, double logOptimalPreyPredatorMassRatio ) {
        //int PreyBinNumber;

        // Calculate the relative feeding preference from a log-normal distribution with mean equal to the optimal 
        // prey to predator ratio and standard deviation as specified
        RelativeFeedingPreference = exp( -( pow
                ( ( ( log( preyIndividualMass / predatorIndividualMass ) - logOptimalPreyPredatorMassRatio ) /
                FeedingPreferenceStandardDeviation ), 2 ) ) );

        // Calculate the individual killing rate
        return SpecificPredatorKillRateConstant * RelativeFeedingPreference * BinnedPreyDensities[preyFunctionalGroup][ preyMassBinNumber];
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculates the time for an individual predator to handle an individual prey in the terrestrial realm

    @param preyIndividualMass The body mass of prey individuals 
    @return The time for an individual predator to handle an individual prey*/
    double CalculateHandlingTimeTerrestrial( double preyIndividualMass ) {
        return ReferenceMassRatioScalingTerrestrial * preyIndividualMass;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculates the time for an individual predator to handle an individual prey in the marine realm
    @param preyIndividualMass The body mass of prey individuals 
    @return The time for an individual predator to handle an individual prey*/
    double CalculateHandlingTimeMarine( double preyIndividualMass ) {
        return ReferenceMassRatioScalingMarine * preyIndividualMass;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the actual abundance of a prey cohort eaten by a predator cohort
    @param potentialKills The potential abundance of the prey cohort eaten by the predator cohort given the number of detections 
    @param totalHandlingTimePlusOne The total time that would be taken to eat all detected prey individuals in all prey cohorts plus one 
    @param predatorAbundanceMultipliedByTimeEating The abundance in the predator cohort 
    @param preyAbundance The abundance in the prey cohort 
    @return The actual abundance of a prey cohort eaten by a predator cohort*/
    double CalculateAbundanceEaten( double potentialKills, double predatorAbundanceMultipliedByTimeEating, double totalHandlingTimePlusOne, double preyAbundance ) {
        // This is the more explicit but slower version
        // Check whether there are any individuals in the prey cohort -MB next section commented out in original
        /*if (preyAbundance > 0.0)
        {
            // Calculate the instantaneous fraction of the prey cohort eaten
            InstantFractionKilled = predatorAbundance * ((potentialKills / totalHandlingTimePlusOne) / preyAbundance);
        }
        else
        {
            // Set the instantaneous fraction of the prey cohort eaten to zero
            InstantFractionKilled = 0.0;
        }
           

        // Calculate the fraction of of the prey cohort remaining given the proportion of time that the predator cohort spends eating
        FractionRemaining = exp(-InstantFractionKilled * SpecificPredatorProportionTimeEating);

        //Return the abundance of prey cohort eaten
        return preyAbundance * (1.0 - FractionRemaining);
         */

        // Optimized for speed; check for zero abundance prey moved to the calling function
        return preyAbundance * ( 1.0 - exp( -( predatorAbundanceMultipliedByTimeEating * ( ( potentialKills / totalHandlingTimePlusOne ) / preyAbundance ) ) ) );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the visibility of the prey cohort (currently set to 1)

    @param preyAbundance The abundance in the prey cohort 
    @return The visibility of the prey cohort*/
    double CalculateVisibility( double preyAbundance ) {
        return pow( preyAbundance, 0 );
    }
    //----------------------------------------------------------------------------------------------
};

#endif
