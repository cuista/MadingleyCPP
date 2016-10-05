#include "Time.h"
#include "Maths.h"

Types::TimePointer Time::mThis = NULL;

Time::Time( ) {
    mMonthlyTimeStep = 0;
    mAnnualTimeStep = 0;
}

Time::~Time( ) {

}

Types::TimePointer Time::Get( ) {
    if( mThis == NULL ) {
        mThis = new Time( );
    }
    return mThis;
}

unsigned Time::GetTimeStep( const std::string& timeUnit ) {
    if( timeUnit == Constants::cMonthlyTimeUnitName )
        return mMonthlyTimeStep;
    else
        return mAnnualTimeStep;
}

void Time::SetMonthlyTimeStep( const unsigned& monthlyTimeStep ) {
    mMonthlyTimeStep = monthlyTimeStep;

    if( monthlyTimeStep != 0 && Maths::Get( )->Mod( mMonthlyTimeStep, 12 ) == 0 )
        mAnnualTimeStep += 1;
}

void Time::SetAnnualTimeStep( const unsigned& annualTimeStep ) {
    mAnnualTimeStep = annualTimeStep;
    mMonthlyTimeStep = mAnnualTimeStep * 12;
}