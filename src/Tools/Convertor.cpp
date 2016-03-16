#include "Convertor.h"

#include <algorithm>

Types::ConvertorPointer Convertor::mThis = NULL;

Types::ConvertorPointer Convertor::Get( ) {
    if( mThis == NULL ) {
        mThis = new Convertor( );
    }
    return mThis;
}

Convertor::~Convertor( ) {
    if( mThis != NULL ) {
        delete mThis;
    }
}

Convertor::Convertor( ) {

}

double Convertor::StringToNumber( const std::string& string ) const {

    double number = strtod( string.c_str( ), NULL );

    return number;
}

const Types::StringVector Convertor::StringToWords( const std::string& inputString, const char wordTerminationCharacter ) const {

    std::stringstream stringStream( inputString );

    std::string word = "";
    Types::StringVector wordList;

    while( std::getline( stringStream, word, wordTerminationCharacter ) ) {
        wordList.push_back( word );
    }

    return wordList;
}

const std::string Convertor::DoubleToPrecisionString( const double& value, const unsigned int& decimals ) const {

    std::ostringstream outputStringStream;
    outputStringStream << std::fixed << std::setprecision( decimals ) << value;

    return outputStringStream.str( );
}

std::string Convertor::ToLowercase( const std::string instring ) const {
    std::string outstring;
    std::transform( instring.begin( ), instring.end( ), std::back_inserter( outstring ), tolower );

    return outstring;
}