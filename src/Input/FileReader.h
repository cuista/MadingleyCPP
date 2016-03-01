#ifndef FILEREADER
#define	FILEREADER

#include "Types.h"
#include "Constants.h"

class FileReader {
public:

    FileReader( );
    ~FileReader( );
    
    void ReadNetCDFFile( const std::string&, const unsigned int );

private:

    std::string mFilePath;
};

#endif

