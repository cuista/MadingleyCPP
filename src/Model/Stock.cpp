/** \file Stock.cc
 * \brief the Stock implementation file
 */
#include <RevisedTerrestrialPlantModel.h>
#include <Environment.h>
#include <GridCell.h>
using namespace std;
//----------------------------------------------------------------------------------------------
//Methods
//----------------------------------------------------------------------------------------------
//Constructor

Stock::Stock( FunctionalGroupDefinitions& StockDefinitions, const unsigned FunctionalGroup, GridCell& gcl, bool& success ) {

    FunctionalGroupIndex = FunctionalGroup;

    // Get the individual body masses for organisms in each stock functional group
    IndividualBodyMass = StockDefinitions.GetBiologicalPropertyOneFunctionalGroup( "individual mass", FunctionalGroup );

    double Mass = 0;
    success = false;

    // If it is a functional group that corresponds to the current realm, then seed the stock
    if( !gcl.isMarine( ) && Environment::Get( "Precipitation", gcl ) != Environment::MissingValue && Environment::Get( "Temperature", gcl ) != Environment::MissingValue ) {
        if( StockDefinitions.GetTraitNames( "Realm", FunctionalGroup ) == "terrestrial" ) {
            // An instance of the terrestrial carbon model class
            RevisedTerrestrialPlantModel PlantModel;

            // Calculate predicted leaf mass at equilibrium for this stock
            TotalBiomass = PlantModel.CalculateEquilibriumLeafMass( gcl, StockDefinitions.GetTraitNames( "leaf strategy", FunctionalGroup ) == "deciduous" );
            success = true;
        }
    } else if( gcl.isMarine( ) && Environment::Get( "NPP", gcl ) != Environment::MissingValue ) {
        if( StockDefinitions.GetTraitNames( "Realm", FunctionalGroup ) == "marine" ) {
            TotalBiomass = 1.e12;
            success = true;
        }
    }
    location = &gcl;
}



