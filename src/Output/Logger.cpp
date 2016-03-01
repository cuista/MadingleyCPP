#include "Logger.h"
#include "FileWriter.h"

#include <iostream>

Types::LoggerPointer Logger::mThis = 0;

Types::LoggerPointer Logger::Get( ) {
    if( mThis == 0 ) {
        mThis = new Logger( );
    }
    return mThis;
}

Logger::~Logger( ) {

    if( mThis != 0 ) {
        delete mThis;
    }
}

Logger::Logger( ) {

}

void Logger::LogStringNoReturn( std::string message ) {
    std::cout << message;
}

void Logger::LogString( std::string message ) {

    message.append( "\n" );
    LogStringNoReturn( message );
}