#ifndef MADINGLEYMODELINITIALISATION_H
#define MADINGLEYMODELINITIALISATION_H

#include "Types.h"
#include "Logger.h"
#include "MassBinsHandler.h"
#include "FunctionalGroupDefinitions.h"
#include "UtilityFunctions.h"
#include "ModelGrid.h"
#include "Stopwatch.h"
#include "Constants.h"
#include "Parameters.h"
#include "NonStaticSimpleRNG.h"

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
    std::string mOutputPath;
    StopWatch mInitializationTimer;
    /** \brief Information from the initialization file  */
    Types::StringMap mInitialisationFileStrings;
    /** \brief The functional group definitions of cohorts in the model */
    FunctionalGroupDefinitions mCohortFunctionalGroupDefinitions;
    /** \brief The functional group definitions of stocks in the model */
    FunctionalGroupDefinitions mStockFunctionalGroupDefinitions;
    /** \brief The paths and filenames for the diagnostics for the ecological processes */
    Types::StringMap mProcessTrackingOutputs;
    /** \brief The string values for the units of each environmental data layer */
    //map<string, string> Units;
    /** \brief An instance of the mass bin handler for the current model run */
    MassBinsHandler mModelMassBins;
    /** Instance of Utilities for timestep conversions */
    UtilityFunctions mUtilities;
    /** \brief An instance of the mersenne twister generator class */
    std::mt19937_64 mRandomNumberGenerator;
    /** track cohort ID number*/
    long long mNextCohortID;

    NonStaticSimpleRNG randomNumber;
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
    MadingleyModelInitialisation( long long& nextCohortID, double& numberOfCohorts, double& numberOfStocks, ModelGrid& modelGrid ) {
        //Write to console
        Logger::Get( )->LogMessage( "Initialising model..." );
        //read and store environmental layers
        Environment::Get( );
        mNextCohortID = 0;

        ReadInitialisationFiles( );
        modelGrid.SetUpGrid( );

        // Set up the cohorts and stocks
        mInitializationTimer.Start( );

        long totalCohorts = 0;
        long totalStocks = 0;

        modelGrid.ApplyFunctionToAllCells( [&]( GridCell & c ) {

            totalCohorts += SeedGridCellCohorts( c );
            totalStocks += SeedGridCellStocks( c );
        } );

        Logger::Get( )->LogMessage( "Total cohorts initialised: " + Convertor::Get( )->ToString( totalCohorts ) );
        Logger::Get( )->LogMessage( "Total stocks created " + Convertor::Get( )->ToString( totalStocks ) );
        Logger::Get( )->LogMessage( "" );

        nextCohortID = mNextCohortID;
        numberOfCohorts = totalCohorts;
        numberOfStocks = totalStocks;
        mInitializationTimer.Stop( );
        Cohort::ResetMassFluxes( );
        Logger::Get( )->LogMessage( "Time required: " + Convertor::Get( )->ToString( mInitializationTimer.GetElapsedTimeSecs( ) ) );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Reads in all initialisation files  
     @ param* initialisationFile The name of the initialization file with information on the simulations to be run
     @param outputPath The path to folder in which outputs will be stored
     //        /// <todo>Need to adjust this file to deal with incorrect inputs, extra columns etc by throwing an error</todo>
     //        /// <todo>Also need to strip leading spaces</todo>*/
    void ReadInitialisationFiles( ) {

        Logger::Get( )->LogMessage( "Reading functional group definitions..." );
        mInitialisationFileStrings["CohortFunctional"] = Constants::cCohortDefinitionsFileName;
        mCohortFunctionalGroupDefinitions = FunctionalGroupDefinitions( Constants::cCohortDefinitionsFileName );
        mInitialisationFileStrings["StockFunctional"] = Constants::cStockDefinitionsFileName;
        mStockFunctionalGroupDefinitions = FunctionalGroupDefinitions( Constants::cStockDefinitionsFileName );
        mModelMassBins.SetUpMassBins( Constants::cMassBinDefinitionsFileName );

        //assert( CellRarefaction >= 1 && "Cell rarefaction cannot be less than 1" );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Reads the environmental layers listed in the specified file containing a list of environmental layers
     @param environmentalLayerFile The name of the file containing the list of environmental layers
     @param outputPath The path to folder in which outputs will be stored */
    //    void ReadEnvironmentalLayers( string environmentalLayerFile, string outputPath, ModelGrid& Grid ) {
    //
    //        Logger::Get( )->LogMessage( "Reading in environmental data:" );
    //        std::ifstream infile( environmentalLayerFile.c_str( ) );
    //        //            // Declare lists to hold the information required to read the environmental layers
    //        Types::StringVector folders;
    //        Types::StringVector fileNames;
    //        Types::StringVector DatasetNames;
    //        Types::StringVector FileTypes;
    //        Types::StringVector LayerName;
    //        Types::StringVector StaticLayer;
    //        Types::StringVector Extensions;
    //        Types::StringVector Resolutions;
    //        Types::StringVector MethodUnits;
    //        
    //        if( infile.is_open( ) ) {
    //
    //            string l, header[9];
    //            getline( infile, l );
    //            //trim off newline character
    //            l.pop_back( );
    //            istringstream s( l );
    //            for( unsigned i = 0; i < 9; i++ ) {
    //                getline( s, header[i], ',' );
    //                transform( header[i].begin( ), header[i].end( ), header[i].begin( ), ::tolower );
    //                if( header[i] != "folder" &&
    //                        header[i] != "filename" &&
    //                        header[i] != "extension" &&
    //                        header[i] != "dataset name" &&
    //                        header[i] != "filetype" &&
    //                        header[i] != "internal layer name" &&
    //                        header[i] != "static" &&
    //                        header[i] != "resolution" &&
    //                        header[i] != "units" ) {
    //                    cout << "Bad header in environmentalLayerFile file " << environmentalLayerFile << endl;
    //                    exit( 1 );
    //                }
    //
    //            }
    //
    //
    //            while( infile.good( ) ) {
    //                string l, data;
    //                getline( infile, l );
    //                if( infile.good( ) )l.pop_back( );
    //                if( l.length( ) > 1 ) {
    //                    istringstream s( l );
    //                    for( unsigned i = 0; i < 9; i++ ) {
    //                        getline( s, data, ',' );
    //
    //                        if( header[i] == "folder" ) folders.push_back( data );
    //                        if( header[i] == "filename" ) fileNames.push_back( data );
    //                        if( header[i] == "extension" ) Extensions.push_back( data );
    //                        if( header[i] == "dataset name" ) DatasetNames.push_back( data );
    //                        if( header[i] == "filetype" ) FileTypes.push_back( data );
    //                        if( header[i] == "internal layer name" ) LayerName.push_back( data );
    //                        if( header[i] == "static" ) StaticLayer.push_back( data );
    //                        if( header[i] == "resolution" ) Resolutions.push_back( data );
    //                        if( header[i] == "units" ) MethodUnits.push_back( data );
    //
    //                    }
    //                }
    //            }
    //
    //        } else {
    //            cout << "Something wrong with environment parameter file " << environmentalLayerFile << endl;
    //            exit( 1 );
    //        }
    //        infile.close( );
    //
    //    }
    //----------------------------------------------------------------------------------------------

    /** \brief  Seed grid cells with cohorts, as specified in the model input files
    @param g A reference to a grid cell 
     */
    long SeedGridCellCohorts( GridCell& gcl ) {

        long totalCohorts = 0;
        // Set the seed for the random number generator from the system time
        unsigned seed = std::chrono::system_clock::now( ).time_since_epoch( ).count( );
        if( Parameters::Get( )->GetDrawRandomly( ) == true ) randomNumber.SetSeed( seed );
        else randomNumber.SetSeed( 1000 );


        unsigned numCohortsThisCell = 0;
        // Define local variables
        double cohortJuvenileMass;
        double cohortAdultMassRatio;
        double cohortAdultMass;
        double expectedLnAdultMassRatio;
        double totalNewBiomass = 0.0;
        double optimalPreyBodySizeRatio;


        //Variable for altering the juvenile to adult mass ratio for marine cells when handling certain functional groups eg baleen whales
        double scaling = 0.0;
        gcl.SetCohortSize( mCohortFunctionalGroupDefinitions.mAllFunctinoalGroupsIndex.size( ) );
        for( int FunctionalGroup : mCohortFunctionalGroupDefinitions.mAllFunctinoalGroupsIndex ) {
            int N = mCohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup( "Initial number of GridCellCohorts", FunctionalGroup );
            if( ( mCohortFunctionalGroupDefinitions.GetTraitNames( "Realm", FunctionalGroup ) == "terrestrial" && !gcl.IsMarine( ) ) ||
                    ( mCohortFunctionalGroupDefinitions.GetTraitNames( "Realm", FunctionalGroup ) == "marine" && gcl.IsMarine( ) ) ) {

                numCohortsThisCell += N;
            }
        }
        if( numCohortsThisCell > 0 );
        {
            //Loop over all functional groups in the model
            for( int functionalGroup : mCohortFunctionalGroupDefinitions.mAllFunctinoalGroupsIndex ) {
                // If it is a functional group that corresponds to the current realm, then seed cohorts
                if( ( mCohortFunctionalGroupDefinitions.GetTraitNames( "Realm", functionalGroup ) == "terrestrial" && !gcl.IsMarine( ) ) ||
                        ( mCohortFunctionalGroupDefinitions.GetTraitNames( "Realm", functionalGroup ) == "marine" && gcl.IsMarine( ) ) ) {
                    // Get the minimum and maximum possible body masses for organisms in each functional group
                    double massMinimum = mCohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup( "minimum mass", functionalGroup );
                    double massMaximum = mCohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup( "maximum mass", functionalGroup );

                    double proportionTimeActive = mCohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup( "proportion suitable time active", functionalGroup );

                    // Loop over the initial number of cohorts
                    unsigned numberOfCohortsInThisFunctionalGroup = 1;

                    numberOfCohortsInThisFunctionalGroup = mCohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup( "initial number of gridcellcohorts", functionalGroup );

                    for( unsigned jj = 0; jj < numberOfCohortsInThisFunctionalGroup; jj++ ) {
                        randomNumber.SetSeed((uint)(jj + 1), (uint)((jj + 1) * 3));

                        // Draw adult mass from a log-normal distribution with mean -6.9 and standard deviation 10.0,
                        // within the bounds of the minimum and maximum body masses for the functional group

                        cohortAdultMass = pow( 10, ( randomNumber.GetUniform( ) * ( log10( massMaximum ) - log10( 50 * massMinimum ) ) + log10( 50 * massMinimum ) ) );
                        //Changes from original code
                        optimalPreyBodySizeRatio = max( 0.01, randomNumber.GetNormal(0.1,0.02 ));

                        if( !gcl.IsMarine( ) ) {
                            do {
                                expectedLnAdultMassRatio = 2.24 + 0.13 * log( cohortAdultMass );

                                cohortAdultMassRatio = 1.0 + randomNumber.GetLogNormal(expectedLnAdultMassRatio, 0.5);
                                cohortJuvenileMass = cohortAdultMass * 1.0 / cohortAdultMassRatio;
                            } while( cohortAdultMass <= cohortJuvenileMass || cohortJuvenileMass < massMinimum );
                        } else{
                            do {
                                expectedLnAdultMassRatio = 2.24 + 0.13 * log( cohortAdultMass );

                                cohortAdultMassRatio = 1.0 + 10*randomNumber.GetLogNormal(expectedLnAdultMassRatio, 0.5);;
                                cohortJuvenileMass = cohortAdultMass * 1.0 / cohortAdultMassRatio;
                            } while( cohortAdultMass <= cohortJuvenileMass || cohortJuvenileMass < massMinimum );
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

                        double NewBiomass = ( 3300. / numCohortsThisCell ) * 100 * 3000 *
                                pow( 0.6, ( log10( cohortJuvenileMass ) ) ) * ( gcl.GetCellArea( ) );

                        totalNewBiomass += NewBiomass;
                        double NewAbund = 0.0;

                        NewAbund = NewBiomass / cohortJuvenileMass;

                        // Initialise the new cohort with the relevant properties
                        Cohort NewCohort( gcl, functionalGroup, cohortJuvenileMass, cohortAdultMass, cohortJuvenileMass, NewAbund,
                                optimalPreyBodySizeRatio, 0, proportionTimeActive, mNextCohortID );

                        // Add the new cohort to the list of grid cell cohorts
                        gcl.mCohorts[functionalGroup].push_back( NewCohort );

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
        for( int functionalGroupIndex : mStockFunctionalGroupDefinitions.mAllFunctinoalGroupsIndex ) {
            // Initialise the new stock with the relevant properties
            bool success;
            Stock NewStock( mStockFunctionalGroupDefinitions, functionalGroupIndex, gcl, success );
            // Add the new stock to the list of grid cell stocks
            if( success ) {
                gcl.mStocks[functionalGroupIndex].push_back( NewStock );

                totalStocks++;
            }
        }

        return totalStocks;
    }
    //----------------------------------------------------------------------------------------------

};
#endif

