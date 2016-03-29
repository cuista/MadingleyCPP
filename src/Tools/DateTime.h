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
    
    unsigned GetRandomNumberSeed( ) const;
    
    unsigned GetCumulativeTimeInSeconds( );
    unsigned GetCumulativeTimeInMicroseconds( );

    unsigned GetTotalTimeInSeconds( ) const;
    unsigned GetTotalTimeInMilliseconds( ) const;
    
    unsigned GetTimeStep( ) const;
    void SetTimeStep( const unsigned& );

private:
    DateTime( );
    static Types::DateTimePointer mThis;

    void SetTimeNow( timeval* ) const;
    void CalculateTotalTimeInMilliseconds( );

    timeval mTimeStart;
    timeval mTimeSplit;
    timeval mTimeNow;
    timeval mTimeEnd;
    
    unsigned mTotalTimeInSeconds;
    unsigned mTotalTimeInMilliseconds;
    
    unsigned mStartTimeAndDate;
    unsigned mEndTimeAndDate;
    
    unsigned mTimeStep;
};

#endif

