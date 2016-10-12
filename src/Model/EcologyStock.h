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

        FunctionalGroupDefinitions& madingleyStockDefinitions = params.mStockFunctionalGroupDefinitions;
        //changes here to make this code consistent with the merge-and-refactor C# code
        if( gcl.IsMarine( ) ) {
            // Run the autotroph processor
            MarineNPPtoAutotrophStock.ConvertNPPToAutotroph( gcl, actingStock );
        } else {
            // Run the dynamic plant model to update the leaf stock for this time step
            double NPPWetMatter=DynamicPlantModel.UpdateLeafStock( gcl, actingStock, currentTimeStep, madingleyStockDefinitions.GetTraitNames( "leaf strategy", actingStock.FunctionalGroupIndex ) == "deciduous", Parameters::Get( )->GetTimeStepUnits( ), currentMonth );

            // Apply human appropriation of NPP - note in the latest C# version this is changed to include the NPPWetMatter calculated above
            double fhanpp=HANPP.RemoveHumanAppropriatedMatter(NPPWetMatter, gcl, Parameters::Get( )->GetHumanNPPExtraction( ), actingStock, currentTimeStep, currentMonth );
            actingStock.TotalBiomass+=NPPWetMatter*(1-fhanpp);
        }
    }
    //----------------------------------------------------------------------------------------------
};

#endif
