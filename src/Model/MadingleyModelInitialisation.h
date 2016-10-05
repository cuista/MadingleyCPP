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
#include <nonStaticSimpleRNG.h>

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
    /** The full path for the output files for a set of simulations */
    string OutputPath;
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
    /** \brief An instance of the mersenne twister generator class */
    std::mt19937_64 RandomNumberGenerator; 
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
        cout << "Initialising model...\n" << endl;
        //read and store environmental layers
        Environment::Get( );
        cout << "data read" << endl;
        NextCohortID = 0;

        ReadInitialisationFiles( );
        Grid.SetUpGrid( );

        // Set up the cohorts and stocks
        InitializationTimer.Start( );

        long totalCohorts = 0, totalStocks = 0;
        Grid.ApplyFunctionToAllCells( [&]( GridCell & c ) {
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
        Cohort::ResetMassFluxes( );
        cout << "Time required: " << InitializationTimer.GetElapsedTimeSecs( ) << endl;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Reads in all initialisation files  
     @ param* initialisationFile The name of the initialization file with information on the simulations to be run
     @param outputPath The path to folder in which outputs will be stored
     //        /// <todo>Need to adjust this file to deal with incorrect inputs, extra columns etc by throwing an error</todo>
     //        /// <todo>Also need to strip leading spaces</todo>*/
    void ReadInitialisationFiles( ) {
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
        gcl.SetCohortSize( CohortFunctionalGroupDefinitions.AllFunctionalGroupsIndex.size( ) );
        for( int FunctionalGroup: CohortFunctionalGroupDefinitions.AllFunctionalGroupsIndex ) {
            int N = CohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup( "Initial number of GridCellCohorts", FunctionalGroup );
            if( ( CohortFunctionalGroupDefinitions.GetTraitNames( "Realm", FunctionalGroup ) == "terrestrial" && !gcl.IsMarine( ) ) ||
                    ( CohortFunctionalGroupDefinitions.GetTraitNames( "Realm", FunctionalGroup ) == "marine" && gcl.IsMarine( ) ) ) {

                NumCohortsThisCell += N;
            }
        }
        if( NumCohortsThisCell > 0 );
        {
            //Loop over all functional groups in the model
            for( int FunctionalGroup: CohortFunctionalGroupDefinitions.AllFunctionalGroupsIndex ) {
                // If it is a functional group that corresponds to the current realm, then seed cohorts
                if( ( CohortFunctionalGroupDefinitions.GetTraitNames( "Realm", FunctionalGroup ) == "terrestrial" && !gcl.IsMarine( ) ) ||
                        ( CohortFunctionalGroupDefinitions.GetTraitNames( "Realm", FunctionalGroup ) == "marine" && gcl.IsMarine( ) ) ) {
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
                        std::uniform_real_distribution<double> unifrandomNumber( 0.0, 1.0 );
                        CohortAdultMass = pow( 10, ( unifrandomNumber( RandomNumberGenerator ) * ( log10( MassMaximum ) - log10( 50 * MassMinimum ) ) + log10( 50 * MassMinimum ) ) );
                        //Changes from original code
                        std::normal_distribution<double> randomNumber( 0.1, 0.02 );
                        OptimalPreyBodySizeRatio = max( 0.01, randomNumber( RandomNumberGenerator ) );

                        if( !gcl.IsMarine( ) ) {
                            do {
                                ExpectedLnAdultMassRatio = 2.24 + 0.13 * log( CohortAdultMass );
                                std::lognormal_distribution<double> randomNumber( ExpectedLnAdultMassRatio, 0.5 );
                                
                                CohortAdultMassRatio = 1.0 + randomNumber( RandomNumberGenerator );
                                
                                CohortJuvenileMass = CohortAdultMass * 1.0 / CohortAdultMassRatio;
                            } while( CohortAdultMass <= CohortJuvenileMass || CohortJuvenileMass < MassMinimum );
                        } else{
                            do {
                                ExpectedLnAdultMassRatio = 2.24 + 0.13 * log( CohortAdultMass );
                                std::lognormal_distribution<double> randomNumber( ExpectedLnAdultMassRatio, 0.5 );
                                
                                CohortAdultMassRatio = 1.0 + 10*randomNumber( RandomNumberGenerator );
                                
                                CohortJuvenileMass = CohortAdultMass * 1.0 / CohortAdultMassRatio;
                            } while( CohortAdultMass <= CohortJuvenileMass || CohortJuvenileMass < MassMinimum );
                        }
                        //original code
/* // Terrestrial and marine organisms have different optimal prey/predator body mass ratios
                        if( !gcl.IsMarine( ) ) {
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
                        if( !gcl.IsMarine( ) ) {
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
*/

                        double NewBiomass = ( 3300. / NumCohortsThisCell ) * 100 * 3000 *
                                pow( 0.6, ( log10( CohortJuvenileMass ) ) ) * ( gcl.GetCellArea( ) );

                        TotalNewBiomass += NewBiomass;
                        double NewAbund = 0.0;

                        NewAbund = NewBiomass / CohortJuvenileMass;
                        
                        // Initialise the new cohort with the relevant properties
                        Cohort NewCohort( gcl, FunctionalGroup, CohortJuvenileMass, CohortAdultMass, CohortJuvenileMass, NewAbund,
                                OptimalPreyBodySizeRatio, 0, ProportionTimeActive, NextCohortID );

                        // Add the new cohort to the list of grid cell cohorts
                        gcl.mGridCellCohorts[FunctionalGroup].push_back( NewCohort );

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
                gcl.mGridCellStocks[FunctionalGroup].push_back( NewStock );

                totalStocks++;
            }
        }

        return totalStocks;
    }
    //----------------------------------------------------------------------------------------------

};
#endif

