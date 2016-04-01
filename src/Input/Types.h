#ifndef TYPES
#define TYPES

#include <vector>       // For std::vector
#include <sstream>      // For std::stringstream
#include <fstream>      // For std::ofstream
#include <sys/time.h>   // For struct timeval
#include <map>          // For map

class BasicDatum;
class Cohort;
class Convertor;
class DataCoords;
class DataIndices;
class DataLayer;
class DataLayerProcessor;
class DataLayerSet;
class DataRecorder;
class DateTime;
class Environment;
class FileReader;
class FileWriter;
class GridCell;
class GridDatum;
class InputData;
class InputDatum;
class Layer;
class Logger;
class MadingleyModelInitialisation;
class Maths;
class Parameters;
class Processor;
class Stock;
class Variable;

namespace Types {
    typedef BasicDatum* BasicDatumPointer;
    typedef Convertor* ConvertorPointer;
    typedef DataCoords* DataCoordsPointer;
    typedef DataIndices* DataIndicesPointer;
    typedef DataLayer* DataLayerPointer;
    typedef DataLayerProcessor* DataLayerProcessorPointer;
    typedef DataLayerSet* DataLayerSetPointer;
    typedef DataRecorder* DataRecorderPointer;
    typedef DateTime* DateTimePointer;
    typedef Environment* EnvironmentPointer;
    typedef FileReader* FileReaderPointer;
    typedef FileWriter* FileWriterPointer;
    typedef GridDatum* GridDatumPointer;
    typedef GridCell* GridCellPointer;
    typedef InputData* InputDataPointer;
    typedef InputDatum* InputDatumPointer;
    typedef Layer* LayerPointer;
    typedef Logger* LoggerPointer;
    typedef Maths* MathsPointer;
    typedef Parameters* ParametersPointer;
    typedef Processor* ProcessorPointer;
    typedef Variable* VariablePointer;
    
    typedef std::pair< float, float > GeoCoords;
    typedef std::pair< unsigned short, unsigned short > GeoIndices;
    
    typedef std::vector< Cohort > CohortVector;
    typedef std::vector< Stock > StockVector;
    typedef std::vector< InputDatumPointer > InputDatumVector;
    typedef std::vector< VariablePointer > VariableVector;
    
    typedef std::vector< CohortVector > Cohort2DVector;
    
    typedef std::vector< bool > BooleanVector;
    typedef std::vector< char > CharVector;
    typedef std::vector< unsigned char > UnsignedCharVector;
    typedef std::vector< double > DoubleVector;
    typedef std::vector< float > FloatVector;
    typedef std::vector< int > IntegerVector;
    typedef std::vector< long > LongVector;
    typedef std::vector< short > ShortVector;
    typedef std::vector< unsigned > UnsignedVector;
    typedef std::vector< unsigned short > UnsignedShortVector;
    typedef std::vector< std::string > StringVector;

    typedef std::vector< DoubleVector > DoubleMatrix;
    typedef std::vector< UnsignedVector > UnsignedMatrix;
    typedef std::vector< StringVector > StringMatrix;

    typedef std::vector< DoubleMatrix > Double3DMatrix;
    
    typedef std::vector< GeoCoords > GeoCoordsVector;
    typedef std::vector< GeoCoordsVector > GeoCoordsMatrix;
    
    typedef std::map< std::string, double > DoubleMap;
    typedef std::map< std::string, BasicDatumPointer > BasicDatumMap;
    typedef std::map< std::string, DataLayerPointer > DataLayerMap;
    typedef std::map< std::string, GridDatumPointer > GridDatumMap;
    typedef std::map< std::string, LayerPointer > LayerMap;
    
    typedef std::map< std::string, DoubleMap > Double2DMap;
    
    typedef std::map< int, StockVector > StocksMap;
}

#endif
