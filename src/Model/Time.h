#ifndef TIME
#define	TIME

#include "Types.h"

class Time {
public:
    ~Time( );
    static Types::TimePointer Get( );
    
    unsigned GetTimeStep( const std::string& );
    void SetMonthlyTimeStep( const unsigned& );
    void SetAnnualTimeStep( const unsigned& );
    
private:
    Time( );
    
    static Types::TimePointer mThis;
    
    unsigned mMonthlyTimeStep;
    unsigned mAnnualTimeStep;
};

#endif

