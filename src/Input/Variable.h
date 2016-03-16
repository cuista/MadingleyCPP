#ifndef VARIABLE
#define	VARIABLE

#include "Types.h"
#include "Constants.h"

class Variable {
public:
    Variable( const std::string&, const Types::UnsignedIntegerVector&, const unsigned int&, float*, const bool );
    ~Variable( );
    
    std::string GetName( ) const;
    Types::UnsignedIntegerVector GetDimensions( ) const;
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
    Types::UnsignedIntegerVector mDimensions;
    unsigned int mSize;
    float* mData;
    float mMaximumDataDifference;
    bool mIsDefault;
};

#endif

