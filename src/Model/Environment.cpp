/* 
 * File:   Environment.cc
 * Author: mb425
 *
 * Created on 26 November 2015, 13:32
 */
#include <Environment.h>
#include <GridCell.h>

Environment* Environment::Instance = 0;
const double Environment::MissingValue = -9999;
map<string, layer*> Environment::Layers;
//------------------------------------------------------------------------------

Environment::Environment( ) {
    double degreeResolution = 10;
    //std::string mInputBaseDirectory = "../netCDFdata/";
    std::string mInputBaseDirectory = "input/Data/";

    cout << "Reading netcdf??" << endl;
    Types::FileReaderPointer mFileReader;
    mFileReader = new FileReader( );

    for( unsigned int variableIndex = 0; variableIndex < Constants::cNumberOfInputFiles; ++variableIndex ) {
        std::string inputFilePath = mInputBaseDirectory + Constants::cInputFileNames[ variableIndex ];
        mFileReader->ReadNetCDFFile( inputFilePath, variableIndex );
    }

    Logger::Get( )->LogString( "Finished reading netcdf!" );
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
}
//------------------------------------------------------------------------------

void Environment::update( int currentMonth ) {
    for( auto& L: Layers )L.second->setTime( currentMonth );
}
//------------------------------------------------------------------------------

void Environment::addLayer( string s ) {
    const unsigned int NumLon = DataGrid::Get( )->GetLengthLongitudeVector( );
    const unsigned int NumLat = DataGrid::Get( )->GetLengthLatitudeVector( );
    Layers[s] = new layer0( NumLon, NumLat );
}
//------------------------------------------------------------------------------

void Environment::addLayerT( string s ) {
    const unsigned int NumLon = DataGrid::Get( )->GetLengthLongitudeVector( );
    const unsigned int NumLat = DataGrid::Get( )->GetLengthLatitudeVector( );
    Layers[s] = new layerT( 12, NumLon, NumLat );
}
//------------------------------------------------------------------------------

Environment* Environment::Get( ) {
    if( Instance == 0 ) {
        Instance = new Environment( );
    }
    return Instance;
}
//------------------------------------------------------------------------------

double& Environment::Get( string s, int lo, int la ) {
    if( Instance == 0 )Instance = new Environment( );
    if( Layers.count( s ) == 0 ) {
        cout << "Invalid Layer Request in Environment:: " << s << endl;
        exit( 0 );
    }
    return (*Layers[s] )[lo][la];
}
//------------------------------------------------------------------------------

double& Environment::Get( string s, GridCell& gcl ) {
    if( Instance == 0 )Instance = new Environment( );
    int lo = gcl.LonIndex( );
    int la = gcl.LatIndex( );
    if( Layers.count( s ) == 0 ) {
        cout << "Invalid Layer Request in Environment:: " << s << endl;
        exit( 0 );
    }
    return (*Layers[s] )[lo][la];
}
//------------------------------------------------------------------------------

void Environment::setTemperature( ) {
    const unsigned int NumLon = DataGrid::Get( )->GetLengthLongitudeVector( );
    const unsigned int NumLat = DataGrid::Get( )->GetLengthLatitudeVector( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            for( int tm = 0; tm < 12; tm++ ) {
                double d;
                if( DataGrid::Get( )->GetGridCell( lo, la )->GetRealm( ) == 1 )
                    d = DataGrid::Get( )->GetGridCell( lo, la )->GetSST( tm );
                else if( DataGrid::Get( )->GetGridCell( lo, la )->GetRealm( ) == 2 )
                    d = DataGrid::Get( )->GetGridCell( lo, la )->GetNearSurfaceTemperature( tm );
                Layers["Temperature"]->setTime( tm );
                if( d == MissingValue ) {
                    d = 0;
                    cout << "Warning Environment::setTemperature- missing values in temperature field!!" << endl;
                }
                ( *Layers["Temperature"] )[lo][la] = d;
            }

        }
    }
}
//------------------------------------------------------------------------------

void Environment::setUVel( ) {
    const unsigned int NumLon = DataGrid::Get( )->GetLengthLongitudeVector( );
    const unsigned int NumLat = DataGrid::Get( )->GetLengthLatitudeVector( );

    for( int tm = 0; tm < 12; tm++ ) {
        for( int lo = 0; lo < NumLon; lo++ ) {
            for( int la = 0; la < NumLat; la++ ) {

                double d = 0;
                
                if( DataGrid::Get( )->GetGridCell( lo, la )->GetRealm( ) == 1 )
                    d = DataGrid::Get( )->GetGridCell( lo, la )->GetUSpeed( tm );

                Layers["uVel"]->setTime( tm );
                ( *Layers["uVel"] )[lo][la] = d;
            }
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setVVel( ) {
    const unsigned int NumLon = DataGrid::Get( )->GetLengthLongitudeVector( );
    const unsigned int NumLat = DataGrid::Get( )->GetLengthLatitudeVector( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            for( int tm = 0; tm < 12; tm++ ) {
                double d = 0;
                
                if( DataGrid::Get( )->GetGridCell( lo, la )->GetRealm( ) == 1 )
                    d = DataGrid::Get( )->GetGridCell( lo, la )->GetUSpeed( tm );
                
                Layers["vVel"]->setTime( tm );
                ( *Layers["vVel"] )[lo][la] = d;
            }

        }
    }
}
//------------------------------------------------------------------------------

void Environment::setDiurnalTemperatureRange( ) {
    const unsigned int NumLon = DataGrid::Get( )->GetLengthLongitudeVector( );
    const unsigned int NumLat = DataGrid::Get( )->GetLengthLatitudeVector( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            for( int tm = 0; tm < 12; tm++ ) {
                double d;
                if( DataGrid::Get( )->GetGridCell( lo, la )->GetRealm( ) == 1 )
                    d = MissingValue; //MB currently missing
                else
                    d = DataGrid::Get( )->GetGridCell( lo, la )->GetDiurnalTemperatureRange( tm );
                Layers["DiurnalTemperatureRange"]->setTime( tm );
                ( *Layers["DiurnalTemperatureRange"] )[lo][la] = d;
            }

        }
    }
}
//------------------------------------------------------------------------------

void Environment::setPrecipitation( ) {

    const unsigned int NumLon = DataGrid::Get( )->GetLengthLongitudeVector( );
    const unsigned int NumLat = DataGrid::Get( )->GetLengthLatitudeVector( );

    for( int tm = 0; tm < 12; tm++ ) {
        for( int lo = 0; lo < NumLon; lo++ ) {
            for( int la = 0; la < NumLat; la++ ) {
                ( *Layers["TotalPrecip"] )[lo][la] = 0;
                double d;

                if( DataGrid::Get( )->GetGridCell( lo, la )->GetRealm( ) == 2 )
                    d = DataGrid::Get( )->GetGridCell( lo, la )->GetPrecipitation( tm );

                if( d == MissingValue ) {
                    d = 0;
                    cout << "Warning Environment::setPrecipitation- missing values in precipitation field!!" << endl;
                }
                Layers["Precipitation"]->setTime( tm );
                ( *Layers["Precipitation"] )[lo][la] = d;
                ( *Layers["TotalPrecip"] )[lo][la] += d;
            }
            //cout << endl;
        }

        //Logger::Get( )->LogString( "" );
    }
}
//------------------------------------------------------------------------------

void Environment::setNPP( ) {
    const unsigned int NumLon = DataGrid::Get( )->GetLengthLongitudeVector( );
    const unsigned int NumLat = DataGrid::Get( )->GetLengthLatitudeVector( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            for( int tm = 0; tm < 12; tm++ ) {
                double d;
                if( DataGrid::Get( )->GetGridCell( lo, la )->GetRealm( ) == 1 )
                    d = DataGrid::Get( )->GetGridCell( lo, la )->GetOceanNPP( tm );
                else if( DataGrid::Get( )->GetGridCell( lo, la )->GetRealm( ) == 2 )
                    d = DataGrid::Get( )->GetGridCell( lo, la )->GetNPP( tm );
                if( d == MissingValue ) {
                    d = 0;
                    cout << "Warning Environment::setNPP- missing values in NPP field!!" << endl;
                }
                Layers["NPP"]->setTime( tm );
                ( *Layers["NPP"] )[lo][la] = d;
            }

        }
    }
}
//------------------------------------------------------------------------------

void Environment::setRealm( ) {
    const unsigned int NumLon = DataGrid::Get( )->GetLengthLongitudeVector( );
    const unsigned int NumLat = DataGrid::Get( )->GetLengthLatitudeVector( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            ( *Layers["Realm"] )[lo][la] = DataGrid::Get( )->GetGridCell( lo, la )->GetRealm( );
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setOrganicPool( ) {
    const unsigned int NumLon = DataGrid::Get( )->GetLengthLongitudeVector( );
    const unsigned int NumLat = DataGrid::Get( )->GetLengthLatitudeVector( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            ( *Layers["Organic Pool"] )[lo][la] = 0;
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setRespiratoryCO2Pool( ) {
    const unsigned int NumLon = DataGrid::Get( )->GetLengthLongitudeVector( );
    const unsigned int NumLat = DataGrid::Get( )->GetLengthLatitudeVector( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            ( *Layers["Respiratory CO2 Pool"] )[lo][la] = 0;
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setAVGSDTemp( ) {

    const unsigned int NumLon = DataGrid::Get( )->GetLengthLongitudeVector( );
    const unsigned int NumLat = DataGrid::Get( )->GetLengthLatitudeVector( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            double avg = 0;
            for( int tm = 0; tm < 12; tm++ ) {
                double d;
                if( DataGrid::Get( )->GetGridCell( lo, la )->GetRealm( ) == 1 )
                    d = DataGrid::Get( )->GetGridCell( lo, la )->GetSST( tm );
                else if( DataGrid::Get( )->GetGridCell( lo, la )->GetRealm( ) == 2 )
                    d = DataGrid::Get( )->GetGridCell( lo, la )->GetNearSurfaceTemperature( tm );
                if( d == MissingValue )d = 0;
                avg += d;
            }
            avg = avg / 12;
            double sota = 0, sumExp = 0;
            vector<double>exptdev( 12 );
            for( int tm = 0; tm < 12; tm++ ) {
                double d;
                if( DataGrid::Get( )->GetGridCell( lo, la )->GetRealm( ) == 1 )
                    d = DataGrid::Get( )->GetGridCell( lo, la )->GetSST( tm );
                else if( DataGrid::Get( )->GetGridCell( lo, la )->GetRealm( ) == 2 )
                    d = DataGrid::Get( )->GetGridCell( lo, la )->GetNearSurfaceTemperature( tm );
                if( d == MissingValue )d = 0;
                sota += ( d - avg )*( d - avg );
                exptdev[tm] = exp( -( d - avg ) / 3 );
                sumExp += exptdev[tm];
            }
            for( int tm = 0; tm < 12; tm++ ) {
                Layers["ExpTDevWeight"]->setTime( tm );
                ( *Layers["ExpTDevWeight"] )[lo][la] = exptdev[tm] / sumExp;
            }

            ( *Layers["AnnualTemperature"] )[lo][la] = avg;
            ( *Layers["SDTemperature"] )[lo][la] = sqrt( sota / 12 );
        }
    }
}
//----------------------------------------------------------------------------------------------

/** \brief Calculate monthly seasonality values of Net Primary Production - ignores missing values. If there is no NPP data (ie all zero or missing values)
then assign 1/12 for each month.
 */
void Environment::setNPPSeasonality( ) {
    const unsigned int NumLon = DataGrid::Get( )->GetLengthLongitudeVector( );
    const unsigned int NumLat = DataGrid::Get( )->GetLengthLatitudeVector( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            // Loop over months and calculate total annual NPP
            double TotalNPP = 0.0;
            for( int i = 0; i < 12; i++ ) {
                Layers["NPP"]->setTime( i );
                double N = ( *Layers["NPP"] )[lo][la];
                if( N != MissingValue && N > 0 ) TotalNPP += N;
            }
            if( TotalNPP == 0 ) {
                // Loop over months and calculate seasonality
                // If there is no NPP value then assign a uniform flat seasonality
                for( int i = 0; i < 12; i++ ) {
                    Layers["Seasonality"]->setTime( i );
                    ( *Layers["Seasonality"] )[lo][la] = 1.0 / 12.0;
                }

            } else {
                // Some NPP data exists for this grid cell so use that to infer the NPP seasonality
                // Loop over months and calculate seasonality
                for( int i = 0; i < 12; i++ ) {
                    Layers["NPP"]->setTime( i );
                    Layers["Seasonality"]->setTime( i );
                    double N = ( *Layers["NPP"] )[lo][la];
                    if( N != MissingValue && N > 0 ) {
                        ( *Layers["Seasonality"] )[lo][la] = N / TotalNPP;
                    } else {
                        ( *Layers["Seasonality"] )[lo][la] = 0.0;
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
    const unsigned int NumLon = DataGrid::Get( )->GetLengthLongitudeVector( );
    const unsigned int NumLat = DataGrid::Get( )->GetLengthLatitudeVector( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            // Calculate the fraction of the year that experiences frost
            vector<double> FrostDays( 12 ), Temperature( 12 ), Precipitation( 12 );
            for( int i = 0; i < 12; i++ ) {
                if( DataGrid::Get( )->GetGridCell( lo, la )->GetRealm( ) == 2 ) {
                    FrostDays[i] = DataGrid::Get( )->GetGridCell( lo, la )->GetGroundFrostFrequency( i );
                    Precipitation[i] = DataGrid::Get( )->GetGridCell( lo, la )->GetPrecipitation( i );
                    Temperature[i] = DataGrid::Get( )->GetGridCell( lo, la )->GetNearSurfaceTemperature( i );
                }
            }
            ( *Layers["Fraction Year Frost"] )[lo][la] = CVC.GetNDF( FrostDays, Temperature, MissingValue );

            vector<double> MonthDays = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

            for( int i = 0; i < 12; i++ ) {
                Layers["Fraction Month Frost"]->setTime( i );
                ( *Layers["Fraction Month Frost"] )[lo][la] = min( FrostDays[i] / MonthDays[i], 1.0 );
            }
            double AWC = DataGrid::Get( )->GetGridCell( lo, la )->GetWaterCapacity( 0 );

            tuple<vector<double>, double, double> TempTuple = CVC.MonthlyActualEvapotranspirationSoilMoisture( AWC, Precipitation, Temperature );
            ( *Layers["TotalAET"] )[lo][la] = 0;
            for( int i = 0; i < 12; i++ ) {
                Layers["AET"]->setTime( i );
                ( *Layers["AET"] )[lo][la] = get<0>( TempTuple )[i];
                ( *Layers["TotalAET"] )[lo][la] += get<0>( TempTuple )[i];
            }
            ( *Layers["Fraction Year Fire"] )[lo][la] = ( get<2> ( TempTuple ) / 360 );
        }
    }
}
//----------------------------------------------------------------------------------------------

void Environment::setBreeding( ) {
    // Designate a breeding season for this grid cell, where a month is considered to be part of the breeding season if its NPP is at
    // least 80% of the maximum NPP throughout the whole year
    const unsigned int NumLon = DataGrid::Get( )->GetLengthLongitudeVector( );
    const unsigned int NumLat = DataGrid::Get( )->GetLengthLatitudeVector( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            double maxSeason = -1;
            for( int i = 0; i < 12; i++ ) {
                Layers["Seasonality"]->setTime( i );
                maxSeason = max( maxSeason, ( *Layers["Seasonality"] )[lo][la] );
            }
            for( int i = 0; i < 12; i++ ) {
                Layers["Seasonality"]->setTime( i );
                Layers["Breeding Season"]->setTime( i );

                if( ( *Layers["Seasonality"] )[lo][la] / maxSeason > 0.5 ) {
                    ( *Layers["Breeding Season"] )[lo][la] = 1.0;
                } else {
                    ( *Layers["Breeding Season"] )[lo][la] = 0.0;
                }
            }
        }
    }
}
//------------------------------------------------------------------------------

void Environment::setHANPP( ) {
    const unsigned int NumLon = DataGrid::Get( )->GetLengthLongitudeVector( );
    const unsigned int NumLat = DataGrid::Get( )->GetLengthLatitudeVector( );
    for( int lo = 0; lo < NumLon; lo++ ) {
        for( int la = 0; la < NumLat; la++ ) {
            ( *Layers["HANPP"] )[lo][la] = 0;
        }
    }
}




