#ifndef IDISPERSALIMPLEMENTATION_H
#define IDISPERSALIMPLEMENTATION_H
#include <ModelGrid.h>
#include <Cohort.h>
#include <random>
#include <NonStaticSimpleRNG.h>

/** \file IDispersalImplementation.h
 * \brief the IDispersalImplementation header file
 */

/** \brief Base Class for implementations of the ecological process of dispersal */
class IDispersalImplementation {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief Include Utility class */
    UtilityFunctions Utilities;
    /** \brief An instance of the simple random number generator class */
    std::mt19937_64 RandomNumberGenerator;
    NonStaticSimpleRNG randomNumber,randomNum2;
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------
    IDispersalImplementation(){;}
    /** \brief Run the dispersal implementation */
    virtual void RunDispersal( ModelGrid& gridForDispersal, Cohort& cohortToDisperse, const unsigned& currentMonth ) {
        cout << "Called virtual dispersal runner: probably not what you want" << endl;
    }
    void ResetRandom(){randomNumber.reset();randomNumber.SetSeed(14141);randomNum2.reset();}
    //----------------------------------------------------------------------------------------------

    GridCell* newCell( ModelGrid& madingleyGrid, double& uSpeed, double& vSpeed, double & LonCellLength, double & LatCellLength, const GridCell* gcl) {
        // Calculate the area of the grid cell that is now outside in the diagonal direction
        double AreaOutsideBoth = abs( uSpeed * vSpeed );

        // Calculate the area of the grid cell that is now outside in the u direction (not including the diagonal)
        double AreaOutsideU = abs( uSpeed * LatCellLength ) - AreaOutsideBoth;

        // Calculate the proportion of the grid cell that is outside in the v direction (not including the diagonal
        double AreaOutsideV = abs( vSpeed * LonCellLength ) - AreaOutsideBoth;

        // Get the cell area, in kilometres squared
        double CellArea = gcl->GetCellArea( );

        // Convert areas to a probability
        double DispersalProbability = ( AreaOutsideU + AreaOutsideV + AreaOutsideBoth ) / CellArea;
        // Check that we don't have any issues
        if( DispersalProbability > 1 )cout << "Bad Dispersal Probability " << DispersalProbability << endl;
        //assert(DispersalProbability <= 1 && "Dispersal probability should always be <= 1");
        GridCell* DestinationCell = 0;
        // Check to see in which axis the cohort disperses

        // Note that the values in the dispersal array are the proportional area moved outside the grid cell in each direction; we simply compare the random draw to this
        // to determine the direction in which the cohort moves probabilistically
        //std::uniform_real_distribution<double> randomNumber( 0.0, 1.0 );
        //double RandomValue = randomNumber( RandomNumberGenerator );
        double RandomValue=randomNum2.GetUniform();
        
        if( DispersalProbability >= RandomValue ) {
            int signu = ( uSpeed > 0 ) - ( uSpeed < 0 );
            int signv = ( vSpeed > 0 ) - ( vSpeed < 0 );
            // Longitudinally
            if( RandomValue <= AreaOutsideU / CellArea ) {
                signv = 0;
            } else {
                //Latitudinally
                if( RandomValue <= ( AreaOutsideU / CellArea + AreaOutsideV / CellArea ) ) {
                    signu = 0;
                }
            }
            // try to get a cell.
            GridCell* FreshCell = madingleyGrid.getNewCell( gcl, signv, signu );
            if( FreshCell != 0 ) DestinationCell = FreshCell;
        }

        return DestinationCell;
    }
    //----------------------------------------------------------------------------------------------
};
#endif
