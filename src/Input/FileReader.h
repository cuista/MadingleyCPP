#ifndef FILEREADER
#define	FILEREADER

#include "Types.h"
#include "Constants.h"

class FileReader {
public:
    FileReader( );
    ~FileReader( );

    bool ReadFiles( );

private:
    bool ReadMetadataFile( const std::string& );
    
    Types::StringMatrix ReadAndReturnTextFile( const std::string& );
    bool ReadInputDataFiles( const Types::StringMatrix& );
    
    std::string mFilePath;
};

#endif

