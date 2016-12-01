#include <fenv.h>

#include <Madingley.h>

#include "FileReader.h"
#include "FileWriter.h"
#include "Logger.h"
#include "Date.h"

int main( ) {
    //this line enables the gdb debugger to catch Nan or floating point problems
    feraiseexcept( FE_INVALID | FE_OVERFLOW );
    // Write out model details to the console
    Logger::Get( )->LogMessage( "Madingley model C++ v. 0.\n" );
    Logger::Get( )->LogMessage( "Model Run started at " + Date::GetDateAndTimeString( Constants::cCompleteDateFormat ) );

    FileReader fileReader;

    if( fileReader.ReadFiles( ) == true ) {
        Logger::Get( )->LogMessage( "Files read successfully..." );
        Logger::Get( )->LogMessage( "" );
        FileWriter fileWriter;

        // Initialise the model
        // Declare an instance of the class that runs a Madingley model simulation
        Madingley madingleyModel;

        // Declare and start a timer
        Stopwatch s;
        s.Start( );

        // Run the simulation
        madingleyModel.Run( );

        if( fileWriter.WriteFiles( ) == true )
            Logger::Get( )->LogMessage( "Files written to \"" + fileWriter.GetOutputDirectory( ) + "\" successfully..." );

        // Stop the timer and write out the time taken to run this simulation
        s.Stop( );
        Logger::Get( )->LogMessage( "Model run finished" );
        Logger::Get( )->LogMessage( "Total elapsed time was " + Convertor::Get( )->ToString( s.GetElapsedTimeSecs( ) ) + " seconds." );
    } else {
        Logger::Get( )->LogMessage( "ERROR> File reading failed. System exiting..." );
    }
    return 0;
}
