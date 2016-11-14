#include "GridCell.h"

GridCell::GridCell( ) {
    // FIX - Should GridCell be initialised here?
}

void GridCell::SetCellCoords( unsigned index ) {
    // set values for this grid cell
    // Also standardise missing values

    mIndex = index;
    Types::DataIndicesPointer indices = Parameters::Get( )->GetDataIndicesFromCellIndex( mIndex );

    //Add the latitude and longitude indices
    mLatitudeIndex = indices->GetY( );
    mLongitudeIndex = indices->GetX( );

    // Add the grid cell area (in km2) to the cell environment with an initial value of 0
    // Calculate the area of this grid cell
    // Add it to the cell environment- latitude value is cell lower left corner
    //Is this really consistent with below? The C# code also has a half cell shift, but in the height and width
    //whereas the utilities code says it wants lower left corner for both areas and lengths
    mCellArea = mUtilities.CalculateGridCellArea( Parameters::Get( )->GetUserLatitudeAtIndex( indices->GetY( ) ), Parameters::Get( )->GetGridCellSize( ) );
    // Calculate the lengths of widths of grid cells in each latitudinal strip
    // Assume that we are at the midpoint of each cell when calculating lengths
    mCellHeightKm = mUtilities.CalculateLengthOfDegreeLatitude( Parameters::Get( )->GetUserLatitudeAtIndex( indices->GetY( ) ) + Parameters::Get( )->GetGridCellSize( ) / 2 ) * Parameters::Get( )->GetGridCellSize( );
    mCellWidthKm = mUtilities.CalculateLengthOfDegreeLongitude( Parameters::Get( )->GetUserLatitudeAtIndex( indices->GetY( ) ) + Parameters::Get( )->GetGridCellSize( ) / 2 ) * Parameters::Get( )->GetGridCellSize( );
}

void GridCell::InsertCohort( Cohort& c ) {
    mCohorts[ c.mFunctionalGroupIndex ].push_back( c );
}

void GridCell::RemoveCohort( Cohort& c ) {
    vector<Cohort>& z = mCohorts[ c.mFunctionalGroupIndex ];
    auto h = find_if( z.begin( ), z.end( ), [ c ]( Cohort & k ) {
        return c.mID == k.mID;
    } );
    if( c.mID != ( *h ).mID ) Logger::Get( )->LogMessage( "Strange things happening in grid delete? " + Convertor::Get( )->ToString( c.mID ) + " " + Convertor::Get( )->ToString( ( *h ).mID ) );
    z.erase( h );
}

void GridCell::MoveCohort( Cohort& c ) {
    c.GetCurrentCell( ).RemoveCohort( c );
    c.SetCurrentCell( c.mDestinationCell );
    c.GetCurrentCell( ).InsertCohort( c );
}

void GridCell::RandomizeCohorts( ) {
    for( int index = 0; index < mCohorts.size( ); index++ ) {
        random_shuffle( mCohorts[ index ].begin( ), mCohorts[ index ].end( ) );
    }
}

double GridCell::Realm( ) {
    return Environment::Get( "Realm", *this );
}

bool GridCell::IsMarine( ) {
    return ( Environment::Get( "Realm", *this ) == 2.0 );
}

unsigned GridCell::GetIndex( ) const {
    return mIndex;
}

unsigned GridCell::GetLatitudeIndex( ) const {
    return mLatitudeIndex;
}

unsigned GridCell::GetLongitudeIndex( ) const {
    return mLongitudeIndex;
}

double GridCell::GetCellArea( ) const {
    return mCellArea;
}

double GridCell::GetCellHeight( ) const {
    return mCellHeightKm;
}

double GridCell::GetCellWidth( ) const {
    return mCellWidthKm;
}

void GridCell::SetCohortSize( unsigned size ) {
    mCohorts.resize( size );
}

/** \brief Gets the number of cohorts in this grid cell */
unsigned GridCell::GetNumberOfCohorts( ) {
    unsigned sum = 0;
    for( unsigned cohortIndex = 0; cohortIndex < mCohorts.size( ); cohortIndex++ ) {
        sum += mCohorts[ cohortIndex ].size( );
    }
    return sum;
}
