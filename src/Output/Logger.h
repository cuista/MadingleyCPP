#ifndef LOGGER
#define LOGGER

#include "Types.h"

class Logger {
public:
    ~Logger( );
    static Types::LoggerPointer Get( );
    void LogStringNoReturn( std::string );
    void LogString( std::string );
    
private:
    Logger( );
    
    static Types::LoggerPointer mThis;
};

#endif
