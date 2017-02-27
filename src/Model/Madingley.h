#ifndef MADINGLEY
#define	MADINGLEY

#include "MadingleyInitialisation.h"
#include "FunctionalGroupDefinitions.h"
#include "Stopwatch.h"
#include "CohortMerger.h"
#include "Grid.h"
#include "GridCell.h"
#include "DispersalSet.h"
#include "EcologyStock.h"
#include "EcologyCohort.h"
#include "Activity.h"
#include "ThreadVariables.h"
#include "Environment.h"
#include "TimeStep.h"
#include "Maths.h"
#include "Parameters.h"
#include "DataRecorder.h"
#include "Types.h"

/** \brief The ecosystem model */
class Madingley {
public:
    /** \brief   Initializes the ecosystem model
    @param initialisationFileName The name of the file with model parameters
    @param OutputPath Where the output will be stored */
    Madingley( );
    /** \brief  Run the global ecosystem model */
    void Run( );

private:
    /** \brief  Run processes for cells*/
    void RunWithinCells( );
    /** \brief   Run ecological processes for stocks in a specified grid cell
    @param gcl The current cell */
    void RunWithinCellStockEcology( GridCell& );
    /** \brief   Run ecological processes for cohorts in a specified grid cell
    @param gcl Reference to the current grid cell
    @param partial Track some global variables pertaining to cohort numbers etc. */
    void RunWithinCellCohortEcology( GridCell&, ThreadVariables& );
    /** \brief Carries out extinction on cohorts that have an abundance below a defined extinction threshold */
    void RunExtinction( GridCell&, ThreadVariables& );
    /** \brief Run ecological processes that operate across grid cells */
    void RunCrossGridCellEcology( unsigned& );
    void SetUpGlobalDiagnosticsList( );
    void Output( unsigned );

    /** \brief An instance of ModelGrid to hold the grid to be used in this model */
    Grid mModelGrid;
    MadingleyInitialisation mParams;
    /** \brief An instance of the merging class */
    CohortMerger mCohortMerger;
    /** \brief Instance of the class to perform general functions */
    UtilityFunctions mUtilities;
    /** \brief An instance of StopWatch to time individual time steps */
    Stopwatch mEcologyTimer;
    Stopwatch mDispersalTimer;
    Stopwatch mOutputTimer;
    Stopwatch mMergeTimer;
    Types::DispersalSetPointer mDispersalSet; //FIX - Does this need to be a pointer?
    /** \brief A list of global diagnostics for this model run */
    Types::DoubleMap mGlobalDiagnosticVariables;
    Types::StringVector mStockLeafStrategy;
    Types::StringVector mCohortNutritionSource;
    Types::StringVector mCohortThermoregulation;
    Types::StringVector mCohortReproductiveStrategy;
    /** A variable to increment for the purposes of giving each cohort a unique ID */
    long long mNextCohortID;
    /** \brief The current time step */
    unsigned mCurrentTimeStep;
    /** \brief The current month: 1=Jan; 2=Feb; 3=Mar etc. */
    unsigned mCurrentMonth;
    /** \brief Variable to track the number of cohorts that have dispersed. Doesn't need to be thread-local because all threads have converged prior to running cross-grid-cell processes */
    unsigned mDispersals;
};

#endif

