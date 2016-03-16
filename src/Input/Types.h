#ifndef TYPES
#define TYPES

#include <vector>       // For std::vector
#include <sstream>      // For std::stringstream
#include <fstream>      // For std::ofstream
#include <sys/time.h>   // For struct timeval
#include <map>          // For map

class BasicDatum;
class Convertor;
class DataLayer;
class DataLayerProcessor;
class DataLayerSet;
class DataRecorder;
class DateTime;
class FileReader;
class FileWriter;
class GeoCoord;
class GridDatum;
class Indices;
class InputData;
class InputDatum;
class Logger;
class Maths;
class Parameters;
class Processor;
class Variable;

namespace Types {
    typedef BasicDatum* BasicDatumPointer;
    typedef Convertor* ConvertorPointer;
    typedef DataLayer* DataLayerPointer;
    typedef DataLayerProcessor* DataLayerProcessorPointer;
    typedef DataLayerSet* DataLayerSetPointer;
    typedef DataRecorder* DataRecorderPointer;
    typedef DateTime* DateTimePointer;
    typedef FileReader* FileReaderPointer;
    typedef FileWriter* FileWriterPointer;
    typedef GeoCoord* GeoCoordPointer;
    typedef GridDatum* GridDatumPointer;
    typedef Indices* IndicesPointer;
    typedef InputData* InputDataPointer;
    typedef InputDatum* InputDatumPointer;
    typedef Logger* LoggerPointer;
    typedef Maths* MathsPointer;
    typedef Parameters* ParametersPointer;
    typedef Processor* ProcessorPointer;
    typedef Variable* VariablePointer;
    
    typedef std::map< std::string, BasicDatumPointer > BasicDatumMap;
    typedef std::map< std::string, DataLayerPointer > DataLayerMap;
    typedef std::map< std::string, GridDatumPointer > GridDatumMap;
    
    typedef std::vector< InputDatumPointer > InputDatumVector;
    typedef std::vector< VariablePointer > VariableVector;
    
    typedef std::vector< bool > BooleanVector;
    typedef std::vector< char > CharVector;
    typedef std::vector< unsigned char > UnsignedCharVector;
    typedef std::vector< short > ShortVector;
    typedef std::vector< unsigned short > UnsignedShortVector;
    typedef std::vector< int > IntegerVector;
    typedef std::vector< unsigned int > UnsignedIntegerVector;
    typedef std::vector< float > FloatVector;
    typedef std::vector< double > DoubleVector;

    typedef std::vector< std::string > StringVector;
    typedef std::vector< StringVector > StringMatrix;
}

#endif
