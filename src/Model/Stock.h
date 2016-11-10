#ifndef STOCK_H
#define STOCK_H
/** \file Stock.h
 * \brief the Stock header file
 */
#include <map>
#include <vector>
#include <FunctionalGroupDefinitions.h>
class GridCell;

/** \brief  Individual stocks */
class Stock {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------

    /** \brief The index of the functional group that the stock belongs to */
    unsigned FunctionalGroupIndex;

    /** \brief The mean body mass of an individual in this stock */

    double IndividualBodyMass;

    /** \brief The total biomass of the stock */
    double TotalBiomass;

    GridCell* mCell;
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    /** \brief Constructor for stock class. Assigns stock starting properties
    @param StockDefinitions Definitions of Stock Properties
    @param Functional Group The functional group index of the stock being generated 
    @param Environment The cell environment 
    @param Success Whether the stock should be present in this cell */
    Stock( FunctionalGroupDefinitions&, const unsigned, GridCell&, bool& success );


};
#endif
