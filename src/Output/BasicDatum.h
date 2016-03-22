#ifndef BASICDATUM
#define	BASICDATUM

#include "Types.h"

class BasicDatum {
public:
    BasicDatum( const std::string&, const std::string& );
    ~BasicDatum( );
    
    std::string GetName( ) const;
    std::string GetUnits( ) const;
    float* GetData( ) const;
    
    void AddData( const float& );
    
private:
    std::string mName;
    std::string mUnits;
    float* mData;
};

#endif

