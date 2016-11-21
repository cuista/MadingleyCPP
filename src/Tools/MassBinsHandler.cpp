#include "MassBinsHandler.h"

void MassBinsHandler::SetUpMassBins( std::string massBinsFile ) {

    massBinsFile = Constants::cConfigurationDirectory + massBinsFile;

    std::ifstream massFile( massBinsFile.c_str( ) );
    std::string title;
    if( massFile.is_open( ) ) {
        std::getline( massFile, title );
        float f;
        while( !massFile.eof( ) ) {
            massFile>>f;
            if( !massFile.eof( ) )mMassBins.push_back( f );
        }
        // Sort the array of mass bins
        std::sort( mMassBins.begin( ), mMassBins.end( ) );
        massFile.close( );
    } else {
        Logger::Get( )->LogMessage( "Problem with Mass Bins file!! " + massBinsFile );
        exit( 1 );
    }
}
