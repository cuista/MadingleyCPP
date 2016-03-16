#include "Logger.h"
#include "DateTime.h"

Types::LoggerPointer Logger::mThis = NULL;

Types::LoggerPointer Logger::Get( ) {
    if( mThis == NULL ) {
        mThis = new Logger( );
    }
    return mThis;
}

Logger::~Logger( ) {
    if( mThis != NULL ) {
        delete mThis;
    }
}

Logger::Logger( ) {

}

void Logger::Click( const std::string& source ) const {
    LogMessage( source + DateTime::Get( )->ProduceSplitTimeString( ) );
}

void Logger::OutputRunTime( ) const {
    LogMessage( DateTime::Get( )->ProduceTotalTimeString( ) );
}