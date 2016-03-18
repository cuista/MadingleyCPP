#ifndef DATARECORDER
#define	DATARECORDER

#include "Types.h"

class DataRecorder {
public:
    ~DataRecorder( );
    static Types::DataRecorderPointer Get( );

    bool Initialise( const Types::StringMatrix& );
    void AddDataTo( const std::string&, const float& );
    void AddDataTo( const std::string&, const Types::GeoCoordPointer, const float& );
    void AddDataTo( const std::string&, const Types::IndicesPointer, const float& );
    
    Types::BasicDatumMap GetBasicDatumMap( ) const;
    Types::GridDatumMap GetGridDatumMap( ) const;
    
    Types::StringVector GetMetadataFilePaths( ) const;
    void AddMetadataFilePath( const std::string& );

private:
    DataRecorder( );
    static Types::DataRecorderPointer mThis;
    
    Types::BasicDatumPointer GetBasicDatum( const std::string& );
    Types::GridDatumPointer GetGridDatum( const std::string& );
    
    Types::BasicDatumMap mBasicDatumMap;
    Types::GridDatumMap mGridDatumMap;
    
    Types::StringMatrix mBasicOutputMetadata;
    Types::StringMatrix mGridOutputMetadata;
    
    Types::StringVector mMetadataFilePaths;
};

#endif

