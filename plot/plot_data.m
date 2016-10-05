
%% Load Meta Data
OutputParameters = ReadTable( [ optionOutputDirectory optionCurrentDataSet optionOutputParametersFile ], ',' );

numberOfDatums = length( OutputParameters( :, 1 ) );

annualBasicIndices = [ ];
annualGridIndices = [ ];
monthlyBasicIndices = [ ];
monthlyGridIndices = [ ];

counters = zeros( 1, 4 );

for datumIndex = 1:numberOfDatums
    if strcmpi( OutputParameters{ datumIndex, 2 }, 'basic' ) == 1 && strcmpi( OutputParameters{ datumIndex, 3 }, 'year' ) == 1
        counters( 1 ) = counters( 1 ) + 1;
        annualBasicIndices( counters( 1 ) ) = datumIndex;
    elseif strcmpi( OutputParameters{ datumIndex, 2 }, 'grid' ) == 1 && strcmpi( OutputParameters{ datumIndex, 3 }, 'year' ) == 1
        counters( 2 ) = counters( 2 ) + 1;
        annualGridIndices( counters( 2 ) ) = datumIndex;
    elseif strcmpi( OutputParameters{ datumIndex, 2 }, 'basic' ) == 1 && strcmpi( OutputParameters{ datumIndex, 3 }, 'month' ) == 1
        counters( 3 ) = counters( 3 ) + 1;
        monthlyBasicIndices( counters( 3 ) ) = datumIndex;
    elseif strcmpi( OutputParameters{ datumIndex, 2 }, 'grid' ) == 1 && strcmpi( OutputParameters{ datumIndex, 3 }, 'month' ) == 1
        counters( 4 ) = counters( 4 ) + 1;
        monthlyGridIndices( counters( 4 ) ) = datumIndex;
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
            disp( [ 'Variable "' OutputParameters{ annualBasicIndices( plotIndex ) } '" does not exist in ' optionAnnualBasicFile '.' ] );
        end
    end
end
%% Annual grid plots
if exist( [ optionOutputDirectory optionCurrentDataSet optionAnnualGridFile ], 'file' ) == 2
    for plotIndex = 1:length( annualGridIndices )
        dataSetName = OutputParameters{ annualGridIndices( plotIndex ), 1 };
        try
            dataSet = ncread( [ optionOutputDirectory optionCurrentDataSet optionAnnualGridFile ], dataSetName );
            
        catch
            disp( [ 'Variable "' OutputParameters{ annualGridIndices( plotIndex ) } '" does not exist in ' optionAnnualGridFile '.' ] );
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
            disp( [ 'Variable "' OutputParameters{ monthlyBasicIndices( plotIndex ) } '" does not exist in ' optionMonthlyBasicFile '.' ] );
        end
    end
end
%% Annual grid plots
if exist( [ optionOutputDirectory optionCurrentDataSet optionMonthlyGridFile ], 'file' ) == 2
    for plotIndex = 1:length( monthlyGridIndices )
        dataSetName = OutputParameters{ monthlyGridIndices( plotIndex ), 1 };
        try
            dataSet = ncread( [ optionOutputDirectory optionCurrentDataSet optionMonthlyGridFile ], dataSetName );
            
            handle = figure;
            imagesc( dataSet(:, :, 1 ) ), colorbar, shading flat;
            title( dataSetName );
            
            if optionPrintPlotsToFile == 1
                disp( [ optionOutputDirectory optionCurrentDataSet dataSetName '.' optionOutputFileFormat  ] );
                printPlotToFile( handle, [ optionPlotImageWidth optionPlotImageHeight ], [ optionOutputDirectory optionCurrentDataSet dataSetName ], optionOutputFileFormat );
            end
        catch
            disp( [ 'Variable "' OutputParameters{ monthlyGridIndices( plotIndex ) } '" does not exist in ' optionMonthlyGridFile '.' ] );
        end
    end
end