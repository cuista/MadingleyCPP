#ifndef ECOLOGYSTOCK_H
#define ECOLOGYSTOCK_H
#include <AutotrophProcessor.h>
#include <RevisedTerrestrialPlantModel.h>
#include <HANPP.h>

#include <MadingleyModelInitialisation.h>
#include "Parameters.h"
/** \file EcologyStock.h
 * \brief the EcologyStock header file
 */

//namespace Madingley
//{

/** \brief A class to specify, initialise and run ecological processes pertaining to stocks */
class EcologyStock {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief An instance of the Autotroph Processor for this model */
    AutotrophProcessor MarineNPPtoAutotrophStock;

    /** \brief An instance of the plant model class */
    RevisedTerrestrialPlantModel DynamicPlantModel;

    /** \brief An instance of the class for human appropriation of NPP */
    HumanAutotrophMatterAppropriation HANPP;

    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief Run ecological processes that operate on stocks within a single grid cell 
    @param gcl The current grid cell 
    @param actingStock The acting stock 
    @param currentTimeStep The current model time step 
    @param currentMonth The current model month 
    @param params Parameters */

    void RunWithinCellEcology( GridCell& gcl, Stock& actingStock, unsigned currentTimeStep, unsigned currentMonth, MadingleyModelInitialisation& params ) {
        FunctionalGroupDefinitions& madingleyStockDefinitions = params.StockFunctionalGroupDefinitions;

        if( gcl.isMarine( ) ) {
            // Run the autotroph processor
            MarineNPPtoAutotrophStock.ConvertNPPToAutotroph( gcl, actingStock, currentTimeStep, currentMonth, params );
        } else {
            // Run the dynamic plant model to update the leaf stock for this time step
            DynamicPlantModel.UpdateLeafStock( gcl, actingStock, currentTimeStep, madingleyStockDefinitions.GetTraitNames( "leaf strategy", actingStock.FunctionalGroupIndex ) == "deciduous", Parameters::Get( )->GetTimeStepUnits( ), currentMonth );

            // Apply human appropriation of NPP
            HANPP.RemoveHumanAppropriatedMatter( gcl, Parameters::Get( )->GetHumanNPPExtraction( ), actingStock, currentTimeStep, currentMonth );

        }
    }
    //----------------------------------------------------------------------------------------------
};

#endif
