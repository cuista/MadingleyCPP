#ifndef DISPERSAL_H
#define DISPERSAL_H

#include "GridCell.h"
#include "IDispersalImplementation.h"
#include "TAdvectiveDispersal.h"
#include "TResponsiveDispersal.h"
#include "TDiffusiveDispersal.h"
#include "Parameters.h"
#include "Types.h"
/** \file Dispersal.h
 * \brief the Dispersal header file
 */

/** \brief Performs dispersal */
class Dispersal {
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    Types::CohortVector mDispersers;
    /** \brief The available implementations of the dispersal process */
    Types::IDispersalMap mChoose;

public:

    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    Dispersal( ) {
        // Assign dispersal implementations
        mChoose["advective"] = new AdvectiveDispersal( );
        mChoose["diffusive"] = new DiffusiveDispersal( );
        mChoose["responsive"] = new ResponsiveDispersal( );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief tidy up pointers */
    ~Dispersal( ) {
        delete mChoose["advective"];
        delete mChoose["diffusive"];
        delete mChoose["responsive"];
    }

    void ResetRandoms( ) {
        // the original model resets the random number sequence every timestep by creating a new dispersal object
        mChoose["advective"]->ResetRandom( );
        mChoose["diffusive"]->ResetRandom( );
        mChoose["responsive"]->ResetRandom( );
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
            if( mChoose.count( c.DispersalType( params ) ) != 0 ) {
                mChoose[c.DispersalType( params )]->RunDispersal( gridForDispersal, c, currentMonth );

            }
            if( c.IsMoving( ) )mDispersers.push_back( c );

        } );

    }
    //----------------------------------------------------------------------------------------------

    void UpdateCrossGridCellEcology( unsigned& dispersalCounter ) {
        dispersalCounter = mDispersers.size( );

        for( auto& c : mDispersers ) {
            c.Move( );
        }
        mDispersers.clear( );
    }
};

#endif
