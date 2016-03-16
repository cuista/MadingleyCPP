#ifndef INPUTDATUM
#define	INPUTDATUM

#include "Types.h"

class InputDatum {
public:
    InputDatum( const std::string&, const std::string&, const Types::UnsignedIntegerVector&, const unsigned int&, float*, const bool );
    ~InputDatum( );
    
    std::string GetName( ) const;
    Types::VariablePointer GetVariable( const unsigned int& ) const;
    unsigned int GetNumberOfVariables( ) const;

    void AddVariable( const std::string&, const Types::UnsignedIntegerVector&, const unsigned int&, float*, const bool );

private:
    Types::VariableVector mVariableVector;
    std::string mName;
};

#endif

