#ifndef AUTOTROPHPROCESSOR_H
#define AUTOTROPHPROCESSOR_H
//#include <ProcessTracker.h>
//#include <GlobalProcessTracker.h>
#include <Environment.h>
#include <GridCell.h>

/** \file AutotrophProcessor.h
 * \brief the AutotrophProcessor header file
 */


//
//namespace Madingley
//{

/** \brief Class for converting primary productivity estimates to autotroph biomass */
class AutotrophProcessor {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief Instance of the class to perform general functions
     */
    UtilityFunctions Utilities;
    /** \brief Factor to convert phytoplankton biomass from grams carbon to grams wet weight
    @remark Currently derived from Ho et al. (2003) J. Phycol., Dalsgaard and Pauly (1997) and Strickland (1966)*/
    const double PhytoplanktonConversionRatio = 10;
    /** \brief Factor to convert NPP from units per m^2 to units per km^2 */
    const double MsqToKmSqConversion = 1000000.0;
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------
    /** \brief Constructor for the autotroph processor: initialises necessary classes */
    AutotrophProcessor() {

    }
    //----------------------------------------------------------------------------------------------
    /** \brief Convert NPP estimate into biomass of an autotroph stock
    @param gcl The current grid cell 
    @param actingStock The stock to add biomass to 
    @param currentTimestep The current model time step
    @param currentMonth Month as an integer 
    @param params Current parameters */
    void ConvertNPPToAutotroph(GridCell& gcl, Stock&  actingStock, 
     unsigned currentTimestep, 
     unsigned currentMonth, MadingleyModelInitialisation& params) {
        // Get NPP from the cell environment
        double NPP = Environment::Get("NPP",gcl);
        // If NPP is a missing value then set to zero
        if (NPP == Environment::MissingValue) NPP = 0.0;

        // Check that this is an ocean cell
        if (gcl.isMarine()) {
            // Check that the units of oceanic NPP are gC per m2 per day
            assert(params.Units["OceanNPP"] == "gC/m2/day" && "Oceanic NPP data are not in the correct units for this formulation of the model");

            //Convert to g/cell/month
            NPP *= MsqToKmSqConversion;

            //Multiply by cell area to get g/cell/day
            NPP *= gcl.CellArea();

            //Convert to g wet matter, assuming carbon content of phytoplankton is 10% of wet matter
            NPP *= PhytoplanktonConversionRatio;

            //Finally convert to g/cell/month and add to the stock totalbiomass
            NPP *= Utilities.ConvertTimeUnits(params.GlobalModelTimeStepUnit, "day");
            actingStock.TotalBiomass += NPP;

            // If the biomass of the autotroph stock has been made less than zero (i.e. because of negative NPP) then reset to zero
            if (actingStock.TotalBiomass < 0.0)
                actingStock.TotalBiomass = 0.0;
        }            // Else if neither on land or in the ocean
        else {
            cout << "This is not a marine cell!" << endl;
            // Set the autotroph biomass to zero
            actingStock.TotalBiomass = 0.0;
        }
        assert(actingStock.TotalBiomass >= 0.0 && "stock negative");
    }
    //----------------------------------------------------------------------------------------------
};
#endif
