#ifndef STOPWATCH_H
#define STOPWATCH_H
#include <chrono>
using namespace std::chrono;
/** \file Stopwatch.h
    \brief The stopwatch header file
 */

/** \brief Timer to track time elapsed */
class StopWatch {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief The start time of a given stopwatch run */
    high_resolution_clock::time_point mStartTime;
    /** \brief The stop time of a given stopwatch run */
    high_resolution_clock::time_point mStopTime;
    /** \brief Whether the stopwatch is running */
    bool running = false;
    /** \brief Time accumlated between last start and stop */
    duration< double > mInterval;
    /** \brief Time accumlated since this stopwatch was created */
    duration< double > mAccumulatedTime;
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------

    /** \brief Constructor - watch defaults to stopped */
    StopWatch( ) {
        running = false;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Start the stopwatch */
    void Start( ) {
        // Set the start time for the stopwatch run
        mStartTime = high_resolution_clock::now( );
        // Set the stopwatch as being running
        running = true;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Stop the stopwatch */

    void Stop( ) {
        // Set the stop time for the stopwatch run
        mStopTime = high_resolution_clock::now( );
        // Set the stopwatch as being not running
        running = false;
        // Calculate the time elapsed during this stopwatch run
        mInterval = ( mStopTime - mStartTime );
        // Update the time accumulated by this stopwatch instance
        mAccumulatedTime += mInterval;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Get the non-cumulative elapsed time of a stopwatch run in milliseconds */
    double GetElapsedTimeMillis( ) {
        //set the units for output - default is second so ratio of 1000 get milliseconds
        duration<double, ratio < 1, 1000 >> elapsed;

        // If the stopwatch is running, then calculate time since the stopwatch started, otherwise use the time elapsed during the last stopwatch run

        if( running )
            elapsed = high_resolution_clock::now( ) - mStartTime;
        else
            elapsed = mInterval;

        return elapsed.count( );
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Get the non-cumulative elapsed time of a stopwatch run in seconds */
    double GetElapsedTimeSecs( ) {
        //set the units for output = default is second
        duration< double > elapsed;
        if( running )
            elapsed = high_resolution_clock::now( ) - mStartTime;
        else
            elapsed = mStopTime - mStartTime;

        return elapsed.count( );

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Get the cumulative elapsed time of a stopwatch run in seconds */
    double AccumulatedTime( ) {
        //units for output - default is second
        return mAccumulatedTime.count( );

    }
    //----------------------------------------------------------------------------------------------
};
#endif
