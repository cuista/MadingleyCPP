#ifndef DATAGRID
#define	DATAGRID

#include "Types.h"
#include "NcGridCell.h"

class DataGrid {
public:

    ~DataGrid( );

    static Types::DataGridPointer Get( );

    void AddDataWithTime( const double*, const unsigned int );
    void AddDataWithoutTime( const double* );

    void InitialiseGridCells( const int*, const double*, const double*, const unsigned int, const unsigned int );
    void InitialiseGridCellVectors( const unsigned int*, const unsigned int );

    unsigned int GetLengthLongitudeVector( ) const;
    unsigned int GetLengthLatitudeVector( ) const;
    unsigned int GetLengthTimeVector( ) const;

    Types::FloatVector GetLongitudeVector( ) const;
    Types::FloatVector GetLatitudeVector( ) const;

    Types::GridCellPointer GetGridCell( const unsigned int, const unsigned int ) const;

private:

    DataGrid( );

    static Types::DataGridPointer mThis;

    Types::GridCellVectorOfVectors mGridCells;
    Types::UnsignedIntegerVector mTimeVector;
    Types::FloatVector mLongitudeVector;
    Types::FloatVector mLatitudeVector;
};

#endif

