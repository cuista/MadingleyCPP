/* 
 * File:   Environment.cc
 * Author: mb425
 *
 * Created on 26 November 2015, 13:32
 */
#include <Environment.h>
#include <GridCell.h>

#include "Types.h"
#include "FileReader.h"
#include "Logger.h"
#include "Parameters.h"
#include "DataLayerSet.h"
#include "DataIndices.h"
#include "DateTime.h"

Types::EnvironmentPointer Environment::mThis = NULL;
Types::LayerMap Environment::mLayers;
//------------------------------------------------------------------------------

Environment::Environment( ) {
    cout << "Reading netcdf??" << endl;
    Types::FileReaderPointer fileReader = new FileReader( );

    bool completedSuccessfully = fileReader->ReadFiles( );

    if( completedSuccessfully == true ) {
        Logger::Get( )->LogMessage( "Files read successfully..." );

        cout << endl;
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

        addLayer( "Realm" );
        setRealm( );
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
        addLayer( "HANPP" );
        setHANPP( );
        //make sure all time dependent fields set to the start
        update( 0 );
    } else {
        Logger::Get( )->LogMessage( "ERROR> File reading failed." );
    }
}
//------------------------------------------------------------------------------

void Environment::update( int currentMonth ) {
    for( auto& L: mLayers )L.second->setTime( currentMonth );
}
//------------------------------------------------------------------------------

void Environment::addLayer( string s ) {
    const unsigned int NumLon = Parameters::Get( )->GetLengthUserLongitudeArray( );
    const unsigned int NumLat = Parameters::Get( )->GetLengthUserLatitudeArray( );
    mLayers[s] = new layer0( NumLon, NumLat );
}
//------------------------------------------------------------------------------

void Environment::addLayerT( string s ) {
    const unsigned int NumLon = Parameters::Get( )->GetLengthUserLongitudeArray( );
    const unsigned int NumLat = Parameters::Get( )->GetLengthUserLatitudeArray( );
    mLayers[s] = new layerT( 12, NumLon, NumLat );
}
//------------------------------------------------------------------------------

Environment* Environment::Get( ) {
    if( mThis == NULL ) {
        mThis = new Environment( );
    }
    return mThis;
}
//------------------------------------------------------------------------------

double& Environment::Get( string s, int lo, int la ) {
    if( mThis == NULL ) mThis = new Environment( );
    if( mLayers.count( s ) == 0 ) {
        cout << "Invalid Layer Request in Environment:: " << s << endl;
        exit( 0 );
    }
    return (*mLayers[s] )[lo][la];
}
//------------------------------------------------------------------------------

double& Environment::Get( string s, GridCell& gcl ) {
    if( mThis == 0 )mThis = new Environment( );
    int lo = gcl.GetLongitudeIndex( );
    int la = gcl.GetLatitudeIndex( );
    if( mLayers.count( s ) == 0 ) {
        cout << "Invalid Layer Request in Environment:: " << s << endl;
        exit( 0 );
    }
    return (*mLayers[s] )[lo][la];
}
//------------------------------------------------------------------------------

void Environment::setTemperature( ) {
    const unsigned int NumLon = Parameters::Get( )->GetLengthUserLongitudeArray( );
    const unsigned int NumLat = Parameters::Get( )->GetLengthUserLatitudeArray( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            for( int tm = 0; tm < 12; tm++ ) {

                double d = 0;

                DateTime::Get( )->SetTimeStep( tm );
                Types::DataIndicesPointer indices = new DataIndices( lo, la, Constants::eUserDomain );
                if( DataLayerSet::Get( )->GetDataAtIndicesFor( "Realm", indices ) == 1 ) {
                    d = DataLayerSet::Get( )->GetDataAtIndicesFor( "MarineTemp", indices );
                } else if( DataLayerSet::Get( )->GetDataAtIndicesFor( "Realm", indices ) == 2 ) {
                    d = DataLayerSet::Get( )->GetDataAtIndicesFor( "TerrestrialTemp", indices );
                }
                delete indices;

                mLayers["Temperature"]->setTime( tm );
                if( d == Constants::cMissingValue ) {
                    d = 0;
                    cout << "Warning Environment::setTemperature- missing values in temperature field!!" << endl;
                }
                ( *mLayers["Temperature"] )[lo][la] = d;
            }

        }
    }
}
//------------------------------------------------------------------------------

void Environment::setUVel( ) {
    const unsigned int NumLon = Parameters::Get( )->GetLengthUserLongitudeArray( );
    const unsigned int NumLat = Parameters::Get( )->GetLengthUserLatitudeArray( );

    for( int tm = 0; tm < 12; tm++ ) {
        for( int lo = 0; lo < NumLon; lo++ ) {
            for( int la = 0; la < NumLat; la++ ) {

                double d = 0;

                DateTime::Get( )->SetTimeStep( tm );
                Types::DataIndicesPointer indices = new DataIndices( lo, la, Constants::eUserDomain );
                if( DataLayerSet::Get( )->GetDataAtIndicesFor( "Realm", indices ) == 1 ) {
                    d = DataLayerSet::Get( )->GetDataAtIndicesFor( "MarineNorthVel", indices );
                }
                delete indices;

                mLayers["uVel"]->setTime( tm );
                ( *mLayers["uVel"] )[lo][la] = d;
            }
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setVVel( ) {
    const unsigned int NumLon = Parameters::Get( )->GetLengthUserLongitudeArray( );
    const unsigned int NumLat = Parameters::Get( )->GetLengthUserLatitudeArray( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            for( int tm = 0; tm < 12; tm++ ) {

                double d = 0;

                DateTime::Get( )->SetTimeStep( tm );
                Types::DataIndicesPointer indices = new DataIndices( lo, la, Constants::eUserDomain );
                if( DataLayerSet::Get( )->GetDataAtIndicesFor( "Realm", indices ) == 1 ) {
                    d = DataLayerSet::Get( )->GetDataAtIndicesFor( "MarineEastVel", indices );
                }
                delete indices;

                mLayers["vVel"]->setTime( tm );
                ( *mLayers["vVel"] )[lo][la] = d;
            }

        }
    }
}
//------------------------------------------------------------------------------

void Environment::setDiurnalTemperatureRange( ) {
    const unsigned int NumLon = Parameters::Get( )->GetLengthUserLongitudeArray( );
    const unsigned int NumLat = Parameters::Get( )->GetLengthUserLatitudeArray( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            for( int tm = 0; tm < 12; tm++ ) {

                double d = 0;

                DateTime::Get( )->SetTimeStep( tm );
                Types::DataIndicesPointer indices = new DataIndices( lo, la, Constants::eUserDomain );
                if( DataLayerSet::Get( )->GetDataAtIndicesFor( "Realm", indices ) == 1 ) {
                    d = Constants::cMissingValue; //MB currently missing
                } else if( DataLayerSet::Get( )->GetDataAtIndicesFor( "Realm", indices ) == 2 ) {
                    d = DataLayerSet::Get( )->GetDataAtIndicesFor( "TerrestrialDTR", indices );
                }
                delete indices;

                mLayers["DiurnalTemperatureRange"]->setTime( tm );
                ( *mLayers["DiurnalTemperatureRange"] )[lo][la] = d;
            }

        }
    }
}
//------------------------------------------------------------------------------

void Environment::setPrecipitation( ) {

    const unsigned int NumLon = Parameters::Get( )->GetLengthUserLongitudeArray( );
    const unsigned int NumLat = Parameters::Get( )->GetLengthUserLatitudeArray( );

    for( int tm = 0; tm < 12; tm++ ) {
        for( int lo = 0; lo < NumLon; lo++ ) {
            for( int la = 0; la < NumLat; la++ ) {

                ( *mLayers["TotalPrecip"] )[lo][la] = 0;
                double d = 0;

                DateTime::Get( )->SetTimeStep( tm );
                Types::DataIndicesPointer indices = new DataIndices( lo, la, Constants::eUserDomain );
                if( DataLayerSet::Get( )->GetDataAtIndicesFor( "Realm", indices ) == 2 ) {
                    d = DataLayerSet::Get( )->GetDataAtIndicesFor( "TerrestrialPre", indices );
                }
                delete indices;

                if( d == Constants::cMissingValue ) {
                    d = 0;
                    cout << "Warning Environment::setPrecipitation- missing values in precipitation field!!" << endl;
                }
                mLayers["Precipitation"]->setTime( tm );
                ( *mLayers["Precipitation"] )[lo][la] = d;
                ( *mLayers["TotalPrecip"] )[lo][la] += d;
            }
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setNPP( ) {
    const unsigned int NumLon = Parameters::Get( )->GetLengthUserLongitudeArray( );
    const unsigned int NumLat = Parameters::Get( )->GetLengthUserLatitudeArray( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            for( int tm = 0; tm < 12; tm++ ) {

                double d = 0;

                DateTime::Get( )->SetTimeStep( tm );
                Types::DataIndicesPointer indices = new DataIndices( lo, la, Constants::eUserDomain );
                if( DataLayerSet::Get( )->GetDataAtIndicesFor( "Realm", indices ) == 1 ) {
                    d = DataLayerSet::Get( )->GetDataAtIndicesFor( "MarineNPP", indices );
                } else if( DataLayerSet::Get( )->GetDataAtIndicesFor( "Realm", indices ) == 2 ) {
                    d = DataLayerSet::Get( )->GetDataAtIndicesFor( "TerrestrialNPP", indices );
                }
                delete indices;

                if( d == Constants::cMissingValue ) {
                    d = 0;
                    cout << "Warning Environment::setNPP- missing values in NPP field!!" << endl;
                }
                mLayers["NPP"]->setTime( tm );
                ( *mLayers["NPP"] )[lo][la] = d;
            }
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setRealm( ) {
    const unsigned int NumLon = Parameters::Get( )->GetLengthUserLongitudeArray( );
    const unsigned int NumLat = Parameters::Get( )->GetLengthUserLatitudeArray( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {

            Types::DataIndicesPointer indices = new DataIndices( lo, la, Constants::eUserDomain );
            if( DataLayerSet::Get( )->GetDataAtIndicesFor( "Realm", indices ) == 1 ) {
                ( *mLayers["Realm"] )[lo][la] = 2.0;
            } else if( DataLayerSet::Get( )->GetDataAtIndicesFor( "Realm", indices ) == 2 ) {
                ( *mLayers["Realm"] )[lo][la] = 1.0;
            }
            delete indices;
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setOrganicPool( ) {
    const unsigned int NumLon = Parameters::Get( )->GetLengthUserLongitudeArray( );
    const unsigned int NumLat = Parameters::Get( )->GetLengthUserLatitudeArray( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            ( *mLayers["Organic Pool"] )[lo][la] = 0;
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setRespiratoryCO2Pool( ) {
    const unsigned int NumLon = Parameters::Get( )->GetLengthUserLongitudeArray( );
    const unsigned int NumLat = Parameters::Get( )->GetLengthUserLatitudeArray( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            ( *mLayers["Respiratory CO2 Pool"] )[lo][la] = 0;
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setAVGSDTemp( ) {

    const unsigned int NumLon = Parameters::Get( )->GetLengthUserLongitudeArray( );
    const unsigned int NumLat = Parameters::Get( )->GetLengthUserLatitudeArray( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            double avg = 0;
            for( int tm = 0; tm < 12; tm++ ) {

                double d = 0;

                DateTime::Get( )->SetTimeStep( tm );
                Types::DataIndicesPointer indices = new DataIndices( lo, la, Constants::eUserDomain );
                if( DataLayerSet::Get( )->GetDataAtIndicesFor( "Realm", indices ) == 1 ) {
                    d = DataLayerSet::Get( )->GetDataAtIndicesFor( "MarineTemp", indices );
                } else if( DataLayerSet::Get( )->GetDataAtIndicesFor( "Realm", indices ) == 2 ) {
                    d = DataLayerSet::Get( )->GetDataAtIndicesFor( "TerrestrialTemp", indices );
                }
                delete indices;

                if( d == Constants::cMissingValue )d = 0;
                avg += d;
            }
            avg = avg / 12;
            double sota = 0, sumExp = 0;
            vector<double>exptdev( 12 );
            for( int tm = 0; tm < 12; tm++ ) {

                double d = 0;

                DateTime::Get( )->SetTimeStep( tm );
                Types::DataIndicesPointer indices = new DataIndices( lo, la, Constants::eUserDomain );
                if( DataLayerSet::Get( )->GetDataAtIndicesFor( "Realm", indices ) == 1 ) {
                    d = DataLayerSet::Get( )->GetDataAtIndicesFor( "MarineTemp", indices );
                } else if( DataLayerSet::Get( )->GetDataAtIndicesFor( "Realm", indices ) == 2 ) {
                    d = DataLayerSet::Get( )->GetDataAtIndicesFor( "TerrestrialTemp", indices );
                }
                delete indices;

                if( d == Constants::cMissingValue )d = 0;
                sota += ( d - avg )*( d - avg );
                exptdev[tm] = exp( -( d - avg ) / 3 );
                sumExp += exptdev[tm];
            }
            for( int tm = 0; tm < 12; tm++ ) {
                mLayers["ExpTDevWeight"]->setTime( tm );
                ( *mLayers["ExpTDevWeight"] )[lo][la] = exptdev[tm] / sumExp;
            }

            ( *mLayers["AnnualTemperature"] )[lo][la] = avg;
            ( *mLayers["SDTemperature"] )[lo][la] = sqrt( sota / 12 );
        }
    }
}
//----------------------------------------------------------------------------------------------

/** \brief Calculate monthly seasonality values of Net Primary Production - ignores missing values. If there is no NPP data (ie all zero or missing values)
then assign 1/12 for each month.
 */
void Environment::setNPPSeasonality( ) {
    const unsigned int NumLon = Parameters::Get( )->GetLengthUserLongitudeArray( );
    const unsigned int NumLat = Parameters::Get( )->GetLengthUserLatitudeArray( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            // Loop over months and calculate total annual NPP
            double TotalNPP = 0.0;
            for( int i = 0; i < 12; i++ ) {
                mLayers["NPP"]->setTime( i );
                double N = ( *mLayers["NPP"] )[lo][la];
                if( N != Constants::cMissingValue && N > 0 ) TotalNPP += N;
            }
            if( TotalNPP == 0 ) {
                // Loop over months and calculate seasonality
                // If there is no NPP value then assign a uniform flat seasonality
                for( int i = 0; i < 12; i++ ) {
                    mLayers["Seasonality"]->setTime( i );
                    ( *mLayers["Seasonality"] )[lo][la] = 1.0 / 12.0;
                }

            } else {
                // Some NPP data exists for this grid cell so use that to infer the NPP seasonality
                // Loop over months and calculate seasonality
                for( int i = 0; i < 12; i++ ) {
                    mLayers["NPP"]->setTime( i );
                    mLayers["Seasonality"]->setTime( i );
                    double N = ( *mLayers["NPP"] )[lo][la];
                    if( N != Constants::cMissingValue && N > 0 ) {
                        ( *mLayers["Seasonality"] )[lo][la] = N / TotalNPP;
                    } else {
                        ( *mLayers["Seasonality"] )[lo][la] = 0.0;
                    }
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
    const unsigned int NumLon = Parameters::Get( )->GetLengthUserLongitudeArray( );
    const unsigned int NumLat = Parameters::Get( )->GetLengthUserLatitudeArray( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            // Calculate the fraction of the year that experiences frost
            vector<double> FrostDays( 12 ), Temperature( 12 ), Precipitation( 12 );
            for( int i = 0; i < 12; i++ ) {

                DateTime::Get( )->SetTimeStep( i );
                Types::DataIndicesPointer indices = new DataIndices( lo, la, Constants::eUserDomain );
                if( DataLayerSet::Get( )->GetDataAtIndicesFor( "Realm", indices ) == 2 ) {
                    FrostDays[i] = DataLayerSet::Get( )->GetDataAtIndicesFor( "TerrestrialFrost", indices );
                    Precipitation[i] = DataLayerSet::Get( )->GetDataAtIndicesFor( "TerrestrialPre", indices );
                    Temperature[i] = DataLayerSet::Get( )->GetDataAtIndicesFor( "TerrestrialTemp", indices );
                }
                delete indices;
            }
            ( *mLayers["Fraction Year Frost"] )[lo][la] = CVC.GetNDF( FrostDays, Temperature, Constants::cMissingValue );

            vector<double> MonthDays = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

            for( int i = 0; i < 12; i++ ) {
                mLayers["Fraction Month Frost"]->setTime( i );
                ( *mLayers["Fraction Month Frost"] )[lo][la] = min( FrostDays[i] / MonthDays[i], 1.0 );
            }
            Types::DataIndicesPointer indices = new DataIndices( lo, la, Constants::eUserDomain );
            double AWC = DataLayerSet::Get( )->GetDataAtIndicesFor( "TerrestrialAWC", indices );
            delete indices;

            tuple<vector<double>, double, double> TempTuple = CVC.MonthlyActualEvapotranspirationSoilMoisture( AWC, Precipitation, Temperature );
            ( *mLayers["TotalAET"] )[lo][la] = 0;
            for( int i = 0; i < 12; i++ ) {
                mLayers["AET"]->setTime( i );
                ( *mLayers["AET"] )[lo][la] = get<0>( TempTuple )[i];
                ( *mLayers["TotalAET"] )[lo][la] += get<0>( TempTuple )[i];
            }
            ( *mLayers["Fraction Year Fire"] )[lo][la] = ( get<2> ( TempTuple ) / 360 );
        }
    }
}
//----------------------------------------------------------------------------------------------

void Environment::setBreeding( ) {
    // Designate a breeding season for this grid cell, where a month is considered to be part of the breeding season if its NPP is at
    // least 80% of the maximum NPP throughout the whole year
    const unsigned int NumLon = Parameters::Get( )->GetLengthUserLongitudeArray( );
    const unsigned int NumLat = Parameters::Get( )->GetLengthUserLatitudeArray( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            double maxSeason = -1;
            for( int i = 0; i < 12; i++ ) {
                mLayers["Seasonality"]->setTime( i );
                maxSeason = max( maxSeason, ( *mLayers["Seasonality"] )[lo][la] );
            }
            for( int i = 0; i < 12; i++ ) {
                mLayers["Seasonality"]->setTime( i );
                mLayers["Breeding Season"]->setTime( i );

                if( ( *mLayers["Seasonality"] )[lo][la] / maxSeason > 0.5 ) {
                    ( *mLayers["Breeding Season"] )[lo][la] = 1.0;
                } else {
                    ( *mLayers["Breeding Season"] )[lo][la] = 0.0;
                }
            }
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setHANPP( ) {
    const unsigned int NumLon = Parameters::Get( )->GetLengthUserLongitudeArray( );
    const unsigned int NumLat = Parameters::Get( )->GetLengthUserLatitudeArray( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            ( *mLayers["HANPP"] )[lo][la] = 0;
        }
    }
}
