/* 
 * File:   Environment.cc
 * Author: mb425
 *
 * Created on 26 November 2015, 13:32
 */
#include <Environment.h>
#include <GridCell.h>
#include <ClimateVariablesCalculator.h>

#include "Types.h"
#include "FileReader.h"
#include "Logger.h"
#include "Parameters.h"
#include "DataLayerSet.h"
#include "DataIndices.h"
#include "Time.h"

Types::EnvironmentPointer Environment::mThis = NULL;
Types::LayerMap Environment::mLayers;
//------------------------------------------------------------------------------

Environment::Environment( ) {
    addLayer( "Realm" );
    setRealm( );

    addLayer( "TerrestrialHANPP" );
    setHANPP( );

    addLayerT( "uVel" );
    setUVel( );
    addLayerT( "vVel" );
    setVVel( );

    addLayerT( "Temperature" );
    setTemperature( );
    addLayerT( "DiurnalTemperatureRange" );

    setDiurnalTemperatureRange( );

    addLayerT( "Precipitation" );

    addLayer( "TotalPrecip" );
    setPrecipitation( );

    addLayerT( "NPP" );
    setNPP( );

    addLayerT( "Seasonality" );
    setNPPSeasonality( );
    addLayerT( "Breeding Season" );
    setBreeding( );


    addLayer( "Organic Pool" );
    setOrganicPool( );

    addLayer( "Respiratory CO2 Pool" );
    setRespiratoryCO2Pool( );

    addLayer( "AnnualTemperature" );
    addLayer( "SDTemperature" );
    addLayerT( "ExpTDevWeight" );
    setAVGSDTemp( );

    addLayerT( "AET" );
    addLayer( "TotalAET" );
    addLayerT( "Fraction Month Frost" );
    addLayer( "Fraction Year Frost" );
    addLayer( "Fraction Year Fire" );
    setFrostandFire( );

    //make sure all time dependent fields set to the start
    update( 0 );
}
//------------------------------------------------------------------------------

void Environment::update( int currentMonth ) {
    for( auto& L: mLayers )L.second->setTime( currentMonth );
}
//------------------------------------------------------------------------------

void Environment::addLayer( string s ) {
    mLayers[s] = new layer0( Parameters::Get( )->GetNumberOfGridCells( ) );
}
//------------------------------------------------------------------------------

void Environment::addLayerT( string s ) {
    mLayers[s] = new layerT( 12, Parameters::Get( )->GetNumberOfGridCells( ) );
}
//------------------------------------------------------------------------------

Environment* Environment::Get( ) {
    if( mThis == NULL ) {
        mThis = new Environment( );
    }
    return mThis;
}
//------------------------------------------------------------------------------

double& Environment::Get( string s, int cellIndex ) {
    if( mThis == NULL ) mThis = new Environment( );
    if( mLayers.count( s ) == 0 ) {
        cout << "Invalid Layer Request in Environment:: " << s << endl;
        exit( 0 );
    }
    return (*mLayers[s] )[cellIndex];
}
//------------------------------------------------------------------------------

double& Environment::Get( string s, GridCell& gcl ) {
    if( mThis == 0 )mThis = new Environment( );
    if( mLayers.count( s ) == 0 ) {
        cout << "Invalid Layer Request in Environment:: " << s << endl;
        exit( 0 );
    }
    return (*mLayers[s] )[ gcl.GetIndex( ) ];
}
//------------------------------------------------------------------------------

void Environment::setTemperature( ) {
    for( int tm = 0; tm < 12; tm++ ) {
        Time::Get( )->SetMonthlyTimeStep( tm );
        mLayers["Temperature"]->setTime( tm );

        for( unsigned cellIndex = 0; cellIndex < Parameters::Get( )->GetNumberOfGridCells( ); cellIndex++ ) {
            double d = Constants::cMissingValue;
            if( DataLayerSet::Get( )->GetDataAtCellIndexFor( "Realm", cellIndex ) == 1 ) {
                d = DataLayerSet::Get( )->GetDataAtCellIndexFor( "MarineTemp", cellIndex );
            } else if( DataLayerSet::Get( )->GetDataAtCellIndexFor( "Realm", cellIndex ) == 2 ) {
                d = DataLayerSet::Get( )->GetDataAtCellIndexFor( "TerrestrialTemp", cellIndex );
            }

            if( d == Constants::cMissingValue ) {
                d = 0;
                cout << "Warning Environment::setTemperature- missing values in temperature field!!" << endl;
            }
            ( *mLayers["Temperature"] )[cellIndex] = d;
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setUVel( ) {
    for( int tm = 0; tm < 12; tm++ ) {
        Time::Get( )->SetMonthlyTimeStep( tm );
        mLayers["uVel"]->setTime( tm );
        for( unsigned cellIndex = 0; cellIndex < Parameters::Get( )->GetNumberOfGridCells( ); cellIndex++ ) {
            double d = Constants::cMissingValue;

            if( DataLayerSet::Get( )->GetDataAtCellIndexFor( "Realm", cellIndex ) == 1 ) {
                d = DataLayerSet::Get( )->GetDataAtCellIndexFor( "MarineEastVel", cellIndex );
            }
            ( *mLayers["uVel"] )[cellIndex] = d;
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setVVel( ) {
    for( int tm = 0; tm < 12; tm++ ) {
        Time::Get( )->SetMonthlyTimeStep( tm );
        mLayers["vVel"]->setTime( tm );

        for( unsigned cellIndex = 0; cellIndex < Parameters::Get( )->GetNumberOfGridCells( ); cellIndex++ ) {
            double d = Constants::cMissingValue;

            if( DataLayerSet::Get( )->GetDataAtCellIndexFor( "Realm", cellIndex ) == 1 ) {
                d = DataLayerSet::Get( )->GetDataAtCellIndexFor( "MarineNorthVel", cellIndex );
            }
            ( *mLayers["vVel"] )[cellIndex] = d;
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setDiurnalTemperatureRange( ) {
    for( int tm = 0; tm < 12; tm++ ) {
        Time::Get( )->SetMonthlyTimeStep( tm );
        mLayers["DiurnalTemperatureRange"]->setTime( tm );
        for( unsigned cellIndex = 0; cellIndex < Parameters::Get( )->GetNumberOfGridCells( ); cellIndex++ ) {
            
            double d = Constants::cMissingValue;
            if( DataLayerSet::Get( )->GetDataAtCellIndexFor( "Realm", cellIndex ) == 2 ) {
                d = DataLayerSet::Get( )->GetDataAtCellIndexFor( "TerrestrialDTR", cellIndex );
            }

            ( *mLayers["DiurnalTemperatureRange"] )[cellIndex] = d;
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setPrecipitation( ) {
    for( unsigned cellIndex = 0; cellIndex < Parameters::Get( )->GetNumberOfGridCells( ); cellIndex++ ) {
        ( *mLayers["TotalPrecip"] )[cellIndex] = 0;
    }
    for( int tm = 0; tm < 12; tm++ ) {
        Time::Get( )->SetMonthlyTimeStep( tm );
        mLayers["Precipitation"]->setTime( tm );

        for( unsigned cellIndex = 0; cellIndex < Parameters::Get( )->GetNumberOfGridCells( ); cellIndex++ ) {
            double d = 0; // There are missing values here. No marine precipitation data.

            if( DataLayerSet::Get( )->GetDataAtCellIndexFor( "Realm", cellIndex ) == 2 ) {
                d = DataLayerSet::Get( )->GetDataAtCellIndexFor( "TerrestrialPre", cellIndex );
            }

            if( d == Constants::cMissingValue ) {
                d = 0;
                cout << "Warning Environment::setPrecipitation- missing values in precipitation field!!" << endl;
            }
            ( *mLayers["Precipitation"] )[cellIndex] = d;
            ( *mLayers["TotalPrecip"] )[cellIndex] += d;
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setNPP( ) {
    for( int tm = 0; tm < 12; tm++ ) {
        Time::Get( )->SetMonthlyTimeStep( tm );
        mLayers["NPP"]->setTime( tm );

        for( unsigned cellIndex = 0; cellIndex < Parameters::Get( )->GetNumberOfGridCells( ); cellIndex++ ) {
            double d = Constants::cMissingValue;
            if( DataLayerSet::Get( )->GetDataAtCellIndexFor( "Realm", cellIndex ) == 1 ) {
                d = DataLayerSet::Get( )->GetDataAtCellIndexFor( "MarineNPP", cellIndex );
            } else if( DataLayerSet::Get( )->GetDataAtCellIndexFor( "Realm", cellIndex ) == 2 ) {
                d = DataLayerSet::Get( )->GetDataAtCellIndexFor( "TerrestrialNPP", cellIndex );
            }

            if( d == Constants::cMissingValue ) {
                d = 0;
                cout << "Warning Environment::setNPP- missing values in NPP field!!" << endl;
            }
            ( *mLayers["NPP"] )[cellIndex] = d; 
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setRealm( ) {
    for( unsigned cellIndex = 0; cellIndex < Parameters::Get( )->GetNumberOfGridCells( ); cellIndex++ ) {
        float d = 0;
        if( DataLayerSet::Get( )->GetDataAtCellIndexFor( "Realm", cellIndex ) == 1 ) {
            d = 2.0;
        } else if( DataLayerSet::Get( )->GetDataAtCellIndexFor( "Realm", cellIndex ) == 2 ) {
            d = 1.0;
        }
        ( *mLayers["Realm"] )[cellIndex] = d;
    }
}
//------------------------------------------------------------------------------

void Environment::setOrganicPool( ) {
    for( unsigned cellIndex = 0; cellIndex < Parameters::Get( )->GetNumberOfGridCells( ); cellIndex++ ) {
        ( *mLayers["Organic Pool"] )[cellIndex] = 0;
    }
}
//------------------------------------------------------------------------------

void Environment::setRespiratoryCO2Pool( ) {
    for( unsigned cellIndex = 0; cellIndex < Parameters::Get( )->GetNumberOfGridCells( ); cellIndex++ ) {
        ( *mLayers["Respiratory CO2 Pool"] )[cellIndex] = 0;
    }
}
//------------------------------------------------------------------------------

void Environment::setAVGSDTemp( ) {
    for( unsigned cellIndex = 0; cellIndex < Parameters::Get( )->GetNumberOfGridCells( ); cellIndex++ ) {
        double avg = 0;
        for( int tm = 0; tm < 12; tm++ ) {

            double d = Constants::cMissingValue ;

            Time::Get( )->SetMonthlyTimeStep( tm );
            if( DataLayerSet::Get( )->GetDataAtCellIndexFor( "Realm", cellIndex ) == 1 ) {
                d = DataLayerSet::Get( )->GetDataAtCellIndexFor( "MarineTemp", cellIndex );
            } else if( DataLayerSet::Get( )->GetDataAtCellIndexFor( "Realm", cellIndex ) == 2 ) {
                d = DataLayerSet::Get( )->GetDataAtCellIndexFor( "TerrestrialTemp", cellIndex );
            }

            if( d == Constants::cMissingValue ) d = 0;
            avg += d;
        }
        avg = avg / 12;
        double sota = 0, sumExp = 0;
        vector<double>exptdev( 12 );
        for( int tm = 0; tm < 12; tm++ ) {

            double d = Constants::cMissingValue;
            Time::Get( )->SetMonthlyTimeStep( tm );
            
            if( DataLayerSet::Get( )->GetDataAtCellIndexFor( "Realm", cellIndex ) == 1 ) {
                d = DataLayerSet::Get( )->GetDataAtCellIndexFor( "MarineTemp", cellIndex );
            } else if( DataLayerSet::Get( )->GetDataAtCellIndexFor( "Realm", cellIndex ) == 2 ) {
                d = DataLayerSet::Get( )->GetDataAtCellIndexFor( "TerrestrialTemp", cellIndex );
            }

            if( d == Constants::cMissingValue )d = 0;
            sota += ( d - avg )*( d - avg );
            exptdev[tm] = exp( -( d - avg ) / 3 );
            sumExp += exptdev[tm];
        }
        for( int tm = 0; tm < 12; tm++ ) {
            mLayers["ExpTDevWeight"]->setTime( tm );
            ( *mLayers["ExpTDevWeight"] )[cellIndex] = exptdev[tm] / sumExp;
        }

        ( *mLayers["AnnualTemperature"] )[cellIndex] = avg;
        ( *mLayers["SDTemperature"] )[cellIndex] = sqrt( sota / 12 );
    }
}
//----------------------------------------------------------------------------------------------

/** \brief Calculate monthly seasonality values of Net Primary Production - ignores missing values. If there is no NPP data (ie all zero or missing values)
then assign 1/12 for each month.
 */
void Environment::setNPPSeasonality( ) {
    for( unsigned cellIndex = 0; cellIndex < Parameters::Get( )->GetNumberOfGridCells( ); cellIndex++ ) {
        // Loop over months and calculate total annual NPP
        double TotalNPP = 0.0;
        for( int i = 0; i < 12; i++ ) {
            mLayers["NPP"]->setTime( i );
            double N = ( *mLayers["NPP"] )[cellIndex];
            if( N != Constants::cMissingValue && N > 0 ) TotalNPP += N;
        }
        if( TotalNPP == 0 ) {
            // Loop over months and calculate seasonality
            // If there is no NPP value then assign a uniform flat seasonality
            for( int i = 0; i < 12; i++ ) {
                mLayers["Seasonality"]->setTime( i );
                ( *mLayers["Seasonality"] )[cellIndex] = 1.0 / 12.0;
            }

        } else {
            // Some NPP data exists for this grid cell so use that to infer the NPP seasonality
            // Loop over months and calculate seasonality
            for( int i = 0; i < 12; i++ ) {
                mLayers["NPP"]->setTime( i );
                mLayers["Seasonality"]->setTime( i );
                double N = ( *mLayers["NPP"] )[cellIndex];
                if( N != Constants::cMissingValue && N > 0 ) {
                    ( *mLayers["Seasonality"] )[cellIndex] = N / TotalNPP;
                } else {
                    ( *mLayers["Seasonality"] )[cellIndex] = 0.0;
                }
            }
        }

    }
}
//----------------------------------------------------------------------------------------------

void Environment::setFrostandFire( ) {
    // Calculate other climate variables from temperature and precipitation
    // Declare an instance of the climate variables calculator
    ClimateVariablesCalculator CVC;

    for( unsigned cellIndex = 0; cellIndex < Parameters::Get( )->GetNumberOfGridCells( ); cellIndex++ ) {
        // Calculate the fraction of the year that experiences frost
        vector<double> FrostDays( 12 ), Temperature( 12 ), Precipitation( 12 );
        for( int i = 0; i < 12; i++ ) {

            Time::Get( )->SetMonthlyTimeStep( i );
            if( DataLayerSet::Get( )->GetDataAtCellIndexFor( "Realm", cellIndex ) == 2 ) {
                FrostDays[i] = DataLayerSet::Get( )->GetDataAtCellIndexFor( "TerrestrialFrost", cellIndex );
                Precipitation[i] = DataLayerSet::Get( )->GetDataAtCellIndexFor( "TerrestrialPre", cellIndex );
                Temperature[i] = DataLayerSet::Get( )->GetDataAtCellIndexFor( "TerrestrialTemp", cellIndex );
            }
        }
        ( *mLayers["Fraction Year Frost"] )[cellIndex] = CVC.GetNDF( FrostDays, Temperature, Constants::cMissingValue );

        vector<double> MonthDays = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

        for( int i = 0; i < 12; i++ ) {
            mLayers["Fraction Month Frost"]->setTime( i );
            ( *mLayers["Fraction Month Frost"] )[cellIndex] = min( FrostDays[i] / MonthDays[i], ( double )1.0 );
        }
        double AWC = DataLayerSet::Get( )->GetDataAtCellIndexFor( "TerrestrialAWC", cellIndex );

        tuple<vector<double>, double, double> TempTuple = CVC.MonthlyActualEvapotranspirationSoilMoisture( AWC, Precipitation, Temperature );
        ( *mLayers["TotalAET"] )[cellIndex] = 0;
        for( int i = 0; i < 12; i++ ) {
            mLayers["AET"]->setTime( i );
            ( *mLayers["AET"] )[cellIndex] = get<0>( TempTuple )[i];
            ( *mLayers["TotalAET"] )[cellIndex] += get<0>( TempTuple )[i];
        }
        ( *mLayers["Fraction Year Fire"] )[cellIndex] = ( get<2> ( TempTuple ) / 360 );
    }
}
//----------------------------------------------------------------------------------------------

void Environment::setBreeding( ) {
    // Designate a breeding season for this grid cell, where a month is considered to be part of the breeding season if its NPP is at
    // least 80% of the maximum NPP throughout the whole year
    for( unsigned cellIndex = 0; cellIndex < Parameters::Get( )->GetNumberOfGridCells( ); cellIndex++ ) {
        double maxSeason = -1;
        for( int i = 0; i < 12; i++ ) {
            mLayers["Seasonality"]->setTime( i );
            maxSeason = max( maxSeason, ( *mLayers["Seasonality"] )[cellIndex] );
        }
        for( int i = 0; i < 12; i++ ) {
            mLayers["Seasonality"]->setTime( i );
            mLayers["Breeding Season"]->setTime( i );

            if( ( *mLayers["Seasonality"] )[cellIndex] / maxSeason > 0.5 ) {
                ( *mLayers["Breeding Season"] )[cellIndex] = 1.0;
            } else {
                ( *mLayers["Breeding Season"] )[cellIndex] = 0.0;
            }
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setHANPP( ) {
    for( unsigned cellIndex = 0; cellIndex < Parameters::Get( )->GetNumberOfGridCells( ); cellIndex++ ) {
        ( *mLayers["TerrestrialHANPP"] )[cellIndex] = 0;
    }
}