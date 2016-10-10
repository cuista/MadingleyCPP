
%% Load Meta Data
OutputParameters = ReadTable( [ optionOutputDirectory optionCurrentDataSet optionOutputParametersFile ], ',' );

numberOfDatums = length( OutputParameters( :, 1 ) );

annualBasicIndices = [ ];
monthlyBasicIndices = [ ];

counters = zeros( 1, 4 );

for datumIndex = 1:numberOfDatums
    if strcmpi( OutputParameters{ datumIndex, 2 }, 'basic' ) == 1 && strcmpi( OutputParameters{ datumIndex, 3 }, 'year' ) == 1
        counters( 1 ) = counters( 1 ) + 1;
        annualBasicIndices( counters( 1 ) ) = datumIndex;
    elseif strcmpi( OutputParameters{ datumIndex, 2 }, 'basic' ) == 1 && strcmpi( OutputParameters{ datumIndex, 3 }, 'month' ) == 1
        counters( 3 ) = counters( 3 ) + 1;
        monthlyBasicIndices( counters( 3 ) ) = datumIndex;
    end
end
%% Annual basic plots
if exist( [ optionOutputDirectory optionCurrentDataSet optionAnnualBasicFile ], 'file' ) == 2
    for plotIndex = 1:length( annualBasicIndices )
        dataSetName = OutputParameters{ annualBasicIndices( plotIndex ), 1 };
        try
            dataSet = ncread( [ optionOutputDirectory optionCurrentDataSet optionAnnualBasicFile ], dataSetName );
            % Plot
        catch
            disp( [ 'ERROR> Unable to plot variable "' OutputParameters{ annualBasicIndices( plotIndex ) } '" in ' optionAnnualBasicFile '.' ] );
        end
    end
end
%% Monthly basic plots
if exist( [ optionOutputDirectory optionCurrentDataSet optionMonthlyBasicFile ], 'file' ) == 2
    for plotIndex = 1:length( monthlyBasicIndices )
        dataSetName = OutputParameters{ monthlyBasicIndices( plotIndex ), 1 };
        try
            dataSet = ncread( [ optionOutputDirectory optionCurrentDataSet optionMonthlyBasicFile ], dataSetName );
            time = ncread( [ optionOutputDirectory optionCurrentDataSet optionMonthlyBasicFile ], 'time' );
            %%
            handle = figure;
            plot( time, dataSet );
            xlabel( 'Time Steps' );
            title( dataSetName );
            
            if optionPrintPlotsToFile == 1
                disp( [ optionOutputDirectory optionCurrentDataSet dataSetName '.' optionOutputFileFormat  ] );
                printPlotToFile( handle, [ optionPlotImageWidth optionPlotImageHeight ], [ optionOutputDirectory optionCurrentDataSet dataSetName ], optionOutputFileFormat );
            end
        catch
            disp( [ 'ERROR> Unable to plot variable "' OutputParameters{ monthlyBasicIndices( plotIndex ) } '" in ' optionMonthlyBasicFile '.' ] );
        end
    end
end