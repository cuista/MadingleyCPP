#include "DateTime.h"

#include "Parameters.h"
#include "Constants.h"
#include "Convertor.h"

Types::DateTimePointer DateTime::mThis = 0;

Types::DateTimePointer DateTime::Get( ) {
    if( mThis == 0 ) {
        mThis = new DateTime( );
    }
    return mThis;
}

DateTime::~DateTime( ) {
    if( mThis != 0 ) {
        delete mThis;
    }
}

DateTime::DateTime( ) {
}

void DateTime::StartTiming( ) {
    SetTimeNow( &mTimeStart );
    mStartTimeAndDate = static_cast < unsigned >( mTimeStart.tv_sec );
    
    mTimeSplit = mTimeStart;
}

void DateTime::SplitTiming( ) {
    SetTimeNow( &mTimeNow );
}

void DateTime::EndTiming( ) {
    SetTimeNow( &mTimeEnd );
    mEndTimeAndDate = static_cast < unsigned >( mTimeEnd.tv_sec );
    mTotalTimeInSeconds = mEndTimeAndDate - mStartTimeAndDate;
    
    CalculateTotalTimeInMilliseconds( );
}

void DateTime::CalculateTotalTimeInMilliseconds( ) {

    unsigned timeStart = static_cast < unsigned >( mTimeStart.tv_usec );
    unsigned timeEnd = static_cast < unsigned >( mTimeEnd.tv_usec );

    mTotalTimeInMilliseconds = ( timeEnd - timeStart ) / 1000;
}

std::string DateTime::ProduceCumulativeTimeString( ) {
    
    SplitTiming( );

    unsigned timeDurationSeconds = GetCumulativeTimeInSeconds( );
    unsigned timeDurationMicroseconds = GetCumulativeTimeInMicroseconds( );

    std::stringstream splitTimeStream;

    splitTimeStream << timeDurationSeconds << "s " << timeDurationMicroseconds << "us";

    mTimeSplit = mTimeNow;

    return splitTimeStream.str( );
}

std::string DateTime::ProduceSplitTimeString( ) {

    SplitTiming( );

    unsigned timeSplitMicroseconds = static_cast < unsigned >( mTimeSplit.tv_usec );
    unsigned timeNowMicroseconds = static_cast < unsigned >( mTimeNow.tv_usec );

    unsigned timeDurationMicroseconds = timeNowMicroseconds - timeSplitMicroseconds;

    std::stringstream splitTimeStream;

    splitTimeStream << timeDurationMicroseconds << "us";

    mTimeSplit = mTimeNow;

    return splitTimeStream.str( );
}

std::string DateTime::ProduceTotalTimeString( ) const {
    
    std::stringstream totalTimeStream;
    totalTimeStream << "Total run time = " << Convertor::Get( )->ToString( mTotalTimeInSeconds ) << "s, ";

    return totalTimeStream.str( );
}

std::string DateTime::GetFormattedDateAndTime( const std::string format, const bool isEnd ) const {

    char dateTimeChar[ Constants::cDateTimeBufferSize ];

    time_t rawtime;
    if( isEnd == true ) {
        rawtime = ( time_t )mEndTimeAndDate;
    } else {
        rawtime = ( time_t )mStartTimeAndDate;
    }

    struct tm *timeinfo;

    timeinfo = localtime( &rawtime );

    strftime( dateTimeChar, Constants::cDateTimeBufferSize, format.c_str( ), timeinfo );
    std::string dateTime( dateTimeChar );

    return dateTime;
}

void DateTime::SetTimeNow( timeval* time ) const {
    gettimeofday( time, NULL );
}

unsigned DateTime::GetCumulativeTimeInSeconds( ) {

    unsigned timeStart = static_cast < unsigned >( mTimeStart.tv_sec );
    unsigned cumulativeTime = static_cast < unsigned >( mTimeNow.tv_sec );

    unsigned cumulativeTimeInSeconds = cumulativeTime - timeStart;

    return cumulativeTimeInSeconds;
}

unsigned DateTime::GetCumulativeTimeInMicroseconds( ) {

    unsigned timeStart = static_cast < unsigned >( mTimeStart.tv_usec );
    unsigned cumulativeTime = static_cast < unsigned >( mTimeNow.tv_usec );

    unsigned cumulativeTimeInMicroseconds = cumulativeTime - timeStart;

    return cumulativeTimeInMicroseconds;
}

unsigned DateTime::GetTotalTimeInSeconds( ) const {
    return mTotalTimeInSeconds;
}

unsigned DateTime::GetTotalTimeInMilliseconds( ) const {
    return mTotalTimeInMilliseconds;
}

unsigned DateTime::GetRandomNumberSeed( ) const {
    unsigned seed = static_cast < unsigned >( mTimeStart.tv_usec );

    return seed;
}

unsigned DateTime::GetTimeStep( ) const {
    return mTimeStep;
}

void DateTime::SetTimeStep( const unsigned& timeIndex ) {
    mTimeStep = timeIndex;
}