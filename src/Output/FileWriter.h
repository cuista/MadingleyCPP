#ifndef FILEWRITER
#define	FILEWRITER

#include "Types.h"

class FileWriter {
public:
    FileWriter( );
    ~FileWriter( );
    
    bool WriteFiles( );
    
private:
    void InitialiseOutputDirectory( );
    void WriteMetadataFiles( );
    bool WriteNetCDFFiles( ) const;
    bool WriteBasicOutputs( ) const;
    bool WriteGridOutputs( ) const;
    
    std::string mOutputDirectory;
};

#endif

