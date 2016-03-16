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
    mStartTimeAndDate = static_cast < unsigned int >( mTimeStart.tv_sec );
    
    mTimeSplit = mTimeStart;
}

void DateTime::SplitTiming( ) {
    SetTimeNow( &mTimeNow );
}

void DateTime::EndTiming( ) {
    SetTimeNow( &mTimeEnd );
    mEndTimeAndDate = static_cast < unsigned int >( mTimeEnd.tv_sec );
    mTotalTimeInSeconds = mEndTimeAndDate - mStartTimeAndDate;
    
    CalculateTotalTimeInMilliseconds( );
}

void DateTime::CalculateTotalTimeInMilliseconds( ) {

    unsigned int timeStart = static_cast < unsigned int >( mTimeStart.tv_usec );
    unsigned int timeEnd = static_cast < unsigned int >( mTimeEnd.tv_usec );

    mTotalTimeInMilliseconds = ( timeEnd - timeStart ) / 1000;
}

std::string DateTime::ProduceCumulativeTimeString( ) {
    
    SplitTiming( );

    unsigned int timeDurationSeconds = GetCumulativeTimeInSeconds( );
    unsigned int timeDurationMicroseconds = GetCumulativeTimeInMicroseconds( );

    std::stringstream splitTimeStream;

    splitTimeStream << timeDurationSeconds << "s " << timeDurationMicroseconds << "us";

    mTimeSplit = mTimeNow;

    return splitTimeStream.str( );
}

std::string DateTime::ProduceSplitTimeString( ) {

    SplitTiming( );

    unsigned int timeSplitMicroseconds = static_cast < unsigned int >( mTimeSplit.tv_usec );
    unsigned int timeNowMicroseconds = static_cast < unsigned int >( mTimeNow.tv_usec );

    unsigned int timeDurationMicroseconds = timeNowMicroseconds - timeSplitMicroseconds;

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

unsigned int DateTime::GetCumulativeTimeInSeconds( ) {

    unsigned int timeStart = static_cast < unsigned int >( mTimeStart.tv_sec );
    unsigned int cumulativeTime = static_cast < unsigned int >( mTimeNow.tv_sec );

    unsigned int cumulativeTimeInSeconds = cumulativeTime - timeStart;

    return cumulativeTimeInSeconds;
}

unsigned int DateTime::GetCumulativeTimeInMicroseconds( ) {

    unsigned int timeStart = static_cast < unsigned int >( mTimeStart.tv_usec );
    unsigned int cumulativeTime = static_cast < unsigned int >( mTimeNow.tv_usec );

    unsigned int cumulativeTimeInMicroseconds = cumulativeTime - timeStart;

    return cumulativeTimeInMicroseconds;
}

unsigned int DateTime::GetTotalTimeInSeconds( ) const {
    return mTotalTimeInSeconds;
}

unsigned int DateTime::GetTotalTimeInMilliseconds( ) const {
    return mTotalTimeInMilliseconds;
}

unsigned int DateTime::GetRandomNumberSeed( ) const {
    unsigned int seed = static_cast < unsigned int >( mTimeStart.tv_usec );

    return seed;
}

unsigned int DateTime::GetTimeStep( ) const {
    return mTimeStep;
}

void DateTime::SetTimeStep( const unsigned int& timeIndex ) {
    mTimeStep = timeIndex;
}