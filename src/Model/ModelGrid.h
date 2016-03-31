#ifndef MODELGRID_H
#define MODELGRID_H
using namespace std;
#include <GridCell.h>
#include <map>
#include <limits>
#include <string>

#include "Parameters.h"
#include "Processor.h"
#include "Logger.h"
//class MadingleyModelInitialisation;

/** \brief A class containing the model grid (composed of individual grid cells) along with grid attributes.
           The model grid is referenced by [Lat index, Lon index]
 */
class ModelGrid {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------

    // Variable to make sure that not more than one grid is instantiated
    unsigned NumGrids = 0;
    /** \brief Array of grid cells */
    map<long, GridCell > Cells;

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
        NumGrids = NumGrids + 1;
        assert( NumGrids < 2 && "You have initialised more than one grid on which to apply models. At present, this is not supported" );
        Logger::Get( )->LogMessage( "Initialising grid cell environment" );

        // Instantiate a grid of grid cells
        for( unsigned longitudeIndex = 0; longitudeIndex < Parameters::Get( )->GetLengthUserLongitudeArray( ); longitudeIndex++ ) {
            for( unsigned latitudeIndex = 0; latitudeIndex < Parameters::Get( )->GetLengthUserLatitudeArray( ); latitudeIndex++ ) {
                // Create the grid cell at the specified position
                unsigned index = latitudeIndex + Parameters::Get( )->GetLengthUserLatitudeArray( ) * longitudeIndex;
                Cells[ index ].setCellCoords( Parameters::Get( )->GetUserLatitudeAtIndex( latitudeIndex ), latitudeIndex, Parameters::Get( )->GetUserLongitudeAtIndex( longitudeIndex ), longitudeIndex );
            }
        }

        cout << "\n" << endl;

    }
    //----------------------------------------------------------------------------------------------

    /** \brief  Get pointer to a viable cell to move to
    @param  gcl Pointer to the focal grid cell
    @param  v latitudinal displacement
    @param  u longitudinal displacement
    @return Pointer to cell that lies at displacement u,v from the current cell
    @remark Currently assumes wrapping in longitude, and a hard upper and lower boundary in latitude
     */
    GridCell* getNewCell( GridCell* gcl, const int& v, const int& u ) {
        GridCell* Cell = 0;
        unsigned latCell = gcl->LatIndex( ), lonCell = gcl->LonIndex( );
        if( latCell + v >= 0 && latCell + v < Parameters::Get( )->GetLengthUserLatitudeArray( ) ) {
            int lnc = lonCell + u;
            while( lnc < 0 )lnc += Parameters::Get( )->GetLengthUserLongitudeArray( );
            while( lnc >= Parameters::Get( )->GetLengthUserLongitudeArray( ) )lnc -= Parameters::Get( )->GetLengthUserLongitudeArray( );
            long idx = ( latCell + v ) + Parameters::Get( )->GetLengthUserLatitudeArray( ) * lnc;
            if( Cells.count( idx != 0 ) )Cell = &( Cells[idx] );
        }
        return Cell;
    }
    //----------------------------------------------------------------------------------------------
    //Apply any function that operates on a cell to all cells in the collection

    template <typename F>
    void ask( F f ) {
        for( auto& j: Cells ) {
            f( j.second );
        }
    }
};
#endif