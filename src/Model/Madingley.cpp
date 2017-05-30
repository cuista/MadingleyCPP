#include "Madingley.h"
#include <omp.h>

Madingley::Madingley( ) {
    // Set up list of global diagnostics
    SetUpGlobalDiagnosticsList( );
    // Initialise the cohort ID to zero
    mNextCohortID = 0;
    mParams = MadingleyInitialisation( mNextCohortID, mGlobalDiagnosticVariables["NumberOfCohortsInModel"], mGlobalDiagnosticVariables["NumberOfStocksInModel"], mModelGrid );
    mDispersalSet = new DispersalSet( );

    mStockLeafStrategy = mParams.mStockFunctionalGroupDefinitions.mTraitLookupFromIndex[ "leaf strategy" ];
    mCohortNutritionSource = mParams.mCohortFunctionalGroupDefinitions.mTraitLookupFromIndex[ "nutrition source" ];
    mCohortThermoregulation = mParams.mCohortFunctionalGroupDefinitions.mTraitLookupFromIndex[ "endo/ectotherm" ];
    mCohortReproductiveStrategy = mParams.mCohortFunctionalGroupDefinitions.mTraitLookupFromIndex[ "reproductive strategy" ];
}

void Madingley::Run( ) {
    // Write out model run details to the console
    std::cout << "Running model" << std::endl;
    std::cout << "Number of time steps is: " << Parameters::Get( )->GetLengthOfSimulationInMonths( ) << std::endl;

    mDispersals = 0;
    /// Run the model
    for( unsigned timeStep = 0; timeStep < Parameters::Get( )->GetLengthOfSimulationInMonths( ); timeStep += 1 ) {

        TimeStep::Get( )->SetMonthly( timeStep );

        std::cout << "Running time step " << timeStep + 1 << "..." << std::endl;

        // Get current time step and month
        mCurrentTimeStep = timeStep;
        mCurrentMonth = mUtilities.GetCurrentMonth( timeStep );
        mEcologyTimer.Start( );

        Environment::Update( mCurrentMonth );

        RunWithinCells( );
        mEcologyTimer.Stop( );
        std::cout << "Within grid ecology took: " << mEcologyTimer.GetElapsedTimeSecs( ) << std::endl;

        mDispersalTimer.Start( );

        RunCrossGridCellEcology( mDispersals );
        mDispersalTimer.Stop( );
        std::cout << "Across grid ecology took: " << mDispersalTimer.GetElapsedTimeSecs( ) << std::endl;

        mOutputTimer.Start( );
        Output( timeStep );
        mOutputTimer.Stop( );
        std::cout << "Global Outputs took: " << mOutputTimer.GetElapsedTimeSecs( ) << std::endl;

        // Write the results of dispersal to the console
        std::cout << "Total Cohorts remaining " << mGlobalDiagnosticVariables["NumberOfCohortsInModel"] << std::endl;
    }
}

void Madingley::RunWithinCells( ) {
    // Instantiate a class to hold thread locked global diagnostic variables
    ThreadVariables singleThreadDiagnostics( 0, 0, 0, mNextCohortID );

    mModelGrid.ApplyFunctionToAllCells( [&]( GridCell & gcl ) {

        RunWithinCellStockEcology( gcl );

        RunWithinCellCohortEcology( gcl, singleThreadDiagnostics );

    } );
    // Update the variable tracking cohort unique IDs
    mNextCohortID = singleThreadDiagnostics.mNextCohortID;
    // Take the results from the thread local variables and apply to the global diagnostic variables
    mGlobalDiagnosticVariables["NumberOfCohortsExtinct"] = singleThreadDiagnostics.mExtinctions - singleThreadDiagnostics.mCombinations;
    mGlobalDiagnosticVariables["NumberOfCohortsProduced"] = singleThreadDiagnostics.mProductions;
    mGlobalDiagnosticVariables["NumberOfCohortsInModel"] = mGlobalDiagnosticVariables["NumberOfCohortsInModel"] + singleThreadDiagnostics.mProductions - singleThreadDiagnostics.mExtinctions;
    mGlobalDiagnosticVariables["NumberOfCohortsCombined"] = singleThreadDiagnostics.mCombinations;
}

void Madingley::RunWithinCellsInParallel( ) {
        // Instantiate a class to hold thread locked global diagnostic variables
        ThreadVariables singleThreadDiagnostics( 0, 0, 0, mNextCohortID );

        #pragma omp parallel num_threads(omp_get_num_procs())
        {
            #pragma omp for schedule(dynamic)
            for( unsigned gridCellIndex = 0; gridCellIndex < Parameters::Get( )->GetNumberOfGridCells( ); gridCellIndex++ ) 
            {
                #pragma omp critical
                {
            RunWithinCellStockEcology( gcl );
            RunWithinCellCohortEcology( gcl, singleThreadDiagnostics );
                }//END critical
            }
        }//END PARALLEL REGION
        
        // Update the variable tracking cohort unique IDs
        mNextCohortID = singleThreadDiagnostics.mNextCohortID;
        // Take the results from the thread local variables and apply to the global diagnostic variables
        mGlobalDiagnosticVariables["NumberOfCohortsExtinct"] = singleThreadDiagnostics.mExtinctions - singleThreadDiagnostics.mCombinations;
        mGlobalDiagnosticVariables["NumberOfCohortsProduced"] = singleThreadDiagnostics.mProductions;
        mGlobalDiagnosticVariables["NumberOfCohortsInModel"] = mGlobalDiagnosticVariables["NumberOfCohortsInModel"] + singleThreadDiagnostics.mProductions - singleThreadDiagnostics.mExtinctions;
        mGlobalDiagnosticVariables["NumberOfCohortsCombined"] = singleThreadDiagnostics.mCombinations;   

}

void Madingley::RunWithinCellStockEcology( GridCell& gcl ) {

    // Create a local instance of the stock ecology class
    EcologyStock MadingleyEcologyStock;
    // Get the list of functional group indices for autotroph stocks
    std::vector<int> AutotrophStockFunctionalGroups = mParams.mStockFunctionalGroupDefinitions.GetFunctionalGroupIndex( "Heterotroph/Autotroph", "Autotroph", false );
    // Loop over autotroph functional groups
    for( unsigned FunctionalGroup: AutotrophStockFunctionalGroups ) {
        for( auto& ActingStock: gcl.mStocks[FunctionalGroup] ) {

            // Run stock ecology
            MadingleyEcologyStock.RunWithinCellEcology( gcl, ActingStock, mCurrentTimeStep, mCurrentMonth, mParams );
        }
    }

}

void Madingley::RunWithinCellCohortEcology( GridCell& gcl, ThreadVariables& partial ) {
    // Local instances of classes
    // Initialize ecology for stocks and cohorts - needed fresh every timestep?
    EcologyCohort mEcologyCohort;
    mEcologyCohort.InitialiseEating( gcl, mParams );
    Activity CohortActivity;

    // Loop over randomly ordered gridCellCohorts to implement biological functions
    gcl.ApplyFunctionToAllCohortsWithStaticRandomness( [&]( Cohort* c ) {
        // Perform all biological functions except dispersal (which is cross grid cell)

        if( gcl.mCohorts[c->mFunctionalGroupIndex].size( ) != 0 && c->mCohortAbundance > Parameters::Get( )->GetExtinctionThreshold( ) ) {

            CohortActivity.AssignProportionTimeActive( gcl, c, mCurrentTimeStep, mCurrentMonth, mParams );

            // Run ecology
            mEcologyCohort.RunWithinCellEcology( gcl, c, mCurrentTimeStep, partial, mCurrentMonth, mParams );
            // Update the properties of the acting cohort
            mEcologyCohort.UpdateEcology( gcl, c, mCurrentTimeStep );
            Cohort::ResetMassFluxes( );
            // Check that the mass of individuals in this cohort is still >= 0 after running ecology
            assert( c->mIndividualBodyMass >= 0.0 && "Biomass < 0 for this cohort" );
        }

        // Check that the mass of individuals in this cohort is still >= 0 after running ecology
        if( gcl.mCohorts[c->mFunctionalGroupIndex].size( ) > 0 ) assert( c->mIndividualBodyMass >= 0.0 && "Biomass < 0 for this cohort" );

    }, mCurrentTimeStep );

    for( auto c: Cohort::mNewCohorts ) {
        gcl.InsertCohort( c );
        if( c->mDestinationCell != &gcl ) std::cout << "whut? wrong cell?" << std::endl;
    }
    partial.mProductions += Cohort::mNewCohorts.size( );
    Cohort::mNewCohorts.clear( );

    RunExtinction( gcl, partial );

    // Merge cohorts, if necessary
    if( gcl.GetNumberOfCohorts( ) > Parameters::Get( )->GetMaximumNumberOfCohorts( ) ) {
        partial.mCombinations += mCohortMerger.MergeToReachThresholdFast( gcl, mParams );

        //Run extinction a second time to remove those cohorts that have been set to zero abundance when merging
        RunExtinction( gcl, partial );
    }
}

void Madingley::RunExtinction( GridCell& gcl, ThreadVariables& partial ) {

    // Loop over cohorts and remove any whose abundance is below the extinction threshold
    std::vector<Cohort*> CohortsToRemove;
    gcl.ApplyFunctionToAllCohorts( [&]( Cohort* c ) {
        if( c->mCohortAbundance - Parameters::Get( )->GetExtinctionThreshold( ) <= 0 || c->mIndividualBodyMass <= 0 ) {
            CohortsToRemove.push_back( c );
            partial.mExtinctions += 1;
        }
    } );

    // Code to add the biomass to the biomass pool and dispose of the cohort
    for( auto c: CohortsToRemove ) {

        // Add biomass of the extinct cohort to the organic matter pool
        double deadMatter = ( c->mIndividualBodyMass + c->mIndividualReproductivePotentialMass ) * c->mCohortAbundance;
        if( deadMatter < 0 ) std::cout << "Dead " << deadMatter << std::endl;
        Environment::Get( "Organic Pool", c->GetCurrentCell( ) ) += deadMatter;
        assert( Environment::Get( "Organic Pool", c->GetCurrentCell( ) ) >= 0 && "Organic pool < 0" );

        // Remove the extinct cohort from the list of cohorts
        gcl.RemoveCohort( c );
    }
    for( auto c: CohortsToRemove ) {delete(c);}
}

void Madingley::RunCrossGridCellEcology( unsigned& dispersals ) {
    // Loop through each grid cell, and run dispersal for each.
    // In the original model a new dispersal object is made every timestep - this resets the random number generators
    mDispersalSet->ResetRandoms( );
    mModelGrid.ApplyFunctionToAllCells( [&]( GridCell & c ) {
        mDispersalSet->RunCrossGridCellEcologicalProcess( c, mModelGrid, mParams, mCurrentMonth );
    } );

    // Apply the changes from dispersal
    mDispersalSet->UpdateCrossGridCellEcology( dispersals );
}

void Madingley::SetUpGlobalDiagnosticsList( ) {
    // Add global diagnostic variables
    mGlobalDiagnosticVariables["NumberOfCohortsExtinct"] = 0.0;
    mGlobalDiagnosticVariables["NumberOfCohortsProduced"] = 0.0;
    mGlobalDiagnosticVariables["NumberOfCohortsCombined"] = 0.0;
    mGlobalDiagnosticVariables["NumberOfCohortsInModel"] = 0.0;
    mGlobalDiagnosticVariables["NumberOfStocksInModel"] = 0.0;
}

void Madingley::Output( unsigned step ) {
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

                gridCell.ApplyFunctionToAllCohorts( [&]( Cohort* c ) {
                    totalCohorts += 1;
                    totalCohortAbundance += c->mCohortAbundance;

                            double cohortBiomass = ( c->mIndividualBodyMass + c->mIndividualReproductivePotentialMass ) * c->mCohortAbundance / 1000.;
                            cohortBiomassThisCell += cohortBiomass;
                            totalCohortBiomass += cohortBiomass;
                            cohortAbundanceThisCell += c->mCohortAbundance;

                    if( mCohortNutritionSource[ c->mFunctionalGroupIndex ] == "herbivore" ) {
                        herbivoreBiomassThisCell += c->mIndividualBodyMass;
                                herbivoreAbundanceThisCell += c->mCohortAbundance;
                    } else if( mCohortNutritionSource[ c->mFunctionalGroupIndex ] == "omnivore" ) {
                        omnivoreBiomassThisCell += c->mIndividualBodyMass;
                                omnivoreAbundanceThisCell += c->mCohortAbundance;
                    } else if( mCohortNutritionSource[ c->mFunctionalGroupIndex ] == "carnivore" ) {
                        carnivoreBiomassThisCell += c->mIndividualBodyMass;
                                carnivoreAbundanceThisCell += c->mCohortAbundance;
                    }

                    if( mCohortThermoregulation[ c->mFunctionalGroupIndex ] == "endotherm" ) {
                        ectothermBiomassThisCell += c->mIndividualBodyMass;
                                ectothermAbundanceThisCell += c->mCohortAbundance;
                    } else if( mCohortThermoregulation[ c->mFunctionalGroupIndex ] == "ectotherm" ) {
                        endothermBiomassThisCell += c->mIndividualBodyMass;
                                endothermAbundanceThisCell += c->mCohortAbundance;
                    }

                    if( mCohortReproductiveStrategy[ c->mFunctionalGroupIndex ] == "iteroparity" ) {
                        iteroparousBiomassThisCell += c->mIndividualBodyMass;
                                iteroparousAbundanceThisCell += c->mCohortAbundance;
                    } else if( mCohortReproductiveStrategy[ c->mFunctionalGroupIndex ] == "semelparity" ) {
                        semelparousBiomassThisCell += c->mIndividualBodyMass;
                                semelparousAbundanceThisCell += c->mCohortAbundance;
                    }
                } );
        gridCell.ApplyFunctionToAllStocks( [&]( Stock & s ) {
            double thisStockBiomass = s.mTotalBiomass / 1000.;
            stockBiomassThisCell += thisStockBiomass; //convert from g to kg
                    totalStockBiomass += thisStockBiomass;

            if( mStockLeafStrategy[ s.mFunctionalGroupIndex ] == "na" ) phytoplanktonBiomassThisCell += thisStockBiomass;
            else if( mStockLeafStrategy[ s.mFunctionalGroupIndex ] == "deciduous" ) deciduousBiomassThisCell += thisStockBiomass;
            else if( mStockLeafStrategy[ s.mFunctionalGroupIndex ] == "evergreen" ) evergreenBiomassThisCell += thisStockBiomass;
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
