#ifndef GRIDCELL_H
#define GRIDCELL_H
#include <vector>
#include <ClimateVariablesCalculator.h>
#include <UtilityFunctions.h>
#include <Stock.h>
#include <Cohort.h>

#include "Environment.h"
#include "Parameters.h"

/** \file GridCell.h
 * \brief the GridCell header file
 */

class GridCell {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief The handler for the cohorts in this grid cell */
    std::vector< std::vector< Cohort> > GridCellCohorts;
    /** \brief The handler for the stocks in this grid cell */
    std::map<signed, std::vector<Stock> > GridCellStocks;


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

    void setCellCoords( float latitude, unsigned latitudeIndex, float longitude, unsigned longitudeIndex ) {
        // set values for this grid cell
        // Also standardise missing values

        // Set the grid cell values of latitude, longitude and missing value as specified
        mLatitude = latitude;
        mLongitude = longitude;

        // Add the grid cell area (in km2) to the cell environment with an initial value of 0
        // Calculate the area of this grid cell
        // Add it to the cell environment
        Cell_Area = Utilities.CalculateGridCellArea( mLatitude, Parameters::Get( )->GetGridCellSize( ) );
        // Calculate the lengths of widths of grid cells in each latitudinal strip
        // Assume that we are at the midpoint of each cell when calculating lengths
        CellHeightKm = Utilities.CalculateLengthOfDegreeLatitude( mLatitude + Parameters::Get( )->GetGridCellSize( ) / 2 ) * Parameters::Get( )->GetGridCellSize( );
        CellWidthKm = Utilities.CalculateLengthOfDegreeLongitude( mLatitude + Parameters::Get( )->GetGridCellSize( ) / 2 ) * Parameters::Get( )->GetGridCellSize( );
        //Add the latitude and longitude indices
        mLatitudeIndex = latitudeIndex;
        mLongitudeIndex = longitudeIndex;
    }
    //----------------------------------------------------------------------------------------------

    void insert( Cohort& c ) {
        GridCellCohorts[c.mFunctionalGroupIndex].push_back( c );
    }
    //----------------------------------------------------------------------------------------------

    void remove( Cohort& c ) {
        vector<Cohort>& z = GridCellCohorts[c.mFunctionalGroupIndex];
        auto h = find_if( z.begin( ), z.end( ), [c]( Cohort & k ) {
            return c.mID == k.mID;
        } );
        if( c.mID != ( *h ).mID )cout << "Strange things happening in grid delete? " << c.mID << " " << ( *h ).mID << endl;
        z.erase( h );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Move a new cohort to another grid cell
    @param c The cohort object to move 
     */
    void Move( Cohort& c ) {
        c.Here( ).remove( c );
        c.mLocation = c.mDestination;
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

    bool IsMarine( ) {
        return (Environment::Get( "Realm", *this ) == 2.0 );
    }
    //----------------------------------------------------------------------------------------------

    unsigned GetLatitudeIndex( ) {
        return mLatitudeIndex;
    }
    //----------------------------------------------------------------------------------------------

    unsigned GetLongitudeIndex( ) {
        return mLongitudeIndex;
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

private:
    float mLatitude;
    float mLongitude;
    unsigned mLatitudeIndex;
    unsigned mLongitudeIndex;
};
#endif
