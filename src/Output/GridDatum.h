#ifndef GRIDDATUM
#define	GRIDDATUM

#include "Types.h"

class GridDatum {
public:
    GridDatum( const std::string&, const std::string& );
    ~GridDatum( );
    
    std::string GetName( ) const;
    std::string GetUnits( ) const;
    float* GetData( ) const;
    
    void AddData( const Types::GeoCoordPointer, const float& );
    void AddData( const Types::IndicesPointer, const float& );
    
private:
    std::string mName;
    std::string mUnits;
    float* mData;
};

#endif

