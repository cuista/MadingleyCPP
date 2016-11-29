#ifndef HANPP_H
#define HANPP_H

#include <GridCell.h>
#include "Constants.h"

/** \file HANPP.h
 * \brief the HANPP header file
 */

/** \brief Removes autotroph matter appropriated by humans from a grid cell's autotroph stocks
\remarks   Assumes that autotroph matter is appropriated evenly from different stocks in proportion to their biomass */
class HumanAutotrophMatterAppropriation {
public:
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    /** \brief Constructor for human appropriation of autotroph matter */
    HumanAutotrophMatterAppropriation( ) {
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Remove human appropriated matter from the grid cell autotroph stocks
    @param gcl The current grid cell 
    @param humanNPPExtraction The type of NPP extraction to apply: 'no' = no removal; 'hanpp' = appropriated NPP estimate from input map; or proportion of total NPP 
    @param gridCellStocks The stocks in the current grid cell 
    @param actingStock The position of the acting stock in the jagged array of grid cell stocks 
    @param currentTimestep The current model time step */
    double RemoveHumanAppropriatedMatter(double NPPWetMatter, GridCell& gcl, std::string humanNPPExtraction, Stock& actingStock, unsigned currentTimestep, unsigned currentMonth ) {
        // Factor to convert NPP from units per m2 to units per km2
       /* double m2Tokm2Conversion = 1000000.0;


        if( humanNPPExtraction == "hanpp" ) {
            // Loop over stocks in the grid cell and calculate the total biomass of all stocks
            double TotalAutotrophBiomass = 0.0;
            for( auto& stockFunctionalGrouping: gcl.mGridCellStocks ) {
                vector<Stock> stockFunctionalGroup = stockFunctionalGrouping.second;
                for( int i = 0; i < stockFunctionalGroup.size( ); i++ ) {
                    TotalAutotrophBiomass += stockFunctionalGroup[i].TotalBiomass;
                }
            }

            // Get the total amount of NPP appropriated by humans from this cell
            double HANPP = Environment::Get( "TerrestrialHANPP", gcl ) * Environment::Get( "Seasonality", gcl );

            // If HANPP value is missing, then assume zero
            if( HANPP == Constants::cMissingValue ) HANPP = 0.0;

            // Allocate HANPP for this stock according to the proportion of total autotroph biomass that the stock represents
            if( TotalAutotrophBiomass == 0.0 ) {
                HANPP = 0.0;
            } else {
                HANPP *= ( actingStock.TotalBiomass / TotalAutotrophBiomass );
            }


            // Convert gC/m2/month to gC/km2/month
            HANPP *= m2Tokm2Conversion;

            // Multiply by cell area (in km2) to get g/cell/day
            HANPP *= gcl.GetCellArea( );

            // Convert from gC to g dry matter
            double DryMatterAppropriated = HANPP * 2;

            // Convert from g dry matter to g wet matter
            double WetMatterAppropriated = DryMatterAppropriated * 2;


            // Remove human appropriated NPP from total NPP, reduce NPP to 10% available to herbivores and then add to autotroph biomass
            actingStock.TotalBiomass -= WetMatterAppropriated;

            if( actingStock.TotalBiomass < 0.0 ) actingStock.TotalBiomass = 0.0;

        } else if( humanNPPExtraction == "no" ) {
        } else {
            assert( ( double(atof( humanNPPExtraction.c_str( ) ) ) >= 0.0 ) && ( double(atof( humanNPPExtraction.c_str( ) ) ) <= 1.0 ) &&
                    "Human NPP extraction must be specified in the scenarios file as 'no', 'hanpp' or a proportion between zero and one" );

            // Get the proportion of plant matter appropriated as specified
            double ProportionMatterAppropriated = double(atof( humanNPPExtraction.c_str( ) ) );

            // Get the absolute amount of NPP appropriated based on this
            double MatterAppropriated = actingStock.TotalBiomass * ProportionMatterAppropriated;

            // Remove human appropriated NPP from total NPP, reduce NPP to 10% available to herbivores and then add to autotroph biomass
            actingStock.TotalBiomass -= MatterAppropriated;
        }*/
        return 0;
    }
    //----------------------------------------------------------------------------------------------
};
#endif
