#include "Convertor.h"
#include "Logger.h"

Types::ConvertorPointer Convertor::mThis = 0;

Types::ConvertorPointer Convertor::Get( ) {
    if( mThis == 0 ) {
        mThis = new Convertor( );
    }
    return mThis;
}

Convertor::~Convertor( ) {
    if( mThis != 0 ) {
        delete mThis;
    }
}

Convertor::Convertor( ) {

}

const std::string Convertor::NumberToString( const double& number ) const {
    Types::StringStream stringStream;
    stringStream << number;

    return stringStream.str( );
}

double Convertor::StringToNumber( std::string& string ) const {

    double number = strtod( string.c_str( ), NULL );

    return number;
}

const Types::StringVector Convertor::StringToWords( const std::string& inputString, const char wordTerminationCharacter ) {

    Types::StringStream stringStream( inputString );

    std::string word = "";
    Types::StringVector wordList;

    while( std::getline( stringStream, word, wordTerminationCharacter ) ) {
        wordList.push_back( word );
    }

    return wordList;
}

const std::string Convertor::DoubleToPrecisionString( double& value, unsigned int& decimals ) {

    std::ostringstream outputStringStream;
    outputStringStream << std::fixed << std::setprecision( decimals ) << value;

    return outputStringStream.str( );
}
