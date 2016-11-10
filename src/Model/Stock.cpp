/** \file Stock.cc
 * \brief the Stock implementation file
 */
#include <RevisedTerrestrialPlantModel.h>
#include <Environment.h>
#include <GridCell.h>

#include "Constants.h"
using namespace std;
//----------------------------------------------------------------------------------------------
//Methods
//----------------------------------------------------------------------------------------------
//Constructor

Stock::Stock( FunctionalGroupDefinitions& StockDefinitions, const unsigned FunctionalGroup, GridCell& gcl, bool& success ) {

    FunctionalGroupIndex = FunctionalGroup;

    // Get the individual body masses for organisms in each stock functional group
    IndividualBodyMass = StockDefinitions.GetBiologicalPropertyOneFunctionalGroup( "individual mass", FunctionalGroup );

    success = false;

    // If it is a functional group that corresponds to the current realm, then seed the stock
    if( !gcl.IsMarine( ) && Environment::Get( "Precipitation", gcl ) != Constants::cMissingValue && Environment::Get( "Temperature", gcl ) != Constants::cMissingValue ) {
        if( StockDefinitions.GetTraitNames( "Realm", FunctionalGroup ) == "terrestrial" ) {
            // An instance of the terrestrial carbon model class
            RevisedTerrestrialPlantModel PlantModel;

            // Calculate predicted leaf mass at equilibrium for this stock
            TotalBiomass = PlantModel.CalculateEquilibriumLeafMass( gcl, StockDefinitions.GetTraitNames( "leaf strategy", FunctionalGroup ) == "deciduous" );
            success = true;
        }
    } else if( gcl.IsMarine( ) && Environment::Get( "NPP", gcl ) != Constants::cMissingValue ) {
        if( StockDefinitions.GetTraitNames( "Realm", FunctionalGroup ) == "marine" ) {
            TotalBiomass = 1.e12;
            success = true;
        }
    }
    mCell = &gcl;
}



