#ifndef MADINGLEYMODEL_H
#define	MADINGLEYMODEL_H

#include "MadingleyModelInitialisation.h"
#include "FunctionalGroupDefinitions.h"
#include "Stopwatch.h"
#include "CohortMerge.h"
#include "ModelGrid.h"
#include "GridCell.h"
#include "Dispersal.h"
#include "EcologyStock.h"
#include "EcologyCohort.h"
#include "Activity.h"
#include "ThreadLocked.h"
#include "Environment.h"

#include "Time.h"
#include "Logger.h"
#include "Maths.h"
#include "Parameters.h"
#include "DataRecorder.h"
#include "Logger.h"
#include "Types.h"
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
    ModelGrid mModelGrid;

    /** \brief The current time step */
    unsigned mCurrentTimeStep;
    /** \brief The current month: 1=Jan; 2=Feb; 3=Mar etc. */
    unsigned mCurrentMonth;

    /** \brief A list of global diagnostics for this model run */
    Types::DoubleMap mGlobalDiagnosticVariables;
    /** \brief An instance of StopWatch to time individual time steps */
    StopWatch mEcologyTimer;
    StopWatch mDispersalTimer;
    StopWatch mOutputTimer;
    StopWatch mMergeTimer;
    /** \brief An array of instances of the output class to deal with grid cell outputs */
    //vector<OutputCell> CellOutputs;

    /** \brief An instance of the output class to deal with global outputs */
    //OutputGlobal GlobalOutputs;
    /** \brief An instance of the output class to deal with gridded outputs */
    //OutputGrid GridOutputs;

    /** A variable to increment for the purposes of giving each cohort a unique ID */
    long long mNextCohortID;
    /** \brief Variable to track the number of cohorts that have dispersed. Doesn't need to be thread-local because all threads have converged prior to running cross-grid-cell processes */
    unsigned mDispersals;
    /** \brief Instance of the class to perform general functions */
    UtilityFunctions mUtilities;
    /** \brief An instance of the merging class */
    CohortMerge mCohortMerger;
    /** \brief An instance of the mersenne twister random number generator class */
    std::mt19937_64 mRandomNumberGenerator;
    MadingleyModelInitialisation mParams;
    Types::DispersalPointer mDisperser; //FIX - Does this need to be a pointer?

    Types::StringVector mStockLeafStrategy;
    Types::StringVector mCohortNutritionSource;
    Types::StringVector mCohortThermoregulation;
    Types::StringVector mCohortReproductiveStrategy;

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
        mNextCohortID = 0;
        mParams = MadingleyModelInitialisation(
                mNextCohortID,
                mGlobalDiagnosticVariables["NumberOfCohortsInModel"],
                mGlobalDiagnosticVariables["NumberOfStocksInModel"],
                mModelGrid );
        mDisperser = new Dispersal( );

        mStockLeafStrategy = mParams.mStockFunctionalGroupDefinitions.mTraitLookupFromIndex[ "leaf strategy" ];
        mCohortNutritionSource = mParams.mCohortFunctionalGroupDefinitions.mTraitLookupFromIndex[ "nutrition source" ];
        mCohortThermoregulation = mParams.mCohortFunctionalGroupDefinitions.mTraitLookupFromIndex[ "endo/ectotherm" ];
        mCohortReproductiveStrategy = mParams.mCohortFunctionalGroupDefinitions.mTraitLookupFromIndex[ "reproductive strategy" ];
    }
    //----------------------------------------------------------------------------------------------

    /** \brief  Run the global ecosystem model     */
    void RunMadingley( ) {
        // Write out model run details to the console
        Logger::Get( )->LogMessage( "Running model" );
        Logger::Get( )->LogMessage( "Number of time steps is: " + Convertor::Get( )->ToString( Parameters::Get( )->GetLengthOfSimulationInMonths( ) ) );

        mDispersals = 0;
        /// Run the model
        for( unsigned timeStep = 0; timeStep < Parameters::Get( )->GetLengthOfSimulationInMonths( ); timeStep += 1 ) {

            Time::Get( )->SetMonthlyTimeStep( timeStep );

            Logger::Get( )->LogMessage( "Running time step " + Convertor::Get( )->ToString( timeStep + 1 ) + "..." );

            // Get current time step and month
            mCurrentTimeStep = timeStep;
            mCurrentMonth = mUtilities.GetCurrentMonth( timeStep );
            mEcologyTimer.Start( );

            Environment::update( mCurrentMonth );

            RunWithinCells( );
            mEcologyTimer.Stop( );
            Logger::Get( )->LogMessage( "Within grid ecology took: " + Convertor::Get( )->ToString( mEcologyTimer.GetElapsedTimeSecs( ) ) );

            mDispersalTimer.Start( );

            RunCrossGridCellEcology( mDispersals );
            mDispersalTimer.Stop( );
            Logger::Get( )->LogMessage( "Across grid ecology took: " + Convertor::Get( )->ToString( mDispersalTimer.GetElapsedTimeSecs( ) ) );

            mOutputTimer.Start( );
            Output( timeStep );
            mOutputTimer.Stop( );
            Logger::Get( )->LogMessage( "Global Outputs took: " + Convertor::Get( )->ToString( mOutputTimer.GetElapsedTimeSecs( ) ) );

            // Write the results of dispersal to the console
            Logger::Get( )->LogMessage( "Total Cohorts remaining " + Convertor::Get( )->ToString( mGlobalDiagnosticVariables["NumberOfCohortsInModel"] ) );
        }
    }//----------------------------------------------------------------------------------------------

    /** \brief  Run processes for cells*/
    void RunWithinCells( ) {
        // Instantiate a class to hold thread locked global diagnostic variables
        ThreadLockedParallelVariables singleThreadDiagnostics( 0, 0, 0, mNextCohortID );

        mModelGrid.ApplyFunctionToAllCells( [&]( GridCell & gcl ) {

            RunWithinCellStockEcology( gcl );

                    RunWithinCellCohortEcology( gcl, singleThreadDiagnostics );

        } );
        // Update the variable tracking cohort unique IDs
        mNextCohortID = singleThreadDiagnostics.NextCohortIDThreadLocked;
        // Take the results from the thread local variables and apply to the global diagnostic variables
        mGlobalDiagnosticVariables["NumberOfCohortsExtinct"] = singleThreadDiagnostics.Extinctions - singleThreadDiagnostics.Combinations;
        mGlobalDiagnosticVariables["NumberOfCohortsProduced"] = singleThreadDiagnostics.Productions;
        mGlobalDiagnosticVariables["NumberOfCohortsInModel"] = mGlobalDiagnosticVariables["NumberOfCohortsInModel"] + singleThreadDiagnostics.Productions - singleThreadDiagnostics.Extinctions;
        mGlobalDiagnosticVariables["NumberOfCohortsCombined"] = singleThreadDiagnostics.Combinations;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief   Run ecological processes for stocks in a specified grid cell
    @param gcl The current cell 
     */
    void RunWithinCellStockEcology( GridCell& gcl ) {

        // Create a local instance of the stock ecology class
        EcologyStock MadingleyEcologyStock;
        // Get the list of functional group indices for autotroph stocks
        vector<int> AutotrophStockFunctionalGroups = mParams.mStockFunctionalGroupDefinitions.GetFunctionalGroupIndex( "Heterotroph/Autotroph", "Autotroph", false );
        // Loop over autotroph functional groups
        for( unsigned FunctionalGroup : AutotrophStockFunctionalGroups ) {
            for( auto& ActingStock : gcl.mGridCellStocks[FunctionalGroup] ) {

                // Run stock ecology
                MadingleyEcologyStock.RunWithinCellEcology( gcl, ActingStock, mCurrentTimeStep, mCurrentMonth, mParams );
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
        mEcologyCohort.initialiseEating( gcl, mParams );
        Activity CohortActivity;

        // Loop over randomly ordered gridCellCohorts to implement biological functions

        gcl.ApplyFunctionToAllCohortsWithStaticRandomness( [&]( Cohort & c ) {
            // Perform all biological functions except dispersal (which is cross grid cell)

            if( gcl.mGridCellCohorts[c.mFunctionalGroupIndex].size( ) != 0 && c.mCohortAbundance > Parameters::Get( )->GetExtinctionThreshold( ) ) {

                CohortActivity.AssignProportionTimeActive( gcl, c, mCurrentTimeStep, mCurrentMonth, mParams );

                // Run ecology
                mEcologyCohort.RunWithinCellEcology( gcl, c, mCurrentTimeStep, partial, mCurrentMonth, mParams );
                // Update the properties of the acting cohort
                mEcologyCohort.UpdateEcology( gcl, c, mCurrentTimeStep );
                Cohort::ResetMassFluxes( );
                // Check that the mass of individuals in this cohort is still >= 0 after running ecology
                assert( c.mIndividualBodyMass >= 0.0 && "Biomass < 0 for this cohort" );
            }

            // Check that the mass of individuals in this cohort is still >= 0 after running ecology
            if( gcl.mGridCellCohorts[c.mFunctionalGroupIndex].size( ) > 0 )assert( c.mIndividualBodyMass >= 0.0 && "Biomass < 0 for this cohort" );

        }, mCurrentTimeStep );

        for( auto& c : Cohort::mNewCohorts ) {
            gcl.InsertCohort( c );
            if( c.mDestinationCell != &gcl ) Logger::Get( )->LogMessage( "whut? wrong cell?" );
        }
        partial.Productions += Cohort::mNewCohorts.size( );
        Cohort::mNewCohorts.clear( );

        RunExtinction( gcl, partial );

        // Merge cohorts, if necessary
        if( gcl.GetNumberOfCohorts( ) > Parameters::Get( )->GetMaximumNumberOfCohorts( ) ) {

            partial.Combinations += mCohortMerger.MergeToReachThresholdFast( gcl, mParams );

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
            if( c.mCohortAbundance - Parameters::Get( )->GetExtinctionThreshold( ) <= 0 || c.mIndividualBodyMass <= 0 ) {
                CohortsToRemove.push_back( c );
                partial.Extinctions += 1;
            }
        } );

        // Code to add the biomass to the biomass pool and dispose of the cohort
        for( auto& c : CohortsToRemove ) {

            // Add biomass of the extinct cohort to the organic matter pool
            double deadMatter = ( c.mIndividualBodyMass + c.mIndividualReproductivePotentialMass ) * c.mCohortAbundance;
            if( deadMatter < 0 ) Logger::Get( )->LogMessage( "Dead " + Convertor::Get( )->ToString( deadMatter ) );
            Environment::Get( "Organic Pool", c.GetCurrentCell( ) ) += deadMatter;
            assert( Environment::Get( "Organic Pool", c.GetCurrentCell( ) ) >= 0 && "Organic pool < 0" );

            // Remove the extinct cohort from the list of cohorts
            gcl.RemoveCohort( c );

        }
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Run ecological processes that operate across grid cells */
    void RunCrossGridCellEcology( unsigned& dispersals ) {
        // Loop through each grid cell, and run dispersal for each.
        // In the original model a new dispersal object is made every timestep - this resets the random number generators
        mDisperser->ResetRandoms( );
        mModelGrid.ApplyFunctionToAllCells( [&]( GridCell & c ) {
            mDisperser->RunCrossGridCellEcologicalProcess( c, mModelGrid, mParams, mCurrentMonth );
        } );

        // Apply the changes from dispersal
        mDisperser->UpdateCrossGridCellEcology( dispersals );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief   Sets up the list of global diagnostic variables
     */
    void SetUpGlobalDiagnosticsList( ) {
        // Add global diagnostic variables
        mGlobalDiagnosticVariables["NumberOfCohortsExtinct"] = 0.0;
        mGlobalDiagnosticVariables["NumberOfCohortsProduced"] = 0.0;
        mGlobalDiagnosticVariables["NumberOfCohortsCombined"] = 0.0;
        mGlobalDiagnosticVariables["NumberOfCohortsInModel"] = 0.0;
        mGlobalDiagnosticVariables["NumberOfStocksInModel"] = 0.0;
    }
    //----------------------------------------------------------------------------------------------

    void Output( unsigned step ) {
        double totalLivingBiomass = 0;
        double totalBiomass = 0;

        double organicMatterPool = 0;
        double respiratoryPool = 0;

        double totalStockBiomass = 0;

        double totalCohortBiomass = 0;
        long totalCohorts = 0;
        double totalCohortAbundance = 0;

        mModelGrid.ApplyFunctionToAllCells( [&]( GridCell & gridCell ) {
            double organicMatterThisCell = Environment::Get( "Organic Pool", gridCell ) / 1000.;
            double respirationThisCell = Environment::Get( "Respiratory CO2 Pool", gridCell ) / 1000.;

                    double cohortBiomassThisCell = 0;
                    double stockBiomassThisCell = 0;

                    double phytoplanktonBiomassThisCell = 0;
                    double evergreenBiomassThisCell = 0;
                    double deciduousBiomassThisCell = 0;

                    double cohortAbundanceThisCell = 0;
                    double herbivoreBiomassThisCell = 0;
                    double herbivoreAbundanceThisCell = 0;
                    double omnivoreBiomassThisCell = 0;
                    double omnivoreAbundanceThisCell = 0;
                    double carnivoreBiomassThisCell = 0;
                    double carnivoreAbundanceThisCell = 0;

                    double ectothermBiomassThisCell = 0;
                    double ectothermAbundanceThisCell = 0;
                    double endothermBiomassThisCell = 0;
                    double endothermAbundanceThisCell = 0;

                    double iteroparousBiomassThisCell = 0;
                    double iteroparousAbundanceThisCell = 0;
                    double semelparousBiomassThisCell = 0;
                    double semelparousAbundanceThisCell = 0;

                    organicMatterPool += organicMatterThisCell;
                    respiratoryPool += respirationThisCell;

                    gridCell.ApplyFunctionToAllCohorts( [&]( Cohort & c ) {
                        totalCohorts += 1;
                        totalCohortAbundance += c.mCohortAbundance;

                                double cohortBiomass = ( c.mIndividualBodyMass + c.mIndividualReproductivePotentialMass ) * c.mCohortAbundance / 1000.;
                                cohortBiomassThisCell += cohortBiomass;
                                totalCohortBiomass += cohortBiomass;
                                cohortAbundanceThisCell += c.mCohortAbundance;

                        if( mCohortNutritionSource[ c.mFunctionalGroupIndex ] == "herbivore" ) {
                            herbivoreBiomassThisCell += c.mIndividualBodyMass;
                                    herbivoreAbundanceThisCell += c.mCohortAbundance;
                        } else if( mCohortNutritionSource[ c.mFunctionalGroupIndex ] == "omnivore" ) {
                            omnivoreBiomassThisCell += c.mIndividualBodyMass;
                                    omnivoreAbundanceThisCell += c.mCohortAbundance;
                        } else if( mCohortNutritionSource[ c.mFunctionalGroupIndex ] == "carnivore" ) {
                            carnivoreBiomassThisCell += c.mIndividualBodyMass;
                                    carnivoreAbundanceThisCell += c.mCohortAbundance;
                        }

                        if( mCohortThermoregulation[ c.mFunctionalGroupIndex ] == "endotherm" ) {
                            ectothermBiomassThisCell += c.mIndividualBodyMass;
                                    ectothermAbundanceThisCell += c.mCohortAbundance;
                        } else if( mCohortThermoregulation[ c.mFunctionalGroupIndex ] == "ectotherm" ) {
                            endothermBiomassThisCell += c.mIndividualBodyMass;
                                    endothermAbundanceThisCell += c.mCohortAbundance;
                        }

                        if( mCohortReproductiveStrategy[ c.mFunctionalGroupIndex ] == "iteroparity" ) {
                            iteroparousBiomassThisCell += c.mIndividualBodyMass;
                                    iteroparousAbundanceThisCell += c.mCohortAbundance;
                        } else if( mCohortReproductiveStrategy[ c.mFunctionalGroupIndex ] == "semelparity" ) {
                            semelparousBiomassThisCell += c.mIndividualBodyMass;
                                    semelparousAbundanceThisCell += c.mCohortAbundance;
                        }
                    } );
            gridCell.ApplyFunctionToAllStocks( [&]( Stock & s ) {
                double thisStockBiomass = s.TotalBiomass / 1000.;
                stockBiomassThisCell += thisStockBiomass; //convert from g to kg
                        totalStockBiomass += thisStockBiomass;

                if( mStockLeafStrategy[ s.FunctionalGroupIndex ] == "na" ) phytoplanktonBiomassThisCell += thisStockBiomass;
                else if( mStockLeafStrategy[ s.FunctionalGroupIndex ] == "deciduous" ) deciduousBiomassThisCell += thisStockBiomass;
                else if( mStockLeafStrategy[ s.FunctionalGroupIndex ] == "evergreen" ) evergreenBiomassThisCell += thisStockBiomass;
                } );

            double biomassThisCell = cohortBiomassThisCell + stockBiomassThisCell + respirationThisCell + organicMatterThisCell;

                    DataRecorder::Get( )->SetDataOn( "BiomassDensity", gridCell.GetIndex( ), biomassThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "AbundanceDensity", gridCell.GetIndex( ), cohortAbundanceThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "AutotrophBiomassDensity", gridCell.GetIndex( ), stockBiomassThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "HeterotrophBiomassDensity", gridCell.GetIndex( ), cohortBiomassThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "PhytoplanktonBiomassDensity", gridCell.GetIndex( ), phytoplanktonBiomassThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "DeciduousBiomassDensity", gridCell.GetIndex( ), deciduousBiomassThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "EvergreenBiomassDensity", gridCell.GetIndex( ), evergreenBiomassThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "HerbivoreBiomassDensity", gridCell.GetIndex( ), herbivoreBiomassThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "HerbivoreAbundanceDensity", gridCell.GetIndex( ), herbivoreAbundanceThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "OmnivoreBiomassDensity", gridCell.GetIndex( ), omnivoreBiomassThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "OmnivoreAbundanceDensity", gridCell.GetIndex( ), omnivoreAbundanceThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "CarnivoreBiomassDensity", gridCell.GetIndex( ), carnivoreBiomassThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "CarnivoreAbundanceDensity", gridCell.GetIndex( ), carnivoreAbundanceThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "EctothermBiomassDensity", gridCell.GetIndex( ), ectothermBiomassThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "EctothermAbundanceDensity", gridCell.GetIndex( ), ectothermAbundanceThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "EndothermBiomassDensity", gridCell.GetIndex( ), endothermBiomassThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "EndothermAbundanceDensity", gridCell.GetIndex( ), endothermAbundanceThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "IteroparityBiomassDensity", gridCell.GetIndex( ), iteroparousBiomassThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "IteroparityAbundanceDensity", gridCell.GetIndex( ), iteroparousAbundanceThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "SemelparityBiomassDensity", gridCell.GetIndex( ), semelparousBiomassThisCell / gridCell.GetCellArea( ) );
                    DataRecorder::Get( )->SetDataOn( "SemelparityAbundanceDensity", gridCell.GetIndex( ), semelparousAbundanceThisCell / gridCell.GetCellArea( ) );

        } );
        totalLivingBiomass = totalCohortBiomass + totalStockBiomass;
        totalBiomass = totalCohortBiomass + totalStockBiomass + respiratoryPool + organicMatterPool;

        DataRecorder::Get( )->SetDataOn( "InCellTime", mEcologyTimer.GetElapsedTimeSecs( ) );
        DataRecorder::Get( )->SetDataOn( "DispersalTime", mDispersalTimer.GetElapsedTimeSecs( ) );

        DataRecorder::Get( )->SetDataOn( "TotalBiomass", totalBiomass );
        DataRecorder::Get( )->SetDataOn( "TotalLivingBiomass", totalLivingBiomass );
        DataRecorder::Get( )->SetDataOn( "TotalStockBiomass", totalStockBiomass );
        DataRecorder::Get( )->SetDataOn( "TotalCohortBiomass", totalCohortBiomass );
        DataRecorder::Get( )->SetDataOn( "OrganicMatterPool", organicMatterPool );
        DataRecorder::Get( )->SetDataOn( "RespiratoryCO2Pool", respiratoryPool );

        DataRecorder::Get( )->SetDataOn( "NumberOfStocks", mGlobalDiagnosticVariables["NumberOfStocksInModel"] );
        DataRecorder::Get( )->SetDataOn( "NumberOfCohorts", mGlobalDiagnosticVariables["NumberOfCohortsInModel"] );

        DataRecorder::Get( )->SetDataOn( "CohortsProduced", mGlobalDiagnosticVariables["NumberOfCohortsProduced"] );
        DataRecorder::Get( )->SetDataOn( "CohortsExtinct", mGlobalDiagnosticVariables["NumberOfCohortsExtinct"] );
        DataRecorder::Get( )->SetDataOn( "CohortsCombined", mGlobalDiagnosticVariables["NumberOfCohortsCombined"] );
        DataRecorder::Get( )->SetDataOn( "CohortsDispersed", mDispersals );
        DataRecorder::Get( )->SetDataOn( "CohortAbundance", totalCohortAbundance );
    }
};

#endif

