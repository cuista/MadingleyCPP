#ifndef TREPRODUCTIONBASIC_H
#define TREPRODUCTIONBASIC_H
#include <limits>
#include <assert.h>
/** \file TReproductionBasic.h
 * \brief the TReproductionBasic header file
 */

/** \brief A formulation of the process of reproduction */
class ReproductionBasic : public IReproductionImplementation {
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------  

    /** \brief The time units associated with this implementation of dispersal */
    const string TimeUnitImplementation = "month";
    /** \brief The per individual ratio of (adult body mass + reproductive potential mass) to adult body mass above which reproduction is possible*/
    const double MassRatioThreshold = 1.5;
    /** \brief The probability that random draws above which result in offspring cohorts with 
    evolved juvenile and adult masses*/
    const double MassEvolutionProbabilityThreshold = 0.95;
    /** \brief The standard deviation around the parent cohort's adult and juvenile masses to apply when drawing offspring
    adult and juvenile masses (when mass evolution occurs)*/
    const double MassEvolutionStandardDeviation = 0.05;
    /** \brief The proportion of adult (non-reproductive) biomass allocated to offspring during a reproductive event by semelparous organisms*/
    const double SemelparityAdultMassAllocation = 0.5;
    /** \brief Scalar to convert from the time step units used by this formulation of reproduction to global model time step units */
    double DeltaT;
    /** \brief Include Utility class */
    UtilityFunctions Utilities;
    /** \brief An instance of the simple random number generator class */
    std::default_random_engine RandomNumberGenerator;
public:
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------  
       
    //----------------------------------------------------------------------------------------------       
    /** \brief
    Constructor for reproduction: assigns all parameter values
    @param globalModelTimeStepUnit The time step of the global model 
    @param drawRandomly Indicates whether to draw values randomly 
     */
    ReproductionBasic(string globalModelTimeStepUnit, bool DrawRandomly) {
        // Calculate the scalar to convert from the time step units used by this implementation of dispersal to the global model time step units
        DeltaT = Utilities.ConvertTimeUnits(globalModelTimeStepUnit, TimeUnitImplementation);

        // Set the seed for the random number generator
        if (DrawRandomly) {
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            RandomNumberGenerator.seed(seed);
        } else {
            RandomNumberGenerator.seed(4000);
        }
    }
    //----------------------------------------------------------------------------------------------       
    /** \brief Generate new cohorts from reproductive potential mass
    @param gridCell The current grid cell 
    @param currentTimestep The current model time step 
    @param partial Thread-locked variables 
    @param iteroparous Whether the acting cohort is iteroparous, as opposed to semelparous 
    @param currentMonth The current model month */
    void RunReproductionEvents(GridCell& gcl, Cohort& actingCohort,
            unsigned currentTimestep,ThreadLockedParallelVariables& partial, 
            bool iteroparous, unsigned currentMonth, MadingleyModelInitialisation& params) {
        // Adult non-reproductive biomass lost by semelparous organisms
        double AdultMassLost;

        // Offspring cohort abundance
        double OffspringCohortAbundance;

        // Mass ratio of body mass + reproductive mass to adult body mass
        double CurrentMassRatio;

        // Individual body mass including change this time step as a result of other ecological processes
        double BodyMassIncludingChangeThisTimeStep;

        // Offspring juvenile and adult body masses
        vector<double> OffspringJuvenileAndAdultBodyMasses(2);

        // Offspring cohort
        //Cohort OffspringCohort;

        // Individual reproductive mass including change this time step as a result of other ecological processes
        double ReproductiveMassIncludingChangeThisTimeStep;


        // Calculate the biomass of an individual in this cohort including changes this time step from other ecological processes  
        BodyMassIncludingChangeThisTimeStep = 0.0;

        for (auto& Biomass : Cohort::Deltas["biomass"]) {
            // Add the delta biomass to net biomass
            BodyMassIncludingChangeThisTimeStep += Biomass.second;

        }

        BodyMassIncludingChangeThisTimeStep += actingCohort.IndividualBodyMass;

        // Calculate the reproductive biomass of an individual in this cohort including changes this time step from other ecological processes  
        ReproductiveMassIncludingChangeThisTimeStep = 0.0;

        for (auto& ReproBiomass : Cohort::Deltas["reproductivebiomass"]) {
            // Add the delta reproductive biomass to net biomass
            ReproductiveMassIncludingChangeThisTimeStep += ReproBiomass.second;
        }

        ReproductiveMassIncludingChangeThisTimeStep += actingCohort.IndividualReproductivePotentialMass;
        if (actingCohort.IndividualBodyMass> 1.e-200){
        // Get the current ratio of total individual mass (including reproductive potential) to adult body mass
        CurrentMassRatio = (BodyMassIncludingChangeThisTimeStep + ReproductiveMassIncludingChangeThisTimeStep) / actingCohort.AdultMass;

        // Must have enough mass to hit reproduction threshold criterion, and either (1) be in breeding season, or (2) be a marine cell (no breeding season in marine cells)
        if ((CurrentMassRatio > MassRatioThreshold) && ((Environment::Get("Breeding Season",actingCohort.Here()) == 1.0) || (gcl.isMarine()))) {
            // Iteroparous and semelparous organisms have different strategies
            if (iteroparous) {
                // Iteroparous organisms do not allocate any of their current non-reproductive biomass to reproduction
                AdultMassLost = 0.0;

                // Calculate the number of offspring that could be produced given the reproductive potential mass of individuals
                OffspringCohortAbundance = actingCohort.CohortAbundance * ReproductiveMassIncludingChangeThisTimeStep /
                        actingCohort.JuvenileMass;
            } else {
                // Semelparous organisms allocate a proportion of their current non-reproductive biomass (including the effects of other ecological processes) to reproduction
                AdultMassLost = SemelparityAdultMassAllocation * BodyMassIncludingChangeThisTimeStep;

                // Calculate the number of offspring that could be produced given the reproductive potential mass of individuals
                OffspringCohortAbundance = actingCohort.CohortAbundance * (AdultMassLost + ReproductiveMassIncludingChangeThisTimeStep) /
                        actingCohort.JuvenileMass;
            }

            // Check that the abundance in the cohort to produce is greater than or equal to zero
            assert(OffspringCohortAbundance >= 0.0 && "Offspring abundance < 0");

            // Get the adult and juvenile masses of the offspring cohort
            OffspringJuvenileAndAdultBodyMasses = GetOffspringCohortProperties( actingCohort, params.CohortFunctionalGroupDefinitions);

            // Update cohort abundance in case juvenile mass has been altered through 'evolution'
            OffspringCohortAbundance = (OffspringCohortAbundance * actingCohort.JuvenileMass) / OffspringJuvenileAndAdultBodyMasses[0];

            // Create the offspring cohort
             
            Cohort OffspringCohort(actingCohort,OffspringJuvenileAndAdultBodyMasses[0], OffspringJuvenileAndAdultBodyMasses[1], OffspringJuvenileAndAdultBodyMasses[0], OffspringCohortAbundance, currentTimestep, partial.NextCohortIDThreadLocked);

            // Add the offspring cohort to the grid cell cohorts array
            Cohort::newCohorts.push_back(OffspringCohort);

            // Subtract all of the reproductive potential mass of the parent cohort, which has been used to generate the new
            // cohort, from the delta reproductive potential mass and delta adult body mass
            Cohort::Deltas["reproductivebiomass"]["reproduction"] -= ReproductiveMassIncludingChangeThisTimeStep;
            Cohort::Deltas["biomass"]["reproduction"] -= AdultMassLost;
        } else {
            // Organism is not large enough, or it is not the breeding season, so take no action
        }
        }

    }
    //----------------------------------------------------------------------------------------------       
    /** \brief assigns ingested biomass from other ecological processes to reproductive potential mass
    @param gridCell The current grid cell 
    @param actingCohort The position of the acting cohort in the jagged array of grid cell cohorts 
    @param currentTimestep The current model time step 
    @param params The model parameters */
    void RunReproductiveMassAssignment(GridCell& gcl, Cohort& actingCohort, unsigned currentTimestep, MadingleyModelInitialisation& params) {
        // Biomass per individual in each cohort to be assigned to reproductive potential
        double BiomassToAssignToReproductivePotential;

        // Net biomass change from other ecological functions this time step
        double NetBiomassFromOtherEcologicalFunctionsThisTimeStep;

        // Reset variable holding net biomass change of individuals in this cohort as a result of other ecological processes
        NetBiomassFromOtherEcologicalFunctionsThisTimeStep = 0.0;

        // Loop over all items in the biomass deltas
        for (auto Biomass : Cohort::Deltas["biomass"]) {
            // Add the delta biomass to net biomass
            NetBiomassFromOtherEcologicalFunctionsThisTimeStep += Biomass.second;
        }

        // If individual body mass after the addition of the net biomass from processes this time step will yield a body mass 
        // greater than the adult body mass for this cohort, then assign the surplus to reproductive potential
        if ((actingCohort.IndividualBodyMass + NetBiomassFromOtherEcologicalFunctionsThisTimeStep) > actingCohort.AdultMass) {
            // Calculate the biomass for each individual in this cohort to be assigned to reproductive potential
            BiomassToAssignToReproductivePotential = actingCohort.IndividualBodyMass + NetBiomassFromOtherEcologicalFunctionsThisTimeStep -
                    actingCohort.AdultMass;

            // Check that a positive biomass is to be assigned to reproductive potential
            assert(BiomassToAssignToReproductivePotential >= 0.0 && "Assignment of negative reproductive potential mass");

            // If this is the first time reproductive potential mass has been assigned for this cohort, 
            // then set the maturity time step for this cohort as the current model time step
            if (actingCohort.MaturityTimeStep == std::numeric_limits<unsigned>::max()) {
                actingCohort.MaturityTimeStep = currentTimestep;
            }

            // Assign the specified mass to reproductive potential mass and remove it from individual biomass
            Cohort::Deltas["reproductivebiomass"]["reproduction"] += BiomassToAssignToReproductivePotential;
            Cohort::Deltas["biomass"]["reproduction"] -= BiomassToAssignToReproductivePotential;

        } else {
            // Cohort has not gained sufficient biomass to assign any to reproductive potential, so take no action
        }
    }
    //----------------------------------------------------------------------------------------------       
    /** \brief Assign the juvenile and adult masses of the new cohort to produce
    @param actingCohort The position of the acting cohort in the jagged array of grid cell cohorts 
    @param madingleyCohortDefinitions The definitions of cohort functional groups in the model 
    @return A vector containing the juvenile and adult masses of the cohort to be produced*/
    vector<double> GetOffspringCohortProperties( Cohort& actingCohort, FunctionalGroupDefinitions& madingleyCohortDefinitions) {
        // A two-element vector holding adult and juvenile body masses in elements zero and one respectively
        vector<double> CohortJuvenileAdultMasses(2);

        // Determine whether offspring cohort 'evolves' in terms of adult and juvenile body masses
        std::uniform_real_distribution<double> randomNumber(0.0, 1.0);
        double RandomValue = randomNumber(RandomNumberGenerator);
        if (RandomValue > MassEvolutionProbabilityThreshold) {
            // Determine the new juvenile body mass //MB correctly formulated?
            std::normal_distribution<double> randomNumberJ(actingCohort.JuvenileMass, MassEvolutionStandardDeviation * actingCohort.JuvenileMass);
            double RandomValueJ = randomNumberJ(RandomNumberGenerator);
            CohortJuvenileAdultMasses[0] = max(RandomValueJ,
                    madingleyCohortDefinitions.GetBiologicalPropertyOneFunctionalGroup("Minimum mass", actingCohort.FunctionalGroupIndex));

            // Determine the new adult body mass
            std::normal_distribution<double> randomNumberA(actingCohort.AdultMass, MassEvolutionStandardDeviation * actingCohort.AdultMass);
            double RandomValueA = randomNumberA(RandomNumberGenerator);
            CohortJuvenileAdultMasses[1] = min(RandomValueA,
                    madingleyCohortDefinitions.GetBiologicalPropertyOneFunctionalGroup("Maximum mass", actingCohort.FunctionalGroupIndex));
        }            // If not, it just gets the same values as the parent cohort
        else {
            // Assign masses to the offspring cohort that are equal to those of the parent cohort
            CohortJuvenileAdultMasses[0] = actingCohort.JuvenileMass;
            CohortJuvenileAdultMasses[1] = actingCohort.AdultMass;
        }

        // Return the vector of adult and juvenile masses
        return CohortJuvenileAdultMasses;
    }
    //----------------------------------------------------------------------------------------------       
};
#endif
