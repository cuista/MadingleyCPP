#ifndef VARIABLE
#define	VARIABLE

#include "Types.h"
#include "Constants.h"

class Variable {
public:
    Variable( const std::string&, const Types::UnsignedIntVector&, const unsigned int&, float*, const bool );
    ~Variable( );
    
    std::string GetName( ) const;
    Types::UnsignedIntVector GetDimensions( ) const;
    unsigned int GetDimension( const unsigned int& ) const;
    unsigned int GetSize( ) const;
    
    float* GetData( ) const;
    float GetDataAtIndex( const int& ) const;
    float GetMaximumDataDifference( ) const;
    
    bool IsDefault( ) const;
    
    void SetDataAtIndex( const int&, const float& );
    void SetMaximumDataDifference( const float& );
    
protected:
    std::string mName;
    Types::UnsignedIntVector mDimensions;
    unsigned int mSize;
    float* mData;
    float mMaximumDataDifference;
    bool mIsDefault;
};

#endif

