#ifndef REVISEDTERRESTRIALPLANTMODEL_H
#define REVISEDTERRESTRIALPLANTMODEL_H
#include <math.h>
#include <Stock.h>
#include <GridCell.h>
#include <UtilityFunctions.h>

using namespace std;
/** \file RevisedTerrestrialPlantModel.h
 * \brief the RevisedTerrestrialPlantModel header file
 */

/** \brief Revised version of Matt Smith's terrestrial carbon model */
class RevisedTerrestrialPlantModel {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------

    /** \brief The maximum possible NPP (kg C per m2 per year) */
    double max_NPP;
    /** \brief First constant in the logistic function relating NPP to temperature in the Miami NPP model */
    double t1_NPP;
    /** \brief Second constant in the logistic function relating NPP to temperature in the Miami NPP model  */
    double t2_NPP;
    /** \brief Constant in the saturating function relating NPP to precipitation in the Miami NPP model */
    double p_NPP;
    /** \brief Scalar relating the fraction of NPP devoted to structural tissue to the total amount of NPP */
    double FracStructScalar;
    /** \brief Coefficient for the quadratic term in the function relating fractional allocation in evergreen leaf matter to fraction of the year experiencing frost */
    double a_FracEvergreen;
    /** \brief Coefficient for the linear term in the function relating fractional allocation in evergreen leaf matter to fraction of the year experiencing frost */
    double b_FracEvergreen;
    /** \brief Intercept in the function relating fractional allocation in evergreen leaf matter to fraction of the year experiencing frost */
    double c_FracEvergreen;
    /** \brief The slope of the relationship between temperature and evergreen leaf mortality rate */
    double m_EGLeafMortality;
    /** \brief The intercept of the relationship between temperature and evergreen leaf mortality rate */
    double c_EGLeafMortality;
    /** \brief The minimum rate of evergreen leaf mortality */
    double er_min;
    /** \brief The maximum rate of evergreen leaf mortality */
    double er_max;
    /** \brief The slope of the relationship between temperature and deciduous leaf mortality rate */
    double m_DLeafMortality;
    /** \brief The intercept of the relationship between temperature and deciduous leaf mortality rate */
    double c_DLeafMortality;
    /** \brief The minimum rate of deciduous leaf mortality */
    double dr_min;
    /** \brief The maximum rate of deciduous leaf mortality */
    double dr_max;
    /** \brief The slope of the relationship between fine root mortality rate and temperature */
    double m_FRootMort;
    /** \brief The intercept of the relationship between fine root mortality rate and temperature */
    double c_FRootMort;
    /** \brief The minimum rate of fine root mortality */
    double frm_min;
    /** \brief The maximum rate of fine root mortality */
    double frm_max;
    /** \brief Scalar relating fire mortality rate to NPP */
    double NPPScalar_Fire;
    /** \brief NPP at which fire mortality reaches half its maximum rate */
    double NPPHalfSaturation_Fire;
    /** \brief Scalar relating fire mortality rate to the fractional fire season length */
    double LFSScalar_Fire;
    /** \brief The fractional fire season length at which fire mortality reaches half its maximum rate */
    double LFSHalfSaturation_Fire;
    /** \brief Base scalar for the fire mortality function */
    double BaseScalar_Fire;
    /** \brief Minimum fire return interval */
    double MinReturnInterval;
    /** \brief Second parameter in the structural mortality function */
    double p2_StMort;
    /** \brief First parameter in the structural mortality function */
    double p1_StMort;
    /** \brief Maximum rate of structural mortality */
    double stm_max;
    /** \brief Minimum rate of structural mortality */
    double stm_min;
    /** \brief The maximum fraction of productivity that can be allocated to structural tissue */
    double MaxFracStruct;
    /** \brief Scalar to convert between mass of carbon and mass of leaf dry matter */
    double MassCarbonPerMassLeafDryMatter;
    /** \brief Scalar to convert between mass of lead dry and mass of leaf wet matter */
    double MassLeafDryMatterPerMassLeafWetMatter;
    /** \brief Constant to convert from m2 to km2 */
    double m2Tokm2Conversion;
    //
    /** \brief Instance of the class to perform general functions */
    UtilityFunctions Utilities;

    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief    Constructor for the plant model     */
    RevisedTerrestrialPlantModel( ) {
        // Initialise parameters
        InitialisePlantModelParameters( );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Initialise parameters for the plant model */
    void InitialisePlantModelParameters( ) {
        // Assign the parameters for the plant model
        max_NPP = 0.961644704;
        t1_NPP = 0.237468183;
        t2_NPP = 0.100597089;
        p_NPP = 0.001184101;
        FracStructScalar = 7.154615419;
        a_FracEvergreen = 1.270782192;
        b_FracEvergreen = -1.828591558;
        c_FracEvergreen = 0.844864063;
        m_EGLeafMortality = 0.040273936;
        c_EGLeafMortality = 1.013070062;
        m_DLeafMortality = 0.020575964;
        c_DLeafMortality = -1.195235464;
        m_FRootMort = 0.04309283;
        c_FRootMort = -1.478393163;
        p2_StMort = 0.139462774;
        p1_StMort = -4.395910091;
        MaxFracStruct = 0.362742634;
        LFSHalfSaturation_Fire = 0.388125108;
        LFSScalar_Fire = 19.98393943;
        NPPHalfSaturation_Fire = 1.148698636;
        NPPScalar_Fire = 8.419032427;
        er_min = 0.01;
        er_max = 24.0;
        dr_min = 0.01;
        dr_max = 24.0;
        frm_min = 0.01;
        frm_max = 12.0;
        stm_max = 1;
        stm_min = 0.001;
        BaseScalar_Fire = 2.0;
        MinReturnInterval = 0.00000226032940698105;// changed in most recent version was exp( -13.0 );

        // mass of Leaf C per g leaf dry matter = 0.4761 g g-1 (from Kattge et al. (2011), TRY- A global database of plant traits, Global Change Biology)
        MassCarbonPerMassLeafDryMatter = 0.476;
        // mass of leaf dry matter per g leaf wet matter = 0.213 g g-1 (from Kattge et al. (2011), TRY- A global database of plant traits, Global Change Biology)
        MassLeafDryMatterPerMassLeafWetMatter = 0.213;

        m2Tokm2Conversion = 1000000.0;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Estimate the mass of leaves in a specified stock in the specified grid cell at equilibrium, given current environmental conditions
    @param gcl The current grid cell 
    @param deciduous Whether the leaves in the specified stock are deciduous 
    @return The equilibrium mass of leaves in the specified stock*/
    double CalculateEquilibriumLeafMass( GridCell& gcl, bool deciduous ) {
        // Calculate annual average temperature
        double MeanTemp = Environment::Get( "AnnualTemperature", gcl );
        //Calculate total annual precipitation
        double TotalPrecip = Environment::Get( "TotalPrecip", gcl );
        // Calculate total annual AET
        double TotalAET = Environment::Get( "TotalAET", gcl );
        // Calculate NPP using the Miami model
        double NPP = CalculateMiamiNPP( MeanTemp, TotalPrecip );

        // Calculate fractional allocation to structural tissue
        double FracStruct = CalculateFracStruct( NPP );

        // Calculate the fractional allocation of NPP to evergreen plant matter
        double FracEvergreen = CalculateFracEvergreen( Environment::Get( "Fraction Year Frost", gcl ) );

        // Calculate the fire mortality rate
        double FireMortRate = CalculateFireMortalityRate( NPP, Environment::Get( "Fraction Year Fire", gcl ) );

        // Update NPP depending on whether the acting stock is deciduous or evergreen
        if( deciduous ) {
            NPP *= ( 1 - FracEvergreen );
        } else {
            NPP *= FracEvergreen;
        }

        // Calculate fine root mortality rate
        double FRootMort = CalculateFineRootMortalityRate( MeanTemp );

        // Calculate the structural mortality rate
        double StMort = CalculateStructuralMortality( TotalAET );

        double LeafMortRate;

        if( deciduous ) {
            // Calculate deciduous leaf mortality
            LeafMortRate = CalculateDeciduousAnnualLeafMortality( MeanTemp );

        } else {
            // Calculate evergreen leaf mortality
            LeafMortRate = CalculateEvergreenAnnualLeafMortality( MeanTemp );
        }

        // Calculate the fractional mortality of leaves
        double LeafMortFrac = CalculateLeafFracAllocation( LeafMortRate,
                CalculateDeciduousAnnualLeafMortality( MeanTemp ),
                CalculateEvergreenAnnualLeafMortality( MeanTemp ), FracEvergreen, FRootMort );

        // Calculate leaf C fixation
        double LeafCFixation = NPP * ( 1 - FracStruct ) * LeafMortFrac;

        // Calculate leaf carbon mortality
        double LeafCMortality = LeafMortRate + FireMortRate + StMort;

        // Calculate equilibrium leaf carbon in kg C per m2
        double EquilibriumLeafCarbon = LeafCFixation / LeafCMortality;

        // Convert to equilibrium leaf wet matter content
        double LeafWetMatter = ConvertToLeafWetMass( EquilibriumLeafCarbon, gcl.GetCellArea( ) );
        return LeafWetMatter;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Update the leaf stock during a time step given the environmental conditions in the grid cell
    @param gcl The current grid cell 
    @param actingStock The acting stock  
    @param currentTimeStep The current model time step 
    @param deciduous Whether the acting stock consists of deciduous leaves 
    @param GlobalModelTimeStepUnit The time step unit used in the model 
    @param currentMonth The current model month */
    double UpdateLeafStock( GridCell& gcl, Stock& actingStock, unsigned currentTimeStep, bool deciduous, string GlobalModelTimeStepUnit, unsigned currentMonth ) {


        //ESTIMATE ANNUAL LEAF CARBON FIXATION ASSUMING ENVIRONMENT THROUGHOUT THE YEAR IS THE SAME AS IN THIS MONTH
        //Get annual average temperature

        double MeanTemp = Environment::Get( "AnnualTemperature", gcl );
        //Calculate total annual precipitation
        double TotalPrecip = Environment::Get( "TotalPrecip", gcl );
        // Calculate annual NPP
        double NPP = CalculateMiamiNPP( MeanTemp, TotalPrecip );

        // Calculate fractional allocation to structural tissue
        double FracStruct = CalculateFracStruct( NPP );

        // Estimate monthly NPP based on seasonality layer
        NPP *= Environment::Get( "Seasonality", gcl );


        // Calculate leaf mortality rates
        double AnnualLeafMortRate;
        double MonthlyLeafMortRate;
        double TimeStepLeafMortRate;

        if( deciduous ) {
            // Calculate annual deciduous leaf mortality
            AnnualLeafMortRate = CalculateDeciduousAnnualLeafMortality( MeanTemp );

            // For deciduous plants monthly leaf mortality is weighted by temperature deviance from the average, to capture seasonal patterns

            double Weight = Environment::Get( "ExpTDevWeight", gcl );
            MonthlyLeafMortRate = AnnualLeafMortRate * Weight;
            TimeStepLeafMortRate = MonthlyLeafMortRate * Utilities.ConvertTimeUnits( GlobalModelTimeStepUnit, "month" );
        } else {
            // Calculate annual evergreen leaf mortality
            AnnualLeafMortRate = CalculateEvergreenAnnualLeafMortality( MeanTemp );

            // For evergreen plants, leaf mortality is assumed to be equal throughout the year
            MonthlyLeafMortRate = AnnualLeafMortRate * ( 1.0 / 12.0 );
            TimeStepLeafMortRate = MonthlyLeafMortRate * Utilities.ConvertTimeUnits( GlobalModelTimeStepUnit, "month" );
        }

        // Calculate fine root mortality rate
        double AnnualFRootMort = CalculateFineRootMortalityRate( Environment::Get( "Temperature", gcl ) );

        // Calculate the NPP allocated to non-structural tissues
        double FracNonStruct = ( 1 - FracStruct );

        // Calculate the fractional allocation of NPP to evergreen plant matter
        double FracEvergreen = CalculateFracEvergreen( Environment::Get( "Fraction Year Frost", gcl ) );

        // Calculate the fractional allocation to leaves
        double FracLeaves = FracNonStruct * CalculateLeafFracAllocation( AnnualLeafMortRate, CalculateDeciduousAnnualLeafMortality( MeanTemp ),
                CalculateEvergreenAnnualLeafMortality( MeanTemp ), FracEvergreen, AnnualFRootMort );


        // Update NPP depending on whether the acting stock is deciduous or evergreen
        if( deciduous ) {
            NPP *= ( 1 - FracEvergreen );
        } else {
            NPP *= FracEvergreen;
        }

        // Calculate the fire mortality rate
        double FireMortRate = CalculateFireMortalityRate( NPP, Environment::Get( "Fraction Year Fire", gcl ) );

        // Calculate the structural mortality rate
        double StMort = CalculateStructuralMortality( Environment::Get( "AET", gcl ) * 12 );

        // Calculate leaf C fixation
        double LeafCFixation = NPP * FracLeaves;

        // Convert from carbon to leaf wet matter
        double WetMatterIncrement = ConvertToLeafWetMass( LeafCFixation, gcl.GetCellArea( ) );
        // Convert from the monthly time step used for this process to the global model time step unit
        WetMatterIncrement *= Utilities.ConvertTimeUnits( GlobalModelTimeStepUnit, "month" );
        
        // Add the leaf wet matter to the acting stock - this line has been change in the latest version!
        //actingStock.TotalBiomass += max( -actingStock.TotalBiomass, WetMatterIncrement );
        double NPPWetMatter = max(-actingStock.TotalBiomass, WetMatterIncrement);


        // Calculate fractional leaf mortality
        double LeafMortFrac = 1 - exp( -TimeStepLeafMortRate );

        // Update the leaf stock biomass owing to the leaf mortality
        actingStock.TotalBiomass *= ( 1 - LeafMortFrac );
        NPPWetMatter *=(1-LeafMortFrac);

        return NPPWetMatter;
        
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate NPP in kg C per m2
    @param temperature Current temperature, in degrees Celsius 
    @param precipitation Current precipitation, in mm 
    @return */
    double CalculateMiamiNPP( double temperature, double precipitation ) {
        // Calculate the maximum annual NPP that could be sustained if average temperature were equal to this month's temperature
        double NPPTemp = max_NPP / ( 1 + exp( t1_NPP - t2_NPP * temperature ) );

        // Calculate theC:\madingley-ecosystem-model\Madingley\Ecological processes cohorts\Reproduction implementations\ReproductionBasic.cs maximum annual NPP that could be sustained if precipitation in every other month was equal to this month's precipitation
        double NPPPrecip = max_NPP * ( 1 - exp( -p_NPP * precipitation ) );

        // Calculate the maximum annual NPP that could be sustained based on temperature and precipitation
        return min( NPPTemp, NPPPrecip );

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the fractional allocation of productivity to structural tissue
    @param NPP Net primary productivity 
    @return The fractional allocation of productivity to structural tissue*/
    double CalculateFracStruct( double NPP ) {
        double MinFracStruct = 0.01; // This prevents the prediction becoming zero (makes likelihood calculation difficult)
        double FracStruc = MinFracStruct * ( exp( FracStructScalar * NPP ) / ( 1 + MinFracStruct * ( exp( FracStructScalar * NPP ) - 1.0 ) ) );
        if( FracStruc > 0.99 ) FracStruc = 1 - MinFracStruct;
        FracStruc *= MaxFracStruct;
        return FracStruc;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the fractional allocation of productivity to evergreen plant matter
    @param NDF The proportion of the current year subject to frost 
    @return The fractional allocation of productivity to evergreen plant matter*/
    double CalculateFracEvergreen( double NDF ) {
        double imed1 = a_FracEvergreen * NDF * NDF + b_FracEvergreen * NDF + c_FracEvergreen;
        if( imed1 < 0 ) imed1 = 0;
        if( imed1 > 1 ) imed1 = 1;
        return imed1;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the mortality rate of evergreen leaves
    @param temperature Current temperature, in degrees Celsius 
    @return The mortality rate of evergreen leaves*/
    double CalculateEvergreenAnnualLeafMortality( double temperature ) {
        double EstimatedRate = exp( m_EGLeafMortality * temperature - c_EGLeafMortality );
        if( EstimatedRate > er_max ) EstimatedRate = er_max;
        if( EstimatedRate < er_min ) EstimatedRate = er_min;
        return EstimatedRate;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the mortality rate of deciduous leaves
    @param temperature Current temperature, in degrees Celsius 
    @return The mortality rate of deciduous leaves*/
    double CalculateDeciduousAnnualLeafMortality( double temperature ) {
        double EstimatedRate = exp( -( m_DLeafMortality * temperature + c_DLeafMortality ) );
        if( EstimatedRate > dr_max ) EstimatedRate = dr_max;
        if( EstimatedRate < dr_min ) EstimatedRate = dr_min;
        return EstimatedRate;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the fraction of NPP allocated to non-structural tissue that is allocated to leaves
    @param LeafMortRate The mortality rate of leaves 
    @param FRootMort The mortality rate of fine roots 
    @return The fractional mortality of leaves*/
    double CalculateLeafFracAllocation( double LeafMortRate, double DecidLeafMortRate, double EvergreenLeafMortRate, double FracEvergreen, double FRootMort ) {
        //Changed in the latest version!
        //double CombinedLeafMortRate = exp( ( FracEvergreen * log( EvergreenLeafMortRate ) ) + ( ( 1 - FracEvergreen ) * log( DecidLeafMortRate ) ) );

        //return LeafMortRate / ( CombinedLeafMortRate + FRootMort );
        return LeafMortRate / (LeafMortRate + FRootMort);
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the mortality rate of fine roots
    @param temperature Current temperature, in degrees Celsius 
    @return The mortality rate of fine roots*/
    double CalculateFineRootMortalityRate( double temperature ) {
        double EstimatedRate = exp( m_FRootMort * temperature + c_FRootMort );
        if( EstimatedRate > frm_max ) EstimatedRate = frm_max;
        if( EstimatedRate < frm_min ) EstimatedRate = frm_min;
        return EstimatedRate;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the rate of plant mortality to fire
    @param NPP Net Primary Productivity, in kg C per m2 
    @param FractionYearFireSeason The fraction of the year subject to fires 
    @return The rate of plant mortality to fire*/
    double CalculateFireMortalityRate( double NPP, double FractionYearFireSeason ) {
        double NPPFunction = ( 1.0 / ( 1.0 + exp( -NPPScalar_Fire * ( NPP - NPPHalfSaturation_Fire ) ) ) );
        double LFSFunction = ( 1.0 / ( 1.0 + exp( -LFSScalar_Fire * ( FractionYearFireSeason - LFSHalfSaturation_Fire ) ) ) );
        double TempRate = BaseScalar_Fire * NPPFunction * LFSFunction;
        if( TempRate > 1.0 ) TempRate = 1.0;
        double Rate = ( TempRate <= MinReturnInterval ) ? MinReturnInterval : TempRate;
        return Rate;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the mortality rate of plant structural tissue
    @param AET Actual evapotranspiration, in mm 
    @return The mortality rate of plant structural tissue*/
    double CalculateStructuralMortality( double AET ) {
        double power = p2_StMort * AET / 1000 + p1_StMort;
        double EstimatedRate = exp( power );
        if( EstimatedRate > stm_max ) EstimatedRate = stm_max;
        if( EstimatedRate < stm_min ) EstimatedRate = stm_min;
        return EstimatedRate;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate leaf carbon, in kg C per m2
    @param NPP Net Primary Productivity, in kg C per m2 
    @param FracStruct Fractional allocation to structural tissue 
    @param LeafMortFrac Fractional mortality of leaves 
    @param LeafMortRate Rate of mortality of leaves 
    @param FireMortRate Rate of mortality to fire 
    @param StMort Rate of mortality of structural tissue 
    @return Leaf carbon, in kg C per m2*/
    double CalculateLeafCarbon( double NPP, double FracStruct, double LeafMortFrac, double LeafMortRate, double FireMortRate, double StMort ) {
        double LeafCFixation = CalculateLeafCFixation( NPP, FracStruct, LeafMortFrac );
        double LeafCMortality = LeafMortRate + FireMortRate + StMort;
        return LeafCFixation / LeafCMortality;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Calculate the carbon fixed by leaves, in kg C per m2

    @param NPP Net Primary Productivity, in kg C per m2 
    @param FracStruct Fractional allocation to structural tissue 
    @param LeafMortFrac Fractional mortality of leaves 
    @return The carbon fixed by leaves, in kg C per m2*/
    double CalculateLeafCFixation( double NPP, double FracStruct, double LeafMortFrac ) {
        return NPP * ( 1 - FracStruct * MaxFracStruct ) * LeafMortFrac;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Convert from kg C per m2 to g of leaf wet matter in the entire grid cell
    @param kgCarbon Value to convert, in kg C per m2 
    @param cellArea The area of the grid cell 
    @return Value in g of wet matter in the grid cell*/
    double ConvertToLeafWetMass( double kgCarbon, double cellArea ) {
        // Convert from kg to g
        double gCarbonPerM2 = kgCarbon * 1000;

        // Convert from m2 to km2
        double gCarbonPerKm2 = gCarbonPerM2 * m2Tokm2Conversion;

        // Convert from km2 to cell area
        double gCarbonPerCell = gCarbonPerKm2 * cellArea;

        // Convert from g C to g dry matter
        // mass of Leaf C per g leaf dry matter = 0.4761 g g-1 (from Kattge et al. (2011), TRY- A global database of plant traits, Global Change Biology)
        double LeafDryMatter = gCarbonPerCell / MassCarbonPerMassLeafDryMatter;

        // Convert from dry matter to wet matter
        // mass of leaf dry matter per g leaf wet matter = 0.213 g g-1 (from Kattge et al. (2011), TRY- A global database of plant traits, Global Change Biology)
        return LeafDryMatter / MassLeafDryMatterPerMassLeafWetMatter;

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Convert from g of plant wet matter in the entire grid cell to kg C per m2
    @param leafWetMatter The value to convert as total g wet matter in the grid cell 
    @param cellArea The area of the grid cell 
    @return Value in kg C per m2*/
    double ConvertToKgCarbonPerM2( double leafWetMatter, double cellArea ) {
        // Convert from wet matter to dry matter
        double LeafDryMatter = leafWetMatter / 2;

        // Convert from dry matter to g C per grid cell
        double gCarbonPerCell = LeafDryMatter * 2;

        // Convert from cell area to km2
        double gCarbonPerKm2 = gCarbonPerCell / cellArea;

        // Convert from km2 to m2
        double gCarbonPerM2 = gCarbonPerKm2 / m2Tokm2Conversion;

        // Convert from g carbon to kg carbon
        return gCarbonPerM2 / 1000;
    }
    //----------------------------------------------------------------------------------------------

};

#endif
