#ifndef MASSBINHANDLER_H
#define MASSBINHANDLER_H

#include "Types.h"
#include "Constants.h"
#include "Logger.h"
//namespace Madingley
//{

/** \brief Handles the mass bins to be used in model outputs */

class MassBinsHandler {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief The number of mass bins to be used for outputs */
    int mNumMassBins = 50;
    /** \brief A vector containing the masses corresponding to the mass bins */
    Types::FloatVector mMassBins;
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief Sets up mass bins based on an input file
    @param massBinsFile The filename containing the mass bin information 
     */
    void SetUpMassBins( std::string massBinsFile ) {

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
    //----------------------------------------------------------------------------------------------

    /** \brief Returns the mass bins copied from file
    @return the mass bins copied from file
     */
    //    Types::FloatVector const& GetSpecifiedMassBins( ) const {
    //        return mMassBins;
    //    }
    //----------------------------------------------------------------------------------------------
};
#endif