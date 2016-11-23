#include "DispersalSet.h"

DispersalSet::DispersalSet( ) {
    // Assign dispersal implementations
    mChoose[ "advective" ] = new DispersalAdvective( );
    mChoose[ "diffusive" ] = new DispersalDiffusive( );
    mChoose[ "responsive"] = new DispersalResponsive( );
}

/** \brief tidy up pointers */
DispersalSet::~DispersalSet( ) {
    delete mChoose[ "advective" ];
    delete mChoose[ "diffusive" ];
    delete mChoose[ "responsive" ];
}

void DispersalSet::ResetRandoms( ) {
    // the original model resets the random number sequence every timestep by creating a new dispersal object
    mChoose[ "advective" ]->ResetRandom( );
    mChoose[ "diffusive" ]->ResetRandom( );
    mChoose[ "responsive" ]->ResetRandom( );
}

/** \brief Run dispersal 
@param cellIndex The cell index for the active cell in the model grid 
@param gridForDispersal The model grid to run the process for 
@param dispersalOnly Whether we are running dispersal only 
@param madingleyCohortDefinitions The functional group definitions for cohorts in the model 
@param madingleyStockDefinitions The functional group definitions for stocks in the model 
@param currentMonth The current model month */
void DispersalSet::RunCrossGridCellEcologicalProcess( GridCell& gcl, Grid& gridForDispersal, MadingleyInitialisation& params, unsigned currentMonth ) {
    gcl.ApplyFunctionToAllCohorts( [&]( Cohort & c ) {
        if( mChoose.count( c.GetDispersalType( params ) ) != 0 ) {
            mChoose[c.GetDispersalType( params )]->Run( gridForDispersal, c, currentMonth );

        }
        if( c.IsMoving( ) )mDispersers.push_back( c );

    } );

}

void DispersalSet::UpdateCrossGridCellEcology( unsigned& dispersalCounter ) {
    dispersalCounter = mDispersers.size( );
    for( auto& c: mDispersers ) {
        c.Move( );
    }
    mDispersers.clear( );
}
