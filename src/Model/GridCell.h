#ifndef GRIDCELL_H
#define GRIDCELL_H
#include <vector>
#include <ClimateVariablesCalculator.h>
#include <UtilityFunctions.h>
#include <Stock.h>
#include <Cohort.h>

#include "Environment.h"
using namespace std;

/** \file GridCell.h
 * \brief the GridCell header file
 */

class GridCell {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief The handler for the cohorts in this grid cell */
    vector< vector< Cohort> > GridCellCohorts;
    /** \brief The handler for the stocks in this grid cell */
    map<signed, vector<Stock> > GridCellStocks;

    /** \brief The latitude of this grid cell */
    float latitude;
    /** \brief The longitude of this grid cell */
    float longitude;
    /** \brief The latitude index in the grid*/
    unsigned latIndex;
    /** \brief The longitude index in the grid*/
    unsigned lonIndex;
    double Cell_Area, CellHeightKm, CellWidthKm;
    /** \brief  Instance of the class to perform general functions*/
    UtilityFunctions Utilities;
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------
    //default constructor - just to get grid set up initially

    GridCell( ) {
        ;
    }
    //----------------------------------------------------------------------------------------------

    void setCellCoords( float _latitude, unsigned _latIndex, float _longitude, unsigned _lonIndex, float cellSize ) {
        // set values for this grid cell
        // Also standardise missing values

        // Set the grid cell values of latitude, longitude and missing value as specified
        latitude = _latitude;
        longitude = _longitude;

        // Add the grid cell area (in km2) to the cell environment with an initial value of 0
        // Calculate the area of this grid cell
        // Add it to the cell environment
        Cell_Area = Utilities.CalculateGridCellArea( latitude, cellSize );
        // Calculate the lengths of widths of grid cells in each latitudinal strip
        // Assume that we are at the midpoint of each cell when calculating lengths
        CellHeightKm = Utilities.CalculateLengthOfDegreeLatitude( latitude + cellSize / 2 ) * cellSize;
        CellWidthKm = Utilities.CalculateLengthOfDegreeLongitude( latitude + cellSize / 2 ) * cellSize;
        //Add the latitude and longitude indices
        latIndex = _latIndex;
        lonIndex = _lonIndex;
    }
    //----------------------------------------------------------------------------------------------

    void insert( Cohort& c ) {
        GridCellCohorts[c.FunctionalGroupIndex].push_back( c );
    }
    //----------------------------------------------------------------------------------------------

    void remove( Cohort& c ) {
        vector<Cohort>& z = GridCellCohorts[c.FunctionalGroupIndex];
        auto h = find_if( z.begin( ), z.end( ), [c]( Cohort & k ) {
            return c.ID == k.ID;
        } );
        if( c.ID != ( *h ).ID )cout << "Strange things happening in grid delete? " << c.ID << " " << ( *h ).ID << endl;
        z.erase( h );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Move a new cohort to another grid cell
    @param c The cohort object to move 
     */
    void Move( Cohort& c ) {
        c.Here( ).remove( c );
        c.location = c.destination;
        c.Here( ).insert( c );
    }
    //----------------------------------------------------------------------------------------------
    //Apply any function to all cohorts in the cell

    template <typename F>
    void ask( F f ) {
        for( int FG = 0; FG < GridCellCohorts.size( ); FG++ ) {
            // Work through the list of cohorts 
            for( Cohort& c: GridCellCohorts[FG] ) {
                f( c );
            }
        }
    }
    //----------------------------------------------------------------------------------------------
    //Apply any function to all stocks in the cell

    template <typename F>
    void askStocks( F f ) {
        for( int FG = 0; FG < GridCellStocks.size( ); FG++ ) {
            // Work through the list of cohorts 
            for( Stock& s: GridCellStocks[FG] ) {
                f( s );
            }
        }
    }
    //----------------------------------------------------------------------------------------------
    //Randomise cohort order jn a cell

    void randomizeCohorts( ) {
        for( int FG = 0; FG < GridCellCohorts.size( ); FG++ ) {
            random_shuffle( GridCellCohorts[FG].begin( ), GridCellCohorts[FG].end( ) );
        }
    }
    //----------------------------------------------------------------------------------------------

    double Realm( ) {
        return Environment::Get( "Realm", *this );
    }
    //----------------------------------------------------------------------------------------------

    bool isMarine( ) {
        return (Environment::Get( "Realm", *this ) == 2.0 );
    }
    //----------------------------------------------------------------------------------------------

    unsigned LatIndex( ) {
        return latIndex;
    }
    //----------------------------------------------------------------------------------------------

    unsigned LonIndex( ) {
        return lonIndex;
    }
    //----------------------------------------------------------------------------------------------

    double CellArea( ) {
        return Cell_Area;
    }
    //----------------------------------------------------------------------------------------------

    void setCohortSize( unsigned n ) {
        GridCellCohorts.resize( n );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Gets the number of cohorts in this grid cell */
    int GetNumberOfCohorts( ) {
        int sum = 0;
        for( unsigned ii = 0; ii < GridCellCohorts.size( ); ii++ ) {
            sum += GridCellCohorts[ii].size( );
        }
        return sum;
    }

};
#endif
