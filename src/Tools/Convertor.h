#ifndef CONVERTOR
#define	CONVERTOR

#include "Types.h"

#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <iomanip>

class Convertor {
public:
    ~Convertor( );
    static Types::ConvertorPointer Get( );

    const std::string NumberToString( const double& ) const;
    double StringToNumber( std::string& ) const;

    const Types::StringVector StringToWords( const std::string&, const char );
    const std::string DoubleToPrecisionString( double&, unsigned int& );

private:
    Convertor( );

    static Types::ConvertorPointer mThis;
};

#endif

