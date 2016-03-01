#ifndef FILEWRITER
#define	FILEWRITER

#include "Types.h"

class FileWriter {
public:
    ~FileWriter( );
    FileWriter( );
    
    void WriteNetCDFFile( const std::string& );

private:
};

#endif

