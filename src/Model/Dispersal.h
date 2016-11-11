#ifndef DISPERSAL
#define DISPERSAL

#include "GridCell.h"
#include "IDispersalImplementation.h"
#include "TAdvectiveDispersal.h"
#include "TResponsiveDispersal.h"
#include "TDiffusiveDispersal.h"
#include "Parameters.h"
#include "Types.h"

/** \brief Performs dispersal */
class Dispersal {
public:
    Dispersal( );
    ~Dispersal( );

    void ResetRandoms( );

    /** \brief Run dispersal 
    @param cellIndex The cell index for the active cell in the model grid 
    @param gridForDispersal The model grid to run the process for 
    @param dispersalOnly Whether we are running dispersal only 
    @param madingleyCohortDefinitions The functional group definitions for cohorts in the model 
    @param madingleyStockDefinitions The functional group definitions for stocks in the model 
    @param currentMonth The current model month */
    void RunCrossGridCellEcologicalProcess( GridCell&, ModelGrid&, MadingleyModelInitialisation&, unsigned );

    void UpdateCrossGridCellEcology( unsigned& dispersalCounter );

    Types::CohortVector mDispersers;
    /** \brief The available implementations of the dispersal process */
    Types::IDispersalMap mChoose;
};

#endif
