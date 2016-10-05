#ifndef DATE
#define	DATE

#include <sys/time.h>   // For struct timeval
#include "Types.h"
#include "Constants.h"

class Date {
public:
    Date( );
    ~Date( );
    
    std::string GetDateAndTimeString( const std::string format = Constants::cDataSetNameFormat ) const;
};

#endif

