#include "Logger.h"

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