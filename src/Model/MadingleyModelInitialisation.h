#ifndef MADINGLEYMODELINITIALISATION_H
#define MADINGLEYMODELINITIALISATION_H

#include <iostream>
#include <map>
#include <string>
#include <UtilityFunctions.h>
#include <FunctionalGroupDefinitions.h>
#include <MassBinsHandler.h>
#include <sstream>
#include <ModelGrid.h>
#include <Stopwatch.h>
#include <cstdlib>

#include "Constants.h"
#include "Parameters.h"

/**
 \ file *MadingleyModelInitialisation.h
 \brief The MadingleyModelInitialisation header file
 */

/**
 \ brief*
 Initialization information for Madingley model simulations
 */
class MadingleyModelInitialisation {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** String identifying time step units to be used by the simulations */
    //string GlobalModelTimeStepUnit;
    /** The number of time steps to be run in the simulations */
    //unsigned NumTimeSteps;
    /** The size of cells to be used in the model grid */
    //float CellSize;
    /** The lowest extent of the model grid in degrees */
    //float BottomLatitude;
    /** The uppermost extent of the model grid in degrees */
    //float TopLatitude;
    /** The leftmost extent of the model grid in degrees */
    //float LeftmostLongitude;
    /** The rightmost extent of the model grid in degrees */
    //float RightmostLongitude;
    /** The rarefaction of active cells in the model grid */
    int CellRarefaction;
    /** Whether to run the model for different grid cells in parallel (??) */
    bool RunInParallel;
    /** Whether to run the model for different grid cells in parallel */
    bool RunCellsInParallel;
    /** Whether to run the model for different simulations in parallel */
    bool RunSimulationsInParallel;
    /** Include marine realm ? */
    string RunRealm;
    /** Whether to draw cohort properties randomly when seeding them and whether cohorts will undergo ecological processes in a random order
     @remark Value should be set in initialization file, but default value is true */
    //bool DrawRandomly;
    //MB 6/8/2015 Now set in the constructor
    /** The threshold abundance below which cohorts will be made extinct */
    //double ExtinctionThreshold;
    /** The threshold difference between cohorts, within which they will be merged */
    double MergeDifference;
    /** The maximum number of cohorts to be in the model, per grid cell, when it is running */
    //int MaxNumberOfCohorts;
    /**Whether to run only dispersal (i.e. turn all other ecological processes off, and set dispersal probability to one temporarily) */
    bool DispersalOnly;
    //MB 6/8/2015 Default now set in the constructor
    /** The weight threshold (grams) below which marine organisms that are not obligate zooplankton will be dispersed planktonically */
    //double PlanktonDispersalThreshold;
    /** The full path for the output files for a set of simulations */
    string OutputPath;
    /** Whether to output detailed diagnostics for the ecological processes */
    bool TrackProcesses;
    //MB 6/8/2015 default set in constructor
    /** Whether to output detailed diagnostics for the ecological processes*/
    bool TrackGlobalProcesses;
    //MB 6/8/2015 default set in constructor
    /** Whether to display live outputs using Dataset Viewer during the model runs */
    bool LiveOutputs;
    /** Whether or not to track trophic level biomass and flow information specific to the marine realm */
    bool TrackMarineSpecifics;
    StopWatch InitializationTimer;
    /** \brief Information from the initialization file  */
    map<string, string> InitialisationFileStrings;
    /** \brief The functional group definitions of cohorts in the model */
    FunctionalGroupDefinitions CohortFunctionalGroupDefinitions;
    /** \brief The functional group definitions of stocks in the model */
    FunctionalGroupDefinitions StockFunctionalGroupDefinitions;
    /** \brief The paths and filenames for the diagnostics for the ecological processes */
    map<string, string> ProcessTrackingOutputs;
    /** \brief The string values for the units of each environmental data layer */
    //map<string, string> Units;
    /** \brief An instance of the mass bin handler for the current model run */
    MassBinsHandler ModelMassBins;
    /** Instance of Utilities for timestep conversions */
    UtilityFunctions Utilities;
    /** \brief An instance of the simple random number generator class */
    std::default_random_engine RandomNumberGenerator;
    /** track cohort ID number*/
    long long NextCohortID;

    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    MadingleyModelInitialisation( ) {
        ;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Reads the initialization file to get information for the set of simulations to be run
     @param initialisationFile The name of the initialization file with information on the simulations to be run
     @param outputPath The path to folder in which outputs will be stored */
    MadingleyModelInitialisation( long long& NC,
            double& TC,
            double& TS,
            ModelGrid& Grid ) {
        //Write to console
        cout << "Initializing model...\n" << endl;
        //read and store environmental layers
        Environment::Get( );
        cout << "data read" << endl;
        //set default value of cohort random draw properties
        //DrawRandomly = true;
        //set default dispersal
        DispersalOnly = false;
        NextCohortID = 0;

        // Read the intialisation files and store values
        ReadInitialisationFiles( );

        //Grid.SetUpGrid( BottomLatitude, LeftmostLongitude, TopLatitude, RightmostLongitude, CellSize );
        Grid.SetUpGrid( Parameters::Get( )->GetUserMinimumLatitude( ), Parameters::Get( )->GetUserMinimumLongitude( ), Parameters::Get( )->GetUserMaximumLatitude( ), Parameters::Get( )->GetUserMaximumLongitude( ), Parameters::Get( )->GetGridCellSize( ) );


        //MB THis is currently just used to get units -needs replacing
        //ReadEnvironmentalLayers( InitialisationFileStrings["Environmental"], outputPath, Grid );

        // Set up the cohorts and stocks
        InitializationTimer.Start( );

        long totalCohorts = 0, totalStocks = 0;
        Grid.ask( [&]( GridCell & c ) {

            totalCohorts += SeedGridCellCohorts( c );
            totalStocks += SeedGridCellStocks( c );
        } );

        cout << "Total cohorts initialised: " << totalCohorts << endl;
        cout << "Total stocks created " << totalStocks << endl;
        cout << "" << endl;
        NC = NextCohortID;
        TC = totalCohorts;
        TS = totalStocks;
        InitializationTimer.Stop( );
        Cohort::zeroDeltas( );
        cout << "Time required: " << InitializationTimer.GetElapsedTimeSecs( ) << endl;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Reads in all initialisation files  
     @ param* initialisationFile The name of the initialization file with information on the simulations to be run
     @param outputPath The path to folder in which outputs will be stored
     //        /// <todo>Need to adjust this file to deal with incorrect inputs, extra columns etc by throwing an error</todo>
     //        /// <todo>Also need to strip leading spaces</todo>*/
    void ReadInitialisationFiles( ) {
        cout << "Reading initialisation parameters file..." << endl;

        std::string initialisationFile = Constants::cConfigurationDirectory + Constants::cInitialisationFileName;
        ifstream infile( initialisationFile.c_str( ) );
        if( infile.is_open( ) ) {
            string l, header[2];
            getline( infile, l );
            //trim off newline character
            l.pop_back( );
            istringstream s( l );
            for( unsigned i = 0; i < 2; i++ ) {
                getline( s, header[i], ',' );
                transform( header[i].begin( ), header[i].end( ), header[i].begin( ), ::tolower );
            }
            while( infile.good( ) ) {
                string l, data[2];
                getline( infile, l );
                //trim off newline if there is still more data
                if( infile.good( ) )l.pop_back( );
                if( l.length( ) > 1 ) {
                    istringstream s( l );
                    for( unsigned i = 0; i < 2; i++ ) {
                        getline( s, data[i], ',' );
                    }
                }
                string param = data[0], val = data[1];
                transform( param.begin( ), param.end( ), param.begin( ), ::tolower );
                transform( val.begin( ), val.end( ), val.begin( ), ::tolower );

                //if( param == "timestep units" )GlobalModelTimeStepUnit = val;
                //if( param == "length of simulation (years)" )NumTimeSteps = ( unsigned )Utilities.ConvertTimeUnits( "year", GlobalModelTimeStepUnit ) * atoi( val.c_str( ) );
                //if( param == "number timesteps" ) NumTimeSteps = atoi( val.c_str( ) );
                //if( param == "grid cell size" ) CellSize = atof( val.c_str( ) );
                //if( param == "bottom latitude" ) BottomLatitude = atof( val.c_str( ) );
                //if( param == "top latitude" ) TopLatitude = atof( val.c_str( ) );
                //if( param == "leftmost longitude" ) LeftmostLongitude = atof( val.c_str( ) );
                //if( param == "rightmost longitude" ) RightmostLongitude = atof( val.c_str( ) );
                //if( param == "draw randomly" ) DrawRandomly = ( ( val == "yes" ) ? true : false );
                //if( param == "extinction threshold" ) ExtinctionThreshold = atof( val.c_str( ) );
                //if( param == "maximum number of cohorts" ) MaxNumberOfCohorts = atof( val.c_str( ) );
                //if( param == "plankton size threshold" ) PlanktonDispersalThreshold = atof( val.c_str( ) );
                //if( param == "human npp extraction" ) InitialisationFileStrings["HumanNPPExtraction"] = val;

                //if( param == "cohort functional group definitions file" ) {
                //    cout << "Reading functional group definitions..." << endl;
                //    CohortFunctionalGroupDefinitions = FunctionalGroupDefinitions( data[1], Constants::cOutputPath );
                //}
                //if( param == "stock functional group definitions file" ) {
                //    cout << "Reading stock group definitions..." << endl;
                //    //Open a the specified csv file and set up the stock functional group definitions
                //    StockFunctionalGroupDefinitions = FunctionalGroupDefinitions( data[1], Constants::cOutputPath );
                //}
                //if( param == "cohort functional group definitions file" ) InitialisationFileStrings["CohortFunctional"] = val;
                //if( param == "stock functional group definitions file" ) InitialisationFileStrings["StockFunctional"] = val;
                //if( param == "mass bin filename" ) ModelMassBins.SetUpMassBins( data[1] ); //read in mass bins : use data[1] so that filename isn't lower-cased
                //if( param == "grid cell rarefaction" ) CellRarefaction = atoi( val.c_str( ) );
                //if( param == "run cells in parallel" ) RunCellsInParallel = ( ( val == "yes" ) ? true : false );
                //if( param == "run simulations in parallel" ) RunSimulationsInParallel = ( ( val == "yes" ) ? true : false );
                //if( param == "run single realm" ) RunRealm = val;
                //if( param == "dispersal only" ) DispersalOnly = ( ( val == "yes" ) ? true : false );
                //if( param == "live outputs" ) LiveOutputs = ( ( val == "yes" ) ? true : false );
                //if( param == "track marine specifics" ) TrackMarineSpecifics = ( ( val == "yes" ) ? true : false );
                //if( param == "track processes" ) TrackProcesses = ( ( val == "yes" ) ? true : false );
                //if( param == "track global processes" ) TrackGlobalProcesses = ( ( val == "yes" ) ? true : false );

                //if( param == "new cohorts filename" ) ProcessTrackingOutputs["NewCohortsOutput"] = val;
                //if( param == "maturity filename" ) ProcessTrackingOutputs["MaturityOutput"] = val;
                //if( param == "biomasses eaten filename" ) ProcessTrackingOutputs["BiomassesEatenOutput"] = val;
                //if( param == "trophic flows filename" ) ProcessTrackingOutputs["TrophicFlowsOutput"] = val;
                //if( param == "growth filename" ) ProcessTrackingOutputs["GrowthOutput"] = val;
                //if( param == "metabolism filename" ) ProcessTrackingOutputs["MetabolismOutput"] = val;
                //if( param == "npp filename" ) ProcessTrackingOutputs["NPPOutput"] = val;
                //if( param == "predation flows filename" ) ProcessTrackingOutputs["PredationFlowsOutput"] = val;
                //if( param == "herbivory flows filename" ) ProcessTrackingOutputs["HerbivoryFlowsOutput"] = val;
                //if( param == "mortality filename" ) ProcessTrackingOutputs["MortalityOutput"] = val;
                //if( param == "extinction filename" ) ProcessTrackingOutputs["ExtinctionOutput"] = val;

                //if( param == "output detail" )InitialisationFileStrings["OutputDetail"] = val;
                //if( param == "dispersal only type" ) InitialisationFileStrings["DispersalOnlyType"] = val;
                //if( param == "specific location file" ) InitialisationFileStrings["Locations"] = val;
                //if( param == "environmental data file" ) InitialisationFileStrings["Environmental"] = data[1];

                // The following parameters are not in the original EcosystemModelInitialisation file...

                //if( param == "merge difference" ) MergeDifference = atof( val.c_str( ) );
            }
        } else {
            cout << "Something wrong with initialisation parameter file " << initialisationFile << endl;
        }

        cout << "Reading functional group definitions..." << endl;
        InitialisationFileStrings["CohortFunctional"] = Constants::cCohortDefinitionsFileName;
        CohortFunctionalGroupDefinitions = FunctionalGroupDefinitions( Constants::cCohortDefinitionsFileName );
        InitialisationFileStrings["StockFunctional"] = Constants::cStockDefinitionsFileName;
        StockFunctionalGroupDefinitions = FunctionalGroupDefinitions( Constants::cStockDefinitionsFileName );
        ModelMassBins.SetUpMassBins( Constants::cMassBinDefinitionsFileName );

        //assert( CellRarefaction >= 1 && "Cell rarefaction cannot be less than 1" );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Reads the environmental layers listed in the specified file containing a list of environmental layers
     @param environmentalLayerFile The name of the file containing the list of environmental layers
     @param outputPath The path to folder in which outputs will be stored */
    void ReadEnvironmentalLayers( string environmentalLayerFile, string outputPath, ModelGrid& Grid ) {

        cout << "Reading in environmental data:" << endl;
        ifstream infile( environmentalLayerFile.c_str( ) );
        //            // Declare lists to hold the information required to read the environmental layers
        vector<string> Folders;
        vector<string> Filenames;
        vector<string> DatasetNames;
        vector<string> FileTypes;
        vector<string> LayerName;
        vector<string> StaticLayer;
        vector<string> Extensions;
        vector<string> Resolutions;
        vector<string> MethodUnits;
        if( infile.is_open( ) ) {

            string l, header[9];
            getline( infile, l );
            //trim off newline character
            l.pop_back( );
            istringstream s( l );
            for( unsigned i = 0; i < 9; i++ ) {
                getline( s, header[i], ',' );
                transform( header[i].begin( ), header[i].end( ), header[i].begin( ), ::tolower );
                if( header[i] != "folder" &&
                        header[i] != "filename" &&
                        header[i] != "extension" &&
                        header[i] != "dataset name" &&
                        header[i] != "filetype" &&
                        header[i] != "internal layer name" &&
                        header[i] != "static" &&
                        header[i] != "resolution" &&
                        header[i] != "units" ) {
                    cout << "Bad header in environmentalLayerFile file " << environmentalLayerFile << endl;
                    exit( 1 );
                }

            }


            while( infile.good( ) ) {
                string l, data;
                getline( infile, l );
                if( infile.good( ) )l.pop_back( );
                if( l.length( ) > 1 ) {
                    istringstream s( l );
                    for( unsigned i = 0; i < 9; i++ ) {
                        getline( s, data, ',' );

                        if( header[i] == "folder" ) Folders.push_back( data );
                        if( header[i] == "filename" ) Filenames.push_back( data );
                        if( header[i] == "extension" ) Extensions.push_back( data );
                        if( header[i] == "dataset name" ) DatasetNames.push_back( data );
                        if( header[i] == "filetype" ) FileTypes.push_back( data );
                        if( header[i] == "internal layer name" ) LayerName.push_back( data );
                        if( header[i] == "static" ) StaticLayer.push_back( data );
                        if( header[i] == "resolution" ) Resolutions.push_back( data );
                        if( header[i] == "units" ) MethodUnits.push_back( data );

                    }
                }
            }

        } else {
            cout << "Something wrong with environment parameter file " << environmentalLayerFile << endl;
            exit( 1 );
        }
        infile.close( );

        for( int ii = 0; ii < MethodUnits.size( ); ii++ ) {
            //Units[LayerName[ii]] = MethodUnits[ii];
        }

    }
    //----------------------------------------------------------------------------------------------

    /** \brief  Seed grid cells with cohorts, as specified in the model input files
    @param g A reference to a grid cell 
     */
    long SeedGridCellCohorts( GridCell& gcl ) {
        long totalCohorts = 0;
        // Set the seed for the random number generator from the system time
        unsigned seed = std::chrono::system_clock::now( ).time_since_epoch( ).count( );
        if( Parameters::Get( )->GetDrawRandomly( ) == true ) RandomNumberGenerator.seed( seed );
        else RandomNumberGenerator.seed( 1000 );


        unsigned NumCohortsThisCell = 0;
        // Define local variables
        double CohortJuvenileMass;
        double CohortAdultMassRatio;
        double CohortAdultMass;
        double ExpectedLnAdultMassRatio;
        double TotalNewBiomass = 0.0;
        double OptimalPreyBodySizeRatio;


        //Variable for altering the juvenile to adult mass ratio for marine cells when handling certain functional groups eg baleen whales
        double Scaling = 0.0;
        gcl.setCohortSize( CohortFunctionalGroupDefinitions.AllFunctionalGroupsIndex.size( ) );
        for( int FunctionalGroup: CohortFunctionalGroupDefinitions.AllFunctionalGroupsIndex ) {
            int N = CohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup( "Initial number of GridCellCohorts", FunctionalGroup );
            if( ( CohortFunctionalGroupDefinitions.GetTraitNames( "Realm", FunctionalGroup ) == "terrestrial" && !gcl.isMarine( ) ) ||
                    ( CohortFunctionalGroupDefinitions.GetTraitNames( "Realm", FunctionalGroup ) == "marine" && gcl.isMarine( ) ) ) {

                NumCohortsThisCell += N;
            }
        }
        if( NumCohortsThisCell > 0 );
        {
            //Loop over all functional groups in the model
            for( int FunctionalGroup: CohortFunctionalGroupDefinitions.AllFunctionalGroupsIndex ) {
                // If it is a functional group that corresponds to the current realm, then seed cohorts
                if( ( CohortFunctionalGroupDefinitions.GetTraitNames( "Realm", FunctionalGroup ) == "terrestrial" && !gcl.isMarine( ) ) ||
                        ( CohortFunctionalGroupDefinitions.GetTraitNames( "Realm", FunctionalGroup ) == "marine" && gcl.isMarine( ) ) ) {
                    // Get the minimum and maximum possible body masses for organisms in each functional group
                    double MassMinimum = CohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup( "minimum mass", FunctionalGroup );
                    double MassMaximum = CohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup( "maximum mass", FunctionalGroup );

                    double ProportionTimeActive = CohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup( "proportion suitable time active", FunctionalGroup );

                    // Loop over the initial number of cohorts
                    unsigned NumberOfCohortsInThisFunctionalGroup = 1;

                    NumberOfCohortsInThisFunctionalGroup = CohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup( "initial number of gridcellcohorts", FunctionalGroup );

                    for( unsigned jj = 0; jj < NumberOfCohortsInThisFunctionalGroup; jj++ ) {


                        // Draw adult mass from a log-normal distribution with mean -6.9 and standard deviation 10.0,
                        // within the bounds of the minimum and maximum body masses for the functional group
                        std::uniform_real_distribution<double> randomNumber( 0.0, 1.0 );
                        CohortAdultMass = pow( 10, ( randomNumber( RandomNumberGenerator ) * ( log10( MassMaximum ) - log10( 50 * MassMinimum ) ) + log10( 50 * MassMinimum ) ) );

                        // Terrestrial and marine organisms have different optimal prey/predator body mass ratios
                        if( !gcl.isMarine( ) ) {
                            // Optimal prey body size 10%
                            std::normal_distribution<double> randomNumber( 0.1, 0.02 );
                            OptimalPreyBodySizeRatio = max( 0.01, randomNumber( RandomNumberGenerator ) );
                        } else {
                            if( CohortFunctionalGroupDefinitions.GetTraitNames( "Diet", FunctionalGroup ) == "allspecial" ) {
                                // Note that for this group
                                // it is actually (despite the name) not an optimal prey body size ratio, but an actual body size.
                                // This is because it is invariant as the predator (filter-feeding baleen whale) grows.
                                // See also the predation classes.
                                std::normal_distribution<double> randomNumber( 0.0001, 0.1 );
                                OptimalPreyBodySizeRatio = max( 0.00001, randomNumber( RandomNumberGenerator ) );
                            } else {
                                // Optimal prey body size for marine organisms is 10%
                                std::normal_distribution<double> randomNumber( 0.1, 0.02 );
                                OptimalPreyBodySizeRatio = max( 0.01, randomNumber( RandomNumberGenerator ) );
                            }

                        }

                        // Draw from a log-normal distribution with mean 10.0 and standard deviation 5.0, then add one to obtain 
                        // the ratio of adult to juvenile body mass, and then calculate juvenile mass based on this ratio and within the
                        // bounds of the minimum and maximum body masses for this functional group
                        if( !gcl.isMarine( ) ) {
                            do {
                                ExpectedLnAdultMassRatio = 2.24 + 0.13 * log( CohortAdultMass );
                                std::lognormal_distribution<double> randomNumber( ExpectedLnAdultMassRatio, 0.5 );
                                CohortAdultMassRatio = 1.0 + randomNumber( RandomNumberGenerator );
                                CohortJuvenileMass = CohortAdultMass * 1.0 / CohortAdultMassRatio;
                            } while( CohortAdultMass <= CohortJuvenileMass || CohortJuvenileMass < MassMinimum );
                        }// In the marine realm, have a greater difference between the adult and juvenile body masses, on average
                        else {
                            unsigned Counter = 0;
                            Scaling = 0.2;
                            // Use the scaling to deal with baleen whales not having such a great difference
                            do {

                                ExpectedLnAdultMassRatio = 2.5 + Scaling * log( CohortAdultMass );
                                std::lognormal_distribution<double> randomNumber( ExpectedLnAdultMassRatio, 0.5 );
                                CohortAdultMassRatio = 1.0 + 10 * randomNumber( RandomNumberGenerator );
                                CohortJuvenileMass = CohortAdultMass * 1.0 / CohortAdultMassRatio;
                                Counter++;
                                if( Counter > 10 ) {
                                    Scaling -= 0.01;
                                    Counter = 0;
                                }
                            } while( CohortAdultMass <= CohortJuvenileMass || CohortJuvenileMass < MassMinimum );
                        }


                        double NewBiomass = ( 3300 / NumCohortsThisCell ) * 100 * 3000 *
                                pow( 0.6, ( log10( CohortJuvenileMass ) ) ) * ( gcl.CellArea( ) );
                        TotalNewBiomass += NewBiomass;
                        double NewAbund = 0.0;

                        NewAbund = NewBiomass / CohortJuvenileMass;


                        // Initialise the new cohort with the relevant properties
                        Cohort NewCohort( gcl, FunctionalGroup, CohortJuvenileMass, CohortAdultMass, CohortJuvenileMass, NewAbund,
                                OptimalPreyBodySizeRatio, 0, ProportionTimeActive, NextCohortID );

                        // Add the new cohort to the list of grid cell cohorts
                        gcl.GridCellCohorts[FunctionalGroup].push_back( NewCohort );

                        // Increment the variable tracking the total number of cohorts in the model
                        totalCohorts++;

                    }
                }
            }
        }
        return totalCohorts;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief    Seed grid cell with stocks, as specified in the model input files

    @param gcl The grid cell  */
    long SeedGridCellStocks( GridCell& gcl ) {

        long totalStocks = 0;

        // Loop over all stock functional groups in the model
        for( int FunctionalGroup: StockFunctionalGroupDefinitions.AllFunctionalGroupsIndex ) {
            // Initialise the new stock with the relevant properties
            bool success;
            Stock NewStock( StockFunctionalGroupDefinitions, FunctionalGroup, gcl, success );
            // Add the new stock to the list of grid cell stocks
            if( success ) {
                gcl.GridCellStocks[FunctionalGroup].push_back( NewStock );

                totalStocks++;
            }
        }

        return totalStocks;
    }
    //----------------------------------------------------------------------------------------------

};
#endif

