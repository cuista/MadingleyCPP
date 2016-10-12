//Magindgley model entry point.
//Changes from original PLOS Biology version:-
//MB 6/8/2015 - 15/11/2015 - converted C# to C++

/** \file Program.cc \brief This is the main entry point for the madingley code */
/**
\brief Main program

Initialise and run the model

 */
#include <fenv.h>

#include <MadingleyModel.h>

#include "FileReader.h"
#include "FileWriter.h"
#include "Logger.h"
//----------------------------------------------------------------------------------------------

/** \brief Starts a model run or set of model runs */
///@todo Complete set up of output directory

int main( ) {
    //this line enables the gdb debugger to catch Nan or floating point problems
    feenableexcept( FE_INVALID | FE_OVERFLOW );
    // Write out model details to the console
    Logger::Get( )->LogMessage( "Madingley model C++ v. 0.\n" );

    std::time_t t = system_clock::to_time_t( high_resolution_clock::now( ) );
    Logger::Get( )->LogMessage( "Model Run started at " + Convertor::Get( )->ToString( std::ctime( &t ) ) );

    FileReader fileReader;

    if( fileReader.ReadFiles( ) == true ) {
        Logger::Get( )->LogMessage( "Files read successfully..." );
        FileWriter fileWriter;

        // Initialise the model
        // Declare an instance of the class that runs a Madingley model simulation
        MadingleyModel madingleyModel;

        // Declare and start a timer
        mStopWatch s;
        s.Start( );

        // Run the simulation
        madingleyModel.RunMadingley( );

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
//----------------------------------------------------------------------------------------------
