#ifndef MADINGLEYMODEL_H
#define	MADINGLEYMODEL_H

#include <vector>
#include <map>
#include <MadingleyModelInitialisation.h>
#include <FunctionalGroupDefinitions.h>
#include <Stopwatch.h>
#include <CohortMerge.h>
#include <ModelGrid.h>
#include <GridCell.h>
#include <Dispersal.h>
#include <EcologyStock.h>
#include <EcologyCohort.h>
#include <Activity.h>
#include <ThreadLocked.h>
#include <fstream>
#include <Environment.h>

#include "DateTime.h"
#include "Maths.h"
#include "Parameters.h"
/// @todo check private versus public variables
/** \file MadingleyModel.h
 * \brief The main model header file
 * */

/** \brief The ecosystem model */
class MadingleyModel {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------       

    /** \brief An instance of ModelGrid to hold the grid to be used in this model */
    ModelGrid EcosystemModelGrid;

    /** \brief The current time step */
    unsigned CurrentTimeStep;
    /** \brief The current month: 1=Jan; 2=Feb; 3=Mar etc. */
    unsigned CurrentMonth;

    /** \brief A list of global diagnostics for this model run */
    map<string, double> GlobalDiagnosticVariables;
    /** \brief An instance of StopWatch to time individual time steps */
    StopWatch TimeStepTimer;
    StopWatch EcologyTimer;
    StopWatch DispersalTimer;
    StopWatch OutputTimer;
    StopWatch MergeTimer;
    /** \brief An array of instances of the output class to deal with grid cell outputs */
    //vector<OutputCell> CellOutputs;

    /** \brief An instance of the output class to deal with global outputs */
    //OutputGlobal GlobalOutputs;
    /** \brief An instance of the output class to deal with gridded outputs */
    //OutputGrid GridOutputs;

    /** A variable to increment for the purposes of giving each cohort a unique ID */
    long long NextCohortID;
    /** \brief Variable to track the number of cohorts that have dispersed. Doesn't need to be thread-local because all threads have converged prior to running cross-grid-cell processes */
    unsigned Dispersals;
    /** \brief Instance of the class to perform general functions */
    UtilityFunctions Utilities;
    /** \brief An instance of the merging class */
    CohortMerge CohortMerger;
    /** \brief An instance of the mersenne twister random number generator class */
    std::mt19937_64 RandomNumberGenerator;
    //
    MadingleyModelInitialisation params;
    //Because the parameters are initialised late (!!!!!!!PARAMETERS::GET() levaes parameters uninitialised!!!!!!!ARG!!!!!)
    //this has to be a pointer
    Dispersal* disperser;
    ofstream outputFile;
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief   Initializes the ecosystem model
    @param initialisationFileName The name of the file with model parameters
    @param OutputPath Where the output will be stored
     */
    MadingleyModel( ) {
        // Set up list of global diagnostics
        SetUpGlobalDiagnosticsList( );
        // Initialise the cohort ID to zero
        NextCohortID = 0;
        params = MadingleyModelInitialisation(
                NextCohortID,
                GlobalDiagnosticVariables["NumberOfCohortsInModel"],
                GlobalDiagnosticVariables["NumberOfStocksInModel"],
                EcosystemModelGrid );
        disperser=new Dispersal();

        // Set up model outputs
        SetUpOutputs( );
        //end of initialisations
    }
    //----------------------------------------------------------------------------------------------

    /** \brief  Run the global ecosystem model     */
    void RunMadingley( ) {
        // Write out model run details to the console
        cout << "Running model" << endl;
        cout << "Number of time steps is: " << Parameters::Get( )->GetLengthOfSimulationInTimeSteps( ) << endl;


        Dispersals = 0;
        /// Run the model
        for( unsigned timeStep = 0; timeStep < Parameters::Get( )->GetLengthOfSimulationInTimeSteps( ); timeStep += 1 ) {

            DateTime::Get( )->SetTimeStep( timeStep );

            cout << "Running time step " << timeStep + 1 << "..." << endl;
            // Start the timer
            TimeStepTimer.Start( );
            // Get current time step and month
            CurrentTimeStep = timeStep;
            CurrentMonth = Utilities.GetCurrentMonth( timeStep );
            EcologyTimer.Start( );

            Environment::update( CurrentMonth );

            RunWithinCells( );
            EcologyTimer.Stop( );
            cout << "Within grid ecology took: " << EcologyTimer.GetElapsedTimeSecs( ) << endl;

            DispersalTimer.Start( );

            RunCrossGridCellEcology( Dispersals );
            DispersalTimer.Stop( );
            cout << "Across grid ecology took: " << DispersalTimer.GetElapsedTimeSecs( ) << endl;

            TimeStepTimer.Stop( );
            OutputTimer.Start( );
            Output( timeStep );
            OutputTimer.Stop( );
            cout << "Global Outputs took: " << OutputTimer.GetElapsedTimeSecs( ) << endl;

            // Write the results of dispersal to the console
            cout << "Total Cohorts remaining " << GlobalDiagnosticVariables["NumberOfCohortsInModel"] << endl;


        }

    }//----------------------------------------------------------------------------------------------

    /** \brief  Run processes for cells*/
    void RunWithinCells( ) {
        // Instantiate a class to hold thread locked global diagnostic variables
        ThreadLockedParallelVariables singleThreadDiagnostics( 0, 0, 0, NextCohortID );

        EcosystemModelGrid.ApplyFunctionToAllCells( [&]( GridCell & gcl ) {

            gcl.RandomizeCohorts( );

            RunWithinCellStockEcology( gcl );

            RunWithinCellCohortEcology( gcl, singleThreadDiagnostics );

        } );
        // Update the variable tracking cohort unique IDs
        NextCohortID = singleThreadDiagnostics.NextCohortIDThreadLocked;
        // Take the results from the thread local variables and apply to the global diagnostic variables
        GlobalDiagnosticVariables["NumberOfCohortsExtinct"] = singleThreadDiagnostics.Extinctions - singleThreadDiagnostics.Combinations;
        GlobalDiagnosticVariables["NumberOfCohortsProduced"] = singleThreadDiagnostics.Productions;
        GlobalDiagnosticVariables["NumberOfCohortsInModel"] = GlobalDiagnosticVariables["NumberOfCohortsInModel"] + singleThreadDiagnostics.Productions - singleThreadDiagnostics.Extinctions;
        GlobalDiagnosticVariables["NumberOfCohortsCombined"] = singleThreadDiagnostics.Combinations;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief   Run ecological processes for stocks in a specified grid cell
    @param gcl The current cell 
     */
    void RunWithinCellStockEcology( GridCell& gcl ) {

        // Create a local instance of the stock ecology class
        EcologyStock MadingleyEcologyStock;
        // Get the list of functional group indices for autotroph stocks
        vector<int> AutotrophStockFunctionalGroups = params.StockFunctionalGroupDefinitions.GetFunctionalGroupIndex( "Heterotroph/Autotroph", "Autotroph", false );
        // Loop over autotroph functional groups
        for( unsigned FunctionalGroup: AutotrophStockFunctionalGroups ) {
            for( auto& ActingStock: gcl.mGridCellStocks[FunctionalGroup] ) {

                // Run stock ecology
                MadingleyEcologyStock.RunWithinCellEcology( gcl, ActingStock, CurrentTimeStep, CurrentMonth, params );
            }
        }

    }
    //----------------------------------------------------------------------------------------------

    /** \brief   Run ecological processes for cohorts in a specified grid cell
    @param gcl Reference to the current grid cell
    @param partial Track some global variables pertaining to cohort numbers etc.

     * NB - need to take care here when cohort updates get applied

     */
    void RunWithinCellCohortEcology( GridCell& gcl, ThreadLockedParallelVariables& partial ) {
        // Local instances of classes
        // Initialize ecology for stocks and cohorts - needed fresh every timestep?

        EcologyCohort mEcologyCohort;
        mEcologyCohort.initialiseEating( gcl, params );
        Activity CohortActivity;

        // Loop over randomly ordered gridCellCohorts to implement biological functions

        gcl.ApplyFunctionToAllCohorts( [&]( Cohort & c ) {
            // Perform all biological functions except dispersal (which is cross grid cell)

            if( gcl.mGridCellCohorts[c.mFunctionalGroupIndex].size( ) != 0 && c.mCohortAbundance > Parameters::Get( )->GetExtinctionThreshold( ) ) {

                CohortActivity.AssignProportionTimeActive( gcl, c, CurrentTimeStep, CurrentMonth, params );

                // Run ecology
                mEcologyCohort.RunWithinCellEcology( gcl, c, CurrentTimeStep, partial, CurrentMonth, params );
                // Update the properties of the acting cohort
                mEcologyCohort.UpdateEcology( gcl, c, CurrentTimeStep );
                Cohort::ResetMassFluxes( );
                // Check that the mass of individuals in this cohort is still >= 0 after running ecology
                assert( c.mIndividualBodyMass >= 0.0 && "Biomass < 0 for this cohort" );
            }

            // Check that the mass of individuals in this cohort is still >= 0 after running ecology
            if( gcl.mGridCellCohorts[c.mFunctionalGroupIndex].size( ) > 0 )assert( c.mIndividualBodyMass >= 0.0 && "Biomass < 0 for this cohort" );

        } );

        for( auto& c: Cohort::mNewCohorts ) {
            gcl.InsertCohort( c );
            if( c.mDestination != &gcl )cout << "whut? wrong cell?" << endl;
        }
        partial.Productions += Cohort::mNewCohorts.size( );
        Cohort::mNewCohorts.clear( );

        RunExtinction( gcl, partial );

        // Merge cohorts, if necessary
        if( gcl.GetNumberOfCohorts( ) > Parameters::Get( )->GetMaximumNumberOfCohorts( ) ) {

            partial.Combinations += CohortMerger.MergeToReachThresholdFast( gcl, params );

            //Run extinction a second time to remove those cohorts that have been set to zero abundance when merging
            RunExtinction( gcl, partial );

        }
    }

    //----------------------------------------------------------------------------------------------

    /** \brief Carries out extinction on cohorts that have an abundance below a defined extinction threshold */
    void RunExtinction( GridCell& gcl, ThreadLockedParallelVariables& partial ) {

        // Loop over cohorts and remove any whose abundance is below the extinction threshold
        vector<Cohort>CohortsToRemove;
        gcl.ApplyFunctionToAllCohorts( [&]( Cohort & c ) {
            if( c.mCohortAbundance - Parameters::Get( )->GetExtinctionThreshold( )<=0 || c.mIndividualBodyMass <= 0 ) {
                CohortsToRemove.push_back( c );
                partial.Extinctions += 1;
            }
        } );

        // Code to add the biomass to the biomass pool and dispose of the cohort
        for( auto& c: CohortsToRemove ) {

            // Add biomass of the extinct cohort to the organic matter pool
            double deadMatter = ( c.mIndividualBodyMass + c.mIndividualReproductivePotentialMass ) * c.mCohortAbundance;
            if( deadMatter < 0 ) cout << "Dead " << deadMatter << endl;
            Environment::Get( "Organic Pool", c.GetCurrentLocation( ) ) += deadMatter;
            assert( Environment::Get( "Organic Pool", c.GetCurrentLocation( ) ) >= 0 && "Organic pool < 0" );

            // Remove the extinct cohort from the list of cohorts
            gcl.RemoveCohort( c );

        }

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Run ecological processes that operate across grid cells */
    void RunCrossGridCellEcology( unsigned& dispersals ) {
        // Loop through each grid cell, and run dispersal for each.
        // In the original model a new dispersal object is made every timestep - this resets the random number generators
        disperser->resetRandoms();
        EcosystemModelGrid.ApplyFunctionToAllCells( [&]( GridCell & c ) {
            disperser->RunCrossGridCellEcologicalProcess( c, EcosystemModelGrid, params, CurrentMonth );
        } );

        // Apply the changes from dispersal
        disperser->UpdateCrossGridCellEcology( dispersals );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief   Sets up the list of global diagnostic variables
     */
    void SetUpGlobalDiagnosticsList( ) {
        // Add global diagnostic variables
        GlobalDiagnosticVariables["NumberOfCohortsExtinct"] = 0.0;
        GlobalDiagnosticVariables["NumberOfCohortsProduced"] = 0.0;
        GlobalDiagnosticVariables["NumberOfCohortsCombined"] = 0.0;
        GlobalDiagnosticVariables["NumberOfCohortsInModel"] = 0.0;
        GlobalDiagnosticVariables["NumberOfStocksInModel"] = 0.0;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief   Sets up the model outputs
    @param initialisation An instance of the model initialisation class
     */
    void SetUpOutputs( ) {
        outputFile.open( "LATEST_OUTPUT" );
        outputFile << "step dispersals extinctions productions combinations totalcohorts totalstocks totalAbundance organicPool respiratoryPool totalStockBiomass totalCohortBiomass totalLivingBiomass totalBiomass incelltime dispersaltime" << endl;
    }
    //----------------------------------------------------------------------------------------------

    void Output( unsigned step ) {

        double organicPool = 0, respiratoryPool = 0, totalAbundance = 0;
        double totalStockBiomass = 0, totalCohortBiomass = 0;long totalCohorts=0;
        EcosystemModelGrid.ApplyFunctionToAllCells( [&]( GridCell & gcl ) {
            organicPool += Environment::Get( "Organic Pool", gcl ) / 1000.;
            respiratoryPool += Environment::Get( "Respiratory CO2 Pool", gcl ) / 1000.;
                    gcl.ApplyFunctionToAllCohorts( [&]( Cohort & c ) {
                        totalCohorts+=1;
                        totalAbundance += c.mCohortAbundance;

                        double CohortBiomass = ( c.mIndividualBodyMass + c.mIndividualReproductivePotentialMass ) * c.mCohortAbundance / 1000.;
                        totalCohortBiomass += CohortBiomass;
                    } );
            gcl.ApplyFunctionToAllStocks( [&]( Stock & s ) {
                totalStockBiomass += s.TotalBiomass / 1000.; //convert from g to kg
            } );
        } );
        double totalLivingBiomass = totalCohortBiomass + totalStockBiomass;
        double totalBiomass = totalCohortBiomass + totalStockBiomass + respiratoryPool + organicPool;
        
        outputFile << step << " " << Dispersals << " " << GlobalDiagnosticVariables["NumberOfCohortsExtinct"] << " " << GlobalDiagnosticVariables["NumberOfCohortsProduced"] << " " << GlobalDiagnosticVariables["NumberOfCohortsCombined"] << " " << GlobalDiagnosticVariables["NumberOfCohortsInModel"] << " " << GlobalDiagnosticVariables["NumberOfStocksInModel"] << " " << totalAbundance << " " << organicPool << " " << respiratoryPool << " " << totalStockBiomass << " " << totalCohortBiomass << " " << totalLivingBiomass << " " << totalBiomass << " " << EcologyTimer.GetElapsedTimeSecs( ) << " " << DispersalTimer.GetElapsedTimeSecs( ) << endl;
    }

};

#endif

