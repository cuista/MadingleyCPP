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
    void ClearMetadata( );
    bool ReadMetadataFile( const std::string& );
    bool ReadNetCDFFiles( );
    
    Types::StringMatrix mMetadata;
    std::string mFilePath;
};

#endif

