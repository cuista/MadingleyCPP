#include "ReproductionBasic.h"

ReproductionBasic::ReproductionBasic( std::string globalModelTimeStepUnit, bool drawRandomly ) {
    // Calculate the scalar to convert from the time step units used by this implementation of dispersal to the global model time step units
    mDeltaT = mUtilities.ConvertTimeUnits( globalModelTimeStepUnit, mTimeUnitImplementation );

    // Set the seed for the random number generator
    if( drawRandomly ) {
        unsigned seed = std::chrono::system_clock::now( ).time_since_epoch( ).count( );
        mRandomNumber.seed( seed );
    } else {
        mRandomNumber.seed( 4000 );
    }
}

void ReproductionBasic::Run( GridCell& gcl, Cohort& actingCohort, unsigned currentTimestep, ThreadVariables& partial, bool iteroparous, unsigned currentMonth, MadingleyInitialisation& params ) {
    // Adult non-reproductive biomass lost by semelparous organisms
    double adultMassLost;

    // Offspring cohort abundance
    double offspringCohortAbundance;

    // Mass ratio of body mass + reproductive mass to adult body mass
    double currentMassRatio;

    // Individual body mass including change this time step as a result of other ecological processes
    double bodyMassIncludingChangeThisTimeStep;

    // Offspring juvenile and adult body masses
    std::vector< double > offspringJuvenileAndAdultBodyMasses( 2 );

    // Individual reproductive mass including change this time step as a result of other ecological processes
    double reproductiveMassIncludingChangeThisTimeStep;

    // Calculate the biomass of an individual in this cohort including changes this time step from other ecological processes  
    bodyMassIncludingChangeThisTimeStep = 0.0;

    for( auto& Biomass: Cohort::mMassFluxes[ "biomass" ] ) {
        // Add the delta biomass to net biomass
        bodyMassIncludingChangeThisTimeStep += Biomass.second;
    }
    bodyMassIncludingChangeThisTimeStep += actingCohort.mIndividualBodyMass;

    // Calculate the reproductive biomass of an individual in this cohort including changes this time step from other ecological processes  
    reproductiveMassIncludingChangeThisTimeStep = 0.0;

    for( auto& ReproBiomass: Cohort::mMassFluxes[ "reproductivebiomass" ] ) {
        // Add the delta reproductive biomass to net biomass
        reproductiveMassIncludingChangeThisTimeStep += ReproBiomass.second;
    }
    reproductiveMassIncludingChangeThisTimeStep += actingCohort.mIndividualReproductivePotentialMass;
    if( actingCohort.mIndividualBodyMass > 1.e-200 ) {
        // Get the current ratio of total individual mass (including reproductive potential) to adult body mass
        currentMassRatio = ( bodyMassIncludingChangeThisTimeStep + reproductiveMassIncludingChangeThisTimeStep ) / actingCohort.mAdultMass;

        // Must have enough mass to hit reproduction threshold criterion, and either (1) be in breeding season, or (2) be a marine cell (no breeding season in marine cells)
        if( ( currentMassRatio > mMassRatioThreshold ) && ( ( Environment::Get( "Breeding Season", actingCohort.GetCurrentCell( ) ) == 1.0 ) || ( gcl.IsMarine( ) ) ) ) {
            // Iteroparous and semelparous organisms have different strategies
            if( iteroparous ) {
                // Iteroparous organisms do not allocate any of their current non-reproductive biomass to reproduction
                adultMassLost = 0.0;

                // Calculate the number of offspring that could be produced given the reproductive potential mass of individuals
                offspringCohortAbundance = actingCohort.mCohortAbundance * reproductiveMassIncludingChangeThisTimeStep / actingCohort.mJuvenileMass;
            } else {
                // Semelparous organisms allocate a proportion of their current non-reproductive biomass (including the effects of other ecological processes) to reproduction
                adultMassLost = mSemelparityAdultMassAllocation * bodyMassIncludingChangeThisTimeStep;

                // Calculate the number of offspring that could be produced given the reproductive potential mass of individuals
                offspringCohortAbundance = ( ( actingCohort.mCohortAbundance ) * ( adultMassLost + reproductiveMassIncludingChangeThisTimeStep ) ) / actingCohort.mJuvenileMass;
            }
            // Check that the abundance in the cohort to produce is greater than or equal to zero
            assert( offspringCohortAbundance >= 0.0 && "Offspring abundance < 0" );

            // Get the adult and juvenile masses of the offspring cohort
            offspringJuvenileAndAdultBodyMasses = GetOffspringCohortProperties( actingCohort, params.mCohortFunctionalGroupDefinitions );

            // Update cohort abundance in case juvenile mass has been altered through 'evolution'
            offspringCohortAbundance = offspringCohortAbundance * ( actingCohort.mJuvenileMass / offspringJuvenileAndAdultBodyMasses[ 0 ] );

            // Create the offspring cohort

            Cohort OffspringCohort( actingCohort, offspringJuvenileAndAdultBodyMasses[ 0 ], offspringJuvenileAndAdultBodyMasses[ 1 ], offspringJuvenileAndAdultBodyMasses[ 0 ], offspringCohortAbundance, currentTimestep, partial.mNextCohortID );
            // Add the offspring cohort to the grid cell cohorts array
            Cohort::mNewCohorts.push_back( OffspringCohort );

            // Subtract all of the reproductive potential mass of the parent cohort, which has been used to generate the new
            // cohort, from the delta reproductive potential mass and delta adult body mass
            Cohort::mMassFluxes[ "reproductivebiomass" ][ "reproduction" ] -= reproductiveMassIncludingChangeThisTimeStep;
            Cohort::mMassFluxes[ "biomass" ][ "reproduction" ] -= adultMassLost;
        } else {
            // Organism is not large enough, or it is not the breeding season, so take no action
        }
    }
}

void ReproductionBasic::MassAssignment( GridCell& gcl, Cohort& actingCohort, unsigned currentTimestep, MadingleyInitialisation& params ) {
    // Biomass per individual in each cohort to be assigned to reproductive potential
    double BiomassToAssignToReproductivePotential;

    // Net biomass change from other ecological functions this time step
    double NetBiomassFromOtherEcologicalFunctionsThisTimeStep;

    // Reset variable holding net biomass change of individuals in this cohort as a result of other ecological processes
    NetBiomassFromOtherEcologicalFunctionsThisTimeStep = 0.0;

    // Loop over all items in the biomass deltas
    for( auto Biomass: Cohort::mMassFluxes[ "biomass" ] ) {
        // Add the delta biomass to net biomass
        NetBiomassFromOtherEcologicalFunctionsThisTimeStep += Biomass.second;
    }

    // If individual body mass after the addition of the net biomass from processes this time step will yield a body mass 
    // greater than the adult body mass for this cohort, then assign the surplus to reproductive potential
    if( ( actingCohort.mIndividualBodyMass + NetBiomassFromOtherEcologicalFunctionsThisTimeStep ) > actingCohort.mAdultMass ) {
        // Calculate the biomass for each individual in this cohort to be assigned to reproductive potential
        BiomassToAssignToReproductivePotential = actingCohort.mIndividualBodyMass + NetBiomassFromOtherEcologicalFunctionsThisTimeStep - actingCohort.mAdultMass;
        // Check that a positive biomass is to be assigned to reproductive potential
        assert( BiomassToAssignToReproductivePotential >= 0.0 && "Assignment of negative reproductive potential mass" );

        // If this is the first time reproductive potential mass has been assigned for this cohort, 
        // then set the maturity time step for this cohort as the current model time step
        if( actingCohort.mMaturityTimeStep == std::numeric_limits< unsigned >::max( ) ) {
            actingCohort.mMaturityTimeStep = currentTimestep;
        }

        // Assign the specified mass to reproductive potential mass and remove it from individual biomass
        Cohort::mMassFluxes[ "reproductivebiomass" ][ "reproduction" ] += BiomassToAssignToReproductivePotential;
        Cohort::mMassFluxes[ "biomass" ][ "reproduction" ] -= BiomassToAssignToReproductivePotential;

    } else {
        // Cohort has not gained sufficient biomass to assign any to reproductive potential, so take no action
    }
}

std::vector< double > ReproductionBasic::GetOffspringCohortProperties( Cohort& actingCohort, FunctionalGroupDefinitions& cohortDefinitions ) {
    // A two-element vector holding adult and juvenile body masses in elements zero and one respectively
    std::vector< double > cohortJuvenileAdultMasses( 2 );

    // Determine whether offspring cohort 'evolves' in terms of adult and juvenile body masses
    std::uniform_real_distribution< double > randomNumber( 0.0, 1.0 );
    double RandomValue = randomNumber( mRandomNumber );

    if( RandomValue > mMassEvolutionProbabilityThreshold ) {
        // Determine the new juvenile body mass // MB correctly formulated?
        std::normal_distribution< double > randomNumberJ( actingCohort.mJuvenileMass, mMassEvolutionStandardDeviation * actingCohort.mJuvenileMass );
        double RandomValueJ = randomNumberJ( mRandomNumber );
        cohortJuvenileAdultMasses[ 0 ] = max( RandomValueJ, cohortDefinitions.GetBiologicalPropertyOneFunctionalGroup( "Minimum mass", actingCohort.mFunctionalGroupIndex ) );

        // Determine the new adult body mass
        std::normal_distribution< double > randomNumberA( actingCohort.mAdultMass, mMassEvolutionStandardDeviation * actingCohort.mAdultMass );
        double RandomValueA = randomNumberA( mRandomNumber );
        cohortJuvenileAdultMasses[ 1 ] = min( RandomValueA, cohortDefinitions.GetBiologicalPropertyOneFunctionalGroup( "Maximum mass", actingCohort.mFunctionalGroupIndex ) );
    
    } else { // If not, it just gets the same values as the parent cohort
        // Assign masses to the offspring cohort that are equal to those of the parent cohort
        cohortJuvenileAdultMasses[ 0 ] = actingCohort.mJuvenileMass;
        cohortJuvenileAdultMasses[ 1 ] = actingCohort.mAdultMass;
    }

    // Return the vector of adult and juvenile masses
    return cohortJuvenileAdultMasses;
}
