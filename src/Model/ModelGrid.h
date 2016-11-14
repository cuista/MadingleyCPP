#ifndef MODELGRID_H
#define MODELGRID_H

#include <GridCell.h>

#include "Parameters.h"
#include "Logger.h"

/** \brief A class containing the model grid (composed of individual grid cells) along with grid attributes.
           The model grid is referenced by [Lat index, Lon index]
 */
class ModelGrid {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------

    // Variable to make sure that not more than one grid is instantiated
    unsigned mNumGrids = 0;
    /** \brief Array of grid cells */
    Types::GridCellMap mCells;

    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------
    //empty placeholder constructor just to get started.

    ModelGrid( ) {
        ;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Constructor for model grid to construct the grid for the globe
     */
    void SetUpGrid( ) {
        // Add one to the counter of the number of grids. If there is more than one model grid, exit the program with a debug crash.
        mNumGrids = mNumGrids + 1;
        assert( mNumGrids < 2 && "You have initialised more than one grid on which to apply models. At present, this is not supported" );
        Logger::Get( )->LogMessage( "Initialising grid cell environment" );

        // Instantiate a grid of grid cells
        for( unsigned index = 0; index < Parameters::Get( )->GetNumberOfGridCells( ); ++index ) {
            // Create the grid cell at the specified position
            mCells[ index ].SetCellCoords( index );
        }
        Logger::Get( )->LogMessage( "" );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief  Get pointer to a viable cell to move to
    @param  gcl Pointer to the focal grid cell
    @param  v latitudinal displacement
    @param  u longitudinal displacement
    @return Pointer to cell that lies at displacement u,v from the current cell
    @remark Currently assumes wrapping in longitude, and a hard upper and lower boundary in latitude
     */
    GridCell* getNewCell( GridCell* gridCell, const int& v, const int& u ) {

        GridCell* newCell = NULL;
        if( gridCell->GetLatitudeIndex( ) + v >= 0 && gridCell->GetLatitudeIndex( ) + v < Parameters::Get( )->GetLengthUserLatitudeArray( ) ) {
            int lnc = gridCell->GetLongitudeIndex( ) + u;
            while( lnc < 0 )lnc += Parameters::Get( )->GetLengthUserLongitudeArray( );
            while( lnc >= Parameters::Get( )->GetLengthUserLongitudeArray( ) )lnc -= Parameters::Get( )->GetLengthUserLongitudeArray( );
            long idx = lnc + Parameters::Get( )->GetLengthUserLongitudeArray( ) * ( gridCell->GetLatitudeIndex( ) + v );
            if( mCells.count( idx ) != 0 ) newCell = &( mCells[idx] );
        }
        return newCell;
    }
    //----------------------------------------------------------------------------------------------

    Location getNewCell( const Location& L, const int& v, const int& u ) {
        Location value = L;
        if( L.mLatitudeIndex + v >= 0 && L.mLongitudeIndex + v < Parameters::Get( )->GetLengthUserLatitudeArray( ) ) {
            int lnc = L.mLongitudeIndex + u;
            while( lnc < 0 )lnc += Parameters::Get( )->GetLengthUserLongitudeArray( );
            while( lnc >= Parameters::Get( )->GetLengthUserLongitudeArray( ) )lnc -= Parameters::Get( )->GetLengthUserLongitudeArray( );
            long idx = lnc + Parameters::Get( )->GetLengthUserLongitudeArray( ) * ( L.mLatitudeIndex + v );
            if( mCells.count( idx ) != 0 ) {
                value.SetIndices( mCells[idx].GetLatitudeIndex( ), mCells[idx].GetLongitudeIndex( ) );
            }
        }
        return value;
    }
    //----------------------------------------------------------------------------------------------

    GridCell& getACell( const Location& L ) {
        long idx = L.mLongitudeIndex + Parameters::Get( )->GetLengthUserLongitudeArray( ) * ( L.mLatitudeIndex );
        return mCells[idx];
    }
    //----------------------------------------------------------------------------------------------
    //Apply any function that operates on a cell to all cells in the collection

    template <typename Function>
    void ApplyFunctionToAllCells( Function f ) {
        for( auto& j : mCells ) {
            f( j.second );
        }
    }
};
#endif