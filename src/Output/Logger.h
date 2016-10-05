#ifndef LOGGER
#define LOGGER

#include "Types.h"

#include <iostream>

class Logger {
public:
    ~Logger( );
    static Types::LoggerPointer Get( );

    template< class T >
    void LogMessageNoReturn( const T& message ) const {
        std::cout << message;
    }

    template< class T >
    void LogMessage( const T& message ) const {
        std::cout << message << std::endl;
    }

private:
    Logger( );
    static Types::LoggerPointer mThis;
};

#endif
