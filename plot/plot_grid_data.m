
%% Load Meta Data
OutputParameters = ReadTable( [ optionOutputDirectory optionCurrentDataSet optionOutputParametersFile ], ',' );

numberOfDatums = length( OutputParameters( :, 1 ) );

annualGridIndices = [ ];
monthlyGridIndices = [ ];

counters = zeros( 1, 4 );

for datumIndex = 1:numberOfDatums
    if strcmpi( OutputParameters{ datumIndex, 2 }, 'grid' ) == 1 && strcmpi( OutputParameters{ datumIndex, 3 }, 'year' ) == 1
        counters( 2 ) = counters( 2 ) + 1;
        annualGridIndices( counters( 2 ) ) = datumIndex;
    elseif strcmpi( OutputParameters{ datumIndex, 2 }, 'grid' ) == 1 && strcmpi( OutputParameters{ datumIndex, 3 }, 'month' ) == 1
        counters( 4 ) = counters( 4 ) + 1;
        monthlyGridIndices( counters( 4 ) ) = datumIndex;
    end
end
%% Annual grid plots
if exist( [ optionOutputDirectory optionCurrentDataSet optionAnnualGridFile ], 'file' ) == 2
    for plotIndex = 1:length( annualGridIndices )
        dataSetName = OutputParameters{ annualGridIndices( plotIndex ), 1 };
        try
            dataSet = ncread( [ optionOutputDirectory optionCurrentDataSet optionAnnualGridFile ], dataSetName );
            
        catch
            disp( [ 'ERROR> Unable to plot variable "' OutputParameters{ annualGridIndices( plotIndex ) } '" in ' optionAnnualGridFile '.' ] );
        end
    end
end
%% Annual grid plots
if exist( [ optionOutputDirectory optionCurrentDataSet optionMonthlyGridFile ], 'file' ) == 2
    
    longitude = ncread( [ optionOutputDirectory optionCurrentDataSet optionMonthlyGridFile ], 'lon' );
    latitude = ncread( [ optionOutputDirectory optionCurrentDataSet optionMonthlyGridFile ], 'lat' );
    
    for plotIndex = 1:length( monthlyGridIndices )
        dataSetName = OutputParameters{ monthlyGridIndices( plotIndex ), 1 };
        colourbarLabel = OutputParameters{ monthlyGridIndices( plotIndex ), 4 };
        try
            dataSet = ncread( [ optionOutputDirectory optionCurrentDataSet optionMonthlyGridFile ], dataSetName );
            temporalResolution = size( dataSet, 3 );
            if optionMonthIndex <= temporalResolution
                handle = figure;
                pcolor( longitude, latitude, dataSet( :, :, optionMonthIndex )' ), shading flat;
                c = colorbar;
                ylabel( c, colourbarLabel );
                xlabel( 'Longitude' );
                ylabel( 'Latitude' );
                title( dataSetName );
                
                if optionPrintPlotsToFile == 1
                    disp( [ optionOutputDirectory optionCurrentDataSet dataSetName '.' optionOutputFileFormat  ] );
                    printPlotToFile( handle, [ optionPlotImageWidth optionPlotImageHeight ], [ optionOutputDirectory optionCurrentDataSet dataSetName ], optionOutputFileFormat );
                end
            else
                disp( [ 'ERROR> optionMonthIndex is set to ' num2str( optionMonthIndex ) '. Data only has a temporal resolution of ' num2str( temporalResolution ) '.' ] );
            end
        catch
            disp( [ 'ERROR> Unable to plot variable "' OutputParameters{ monthlyGridIndices( plotIndex ) } '" in ' optionMonthlyGridFile '.' ] );
        end
    end
end