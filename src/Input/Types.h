#ifndef TYPES
#define TYPES

#include <vector>       // For std::vector
#include <sstream>      // For std::stringstream
#include <fstream>      // For std::ofstream
#include <sys/time.h>   // For struct timeval

class Logger;
class FileReader;
class FileWriter;
class Convertor;
class DataGrid;
class NcGridCell;
class MainTable;

namespace Types {
    typedef Logger* LoggerPointer;

    typedef FileReader* FileReaderPointer;
    typedef FileWriter* FileWriterPointer;
    typedef Convertor* ConvertorPointer;
    
    typedef DataGrid* DataGridPointer;
    typedef NcGridCell* GridCellPointer;
    
    typedef MainTable* MainTablePointer;
    
    typedef std::vector< GridCellPointer > GridCellVector;
    typedef std::vector< GridCellVector > GridCellVectorOfVectors;

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

    typedef std::string String;
    typedef std::ifstream InputFileStream;
    typedef std::ofstream OutputFileStream;
    typedef std::stringstream StringStream;
}

#endif
