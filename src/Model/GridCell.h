#ifndef GRIDCELL
#define GRIDCELL

#include <UtilityFunctions.h>
#include <Stock.h>
#include <Cohort.h>

#include "Environment.h"
#include "Parameters.h"

class GridCell {
public:

    Types::Cohort2DVector mGridCellCohorts;
    Types::StocksMap mGridCellStocks;

    GridCell( ) {

    }

    void SetCellCoords( float latitude, unsigned latitudeIndex, float longitude, unsigned longitudeIndex ) {
        // set values for this grid cell
        // Also standardise missing values

        // Set the grid cell values of latitude, longitude and missing value as specified
        mLatitude = latitude;
        mLongitude = longitude;

        // Add the grid cell area (in km2) to the cell environment with an initial value of 0
        // Calculate the area of this grid cell
        // Add it to the cell environment
        mCellArea = mUtilities.CalculateGridCellArea( mLatitude, Parameters::Get( )->GetGridCellSize( ) );
        // Calculate the lengths of widths of grid cells in each latitudinal strip
        // Assume that we are at the midpoint of each cell when calculating lengths
        mCellHeightKm = mUtilities.CalculateLengthOfDegreeLatitude( mLatitude + Parameters::Get( )->GetGridCellSize( ) / 2 ) * Parameters::Get( )->GetGridCellSize( );
        mCellWidthKm = mUtilities.CalculateLengthOfDegreeLongitude( mLatitude + Parameters::Get( )->GetGridCellSize( ) / 2 ) * Parameters::Get( )->GetGridCellSize( );
        //Add the latitude and longitude indices
        mLatitudeIndex = latitudeIndex;
        mLongitudeIndex = longitudeIndex;
    }

    void InsertCohort( Cohort& c ) {
        mGridCellCohorts[ c.mFunctionalGroupIndex ].push_back( c );
    }

    void RemoveCohort( Cohort& c ) {
        vector<Cohort>& z = mGridCellCohorts[ c.mFunctionalGroupIndex ];
        auto h = find_if( z.begin( ), z.end( ), [ c ]( Cohort & k ) {
            return c.mID == k.mID;
        } );
        if( c.mID != ( *h ).mID )cout << "Strange things happening in grid delete? " << c.mID << " " << ( *h ).mID << endl;
        z.erase( h );
    }

    void MoveCohort( Cohort& c ) {
        c.GetCurrentLocation( ).RemoveCohort( c );
        c.SetCurrentLocation( c.mDestination );
        c.GetCurrentLocation( ).InsertCohort( c );
    }

    void RandomizeCohorts( ) {
        for( int index = 0; index < mGridCellCohorts.size( ); index++ ) {
            random_shuffle( mGridCellCohorts[ index ].begin( ), mGridCellCohorts[ index ].end( ) );
        }
    }

    template <typename F>
    void ApplyFunctionToAllCohorts( F f ) {
        for( int index = 0; index < mGridCellCohorts.size( ); index++ ) {
            // Work through the list of cohorts 
            for( Cohort& c: mGridCellCohorts[ index ] ) {
                f( c );
            }
        }
    }

    template <typename F>
    void ApplyFunctionToAllStocks( F f ) {
        for( int index = 0; index < mGridCellStocks.size( ); index++ ) {
            // Work through the list of cohorts 
            for( Stock& s: mGridCellStocks[ index ] ) {
                f( s );
            }
        }
    }

    double Realm( ) {
        return Environment::Get( "Realm", *this );
    }

    bool IsMarine( ) {
        return ( Environment::Get( "Realm", *this ) == 2.0 );
    }

    unsigned GetLatitudeIndex( ) const {
        return mLatitudeIndex;
    }

    unsigned GetLongitudeIndex( ) const {
        return mLongitudeIndex;
    }

    double GetCellArea( ) const {
        return mCellArea;
    }

    double GetCellHeight( ) const {
        return mCellHeightKm;
    }

    double GetCellWidth( ) const {
        return mCellWidthKm;
    }

    void SetCohortSize( unsigned size ) {
        mGridCellCohorts.resize( size );
    }

    /** \brief Gets the number of cohorts in this grid cell */
    int GetNumberOfCohorts( ) {
        int sum = 0;
        for( unsigned ii = 0; ii < mGridCellCohorts.size( ); ii++ ) {
            sum += mGridCellCohorts[ii].size( );
        }
        return sum;
    }

private:
    float mLatitude;
    float mLongitude;
    unsigned mLatitudeIndex;
    unsigned mLongitudeIndex;
    double mCellArea;
    double mCellHeightKm;
    double mCellWidthKm;

    UtilityFunctions mUtilities;
};
#endif
