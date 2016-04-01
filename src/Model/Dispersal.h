#ifndef DISPERSAL_H
#define DISPERSAL_H
#include <string>
#include <GridCell.h>
#include <IDispersalImplementation.h>
#include <limits>
#include <TAdvectiveDispersal.h>
#include <TResponsiveDispersal.h>
#include <TDiffusiveDispersal.h>

#include "Parameters.h"
using namespace std;
/** \file Dispersal.h
 * \brief the Dispersal header file
 */

/** \brief Performs dispersal */
class Dispersal {
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    vector<Cohort>dispersers;
    /** \brief The available implementations of the dispersal process */
    map<string, IDispersalImplementation*>choose;

public:

    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    Dispersal( ) {
        // Assign dispersal implementations
        choose["advective"] = new AdvectiveDispersal( );
        choose["diffusive"] = new DiffusiveDispersal( );
        choose["responsive"] = new ResponsiveDispersal( );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief tidy up pointers */
    ~Dispersal( ) {
        delete choose["advective"];
        delete choose["diffusive"];
        delete choose["responsive"];
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Run dispersal 
    @param cellIndex The cell index for the active cell in the model grid 
    @param gridForDispersal The model grid to run the process for 
    @param dispersalOnly Whether we are running dispersal only 
    @param madingleyCohortDefinitions The functional group definitions for cohorts in the model 
    @param madingleyStockDefinitions The functional group definitions for stocks in the model 
    @param currentMonth The current model month */
    void RunCrossGridCellEcologicalProcess( GridCell& gcl, ModelGrid& gridForDispersal, MadingleyModelInitialisation& params, unsigned currentMonth ) {

        gcl.ApplyFunctionToAllCohorts( [&]( Cohort & c ) {
            if( choose.count( c.DispersalType( params ) ) != 0 ) {
                choose[c.DispersalType( params )]->RunDispersal( gridForDispersal, c, currentMonth );
            }
            if( c.IsMoving( ) )dispersers.push_back( c );

        } );

    }
    //----------------------------------------------------------------------------------------------

    void UpdateCrossGridCellEcology( unsigned& dispersalCounter ) {
        dispersalCounter = dispersers.size( );

        for( auto& c: dispersers ) {
            c.Move( );
        }
        dispersers.clear( );
    }
};

#endif
