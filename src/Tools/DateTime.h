#ifndef DATETIME
#define	DATETIME

#include <sys/time.h>   // For struct timeval
#include "Types.h"

class DateTime {
public:
    ~DateTime( );
    static Types::DateTimePointer Get( );

    void StartTiming( );
    void SplitTiming( );
    void EndTiming( );

    std::string ProduceCumulativeTimeString( );
    std::string ProduceSplitTimeString( );
    std::string ProduceTotalTimeString( ) const;
    std::string GetFormattedDateAndTime( const std::string, const bool isEnd = true ) const;
    
    unsigned int GetRandomNumberSeed( ) const;
    
    unsigned int GetCumulativeTimeInSeconds( );
    unsigned int GetCumulativeTimeInMicroseconds( );

    unsigned int GetTotalTimeInSeconds( ) const;
    unsigned int GetTotalTimeInMilliseconds( ) const;
    
    unsigned int GetTimeStep( ) const;
    void SetTimeStep( const unsigned int& );

private:
    DateTime( );
    static Types::DateTimePointer mThis;

    void SetTimeNow( timeval* ) const;
    void CalculateTotalTimeInMilliseconds( );

    timeval mTimeStart;
    timeval mTimeSplit;
    timeval mTimeNow;
    timeval mTimeEnd;
    
    unsigned int mTotalTimeInSeconds;
    unsigned int mTotalTimeInMilliseconds;
    
    unsigned int mStartTimeAndDate;
    unsigned int mEndTimeAndDate;
    
    unsigned int mTimeStep;
};

#endif

