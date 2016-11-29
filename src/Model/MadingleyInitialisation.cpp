#include "MadingleyInitialisation.h"

MadingleyInitialisation::MadingleyInitialisation( ) {
    mOutputPath = "";
    mNextCohortID = 0;
}

MadingleyInitialisation::MadingleyInitialisation( long long& nextCohortID, double& numberOfCohorts, double& numberOfStocks, Grid& modelGrid ) {
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

void MadingleyInitialisation::ReadInitialisationFiles( ) {

    Logger::Get( )->LogMessage( "Reading functional group definitions..." );
    mInitialisationFileStrings["CohortFunctional"] = Constants::cCohortDefinitionsFileName;
    mCohortFunctionalGroupDefinitions = FunctionalGroupDefinitions( Constants::cCohortDefinitionsFileName );
    mInitialisationFileStrings["StockFunctional"] = Constants::cStockDefinitionsFileName;
    mStockFunctionalGroupDefinitions = FunctionalGroupDefinitions( Constants::cStockDefinitionsFileName );
    mModelMassBins.SetUpMassBins( Constants::cMassBinDefinitionsFileName );

    //assert( CellRarefaction >= 1 && "Cell rarefaction cannot be less than 1" ); // FIX - Does this need to be uncommented?
}

long MadingleyInitialisation::SeedGridCellCohorts( GridCell& gcl ) {

    long totalCohorts = 0;
    // Set the seed for the random number generator from the system time
    unsigned seed = std::chrono::system_clock::now( ).time_since_epoch( ).count( );
    if( Parameters::Get( )->GetDrawRandomly( ) == true ) mRandomNumber.SetSeed( seed );
    else mRandomNumber.SetSeed( 1000 );


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
    for( int FunctionalGroup: mCohortFunctionalGroupDefinitions.mAllFunctinoalGroupsIndex ) {
        int N = mCohortFunctionalGroupDefinitions.GetBiologicalPropertyOneFunctionalGroup( "Initial number of GridCellCohorts", FunctionalGroup );
        if( ( mCohortFunctionalGroupDefinitions.GetTraitNames( "Realm", FunctionalGroup ) == "terrestrial" && !gcl.IsMarine( ) ) ||
                ( mCohortFunctionalGroupDefinitions.GetTraitNames( "Realm", FunctionalGroup ) == "marine" && gcl.IsMarine( ) ) ) {

            numCohortsThisCell += N;
        }
    }
    if( numCohortsThisCell > 0 );
    {
        //Loop over all functional groups in the model
        for( int functionalGroup: mCohortFunctionalGroupDefinitions.mAllFunctinoalGroupsIndex ) {
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
                    mRandomNumber.SetSeed( ( uint )( jj + 1 ), ( uint )( ( jj + 1 ) * 3 ) );

                    // Draw adult mass from a log-normal distribution with mean -6.9 and standard deviation 10.0,
                    // within the bounds of the minimum and maximum body masses for the functional group

                    cohortAdultMass = pow( 10, ( mRandomNumber.GetUniform( ) * ( log10( massMaximum ) - log10( 50 * massMinimum ) ) + log10( 50 * massMinimum ) ) );
                    //Changes from original code
                    optimalPreyBodySizeRatio = std::max( 0.01, mRandomNumber.GetNormal( 0.1, 0.02 ) );

                    if( !gcl.IsMarine( ) ) {
                        do {
                            expectedLnAdultMassRatio = 2.24 + 0.13 * log( cohortAdultMass );

                            cohortAdultMassRatio = 1.0 + mRandomNumber.GetLogNormal( expectedLnAdultMassRatio, 0.5 );
                            cohortJuvenileMass = cohortAdultMass * 1.0 / cohortAdultMassRatio;
                        } while( cohortAdultMass <= cohortJuvenileMass || cohortJuvenileMass < massMinimum );
                    } else {
                        do {
                            expectedLnAdultMassRatio = 2.24 + 0.13 * log( cohortAdultMass );

                            cohortAdultMassRatio = 1.0 + 10 * mRandomNumber.GetLogNormal( expectedLnAdultMassRatio, 0.5 );
                            ;
                            cohortJuvenileMass = cohortAdultMass * 1.0 / cohortAdultMassRatio;
                        } while( cohortAdultMass <= cohortJuvenileMass || cohortJuvenileMass < massMinimum );
                    }

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

long MadingleyInitialisation::SeedGridCellStocks( GridCell& gcl ) {

    long totalStocks = 0;

    // Loop over all stock functional groups in the model
    for( int functionalGroupIndex: mStockFunctionalGroupDefinitions.mAllFunctinoalGroupsIndex ) {
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

