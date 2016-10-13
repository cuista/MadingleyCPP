tic
%clear

%% User Defined Parameters
optionCurrentDataSet            = '2016-10-13_12-36-38';
optionOutputDirectory           = '/home/philju/Dropbox/Development/MadingleyCPP/output/';

optionOutputParametersFile      = 'OutputControlParameters.csv';
optionAnnualBasicFile           = 'AnnualBasicOutputs.nc';
optionAnnualGridFile            = 'AnnualGridOutputs.nc';
optionMonthlyBasicFile          = 'MonthlyBasicOutputs.nc';
optionMonthlyGridFile           = 'MonthlyGridOutputs.nc';

optionPrintPlotsToFile          = 1; % yes = 1, no = anything else
optionOutputFileFormat          = 'png'; % EPS or PNG
optionPlotImageWidth            = 12; % cm
optionPlotImageHeight           = 10; % cm

optionMissingValue              = -9999;
optionMonthIndex                = 1200; % Time index for plotting grid datums

%% Input Formatting
if strcmp( optionOutputDirectory( end ), '/' ) == 0
    optionOutputDirectory = [ optionOutputDirectory '/' ];
end

if strcmp( optionCurrentDataSet( end ), '/' ) == 0
    optionCurrentDataSet = [ optionCurrentDataSet '/' ];
end

%% Plot Script Execution
if exist( [ optionOutputDirectory optionCurrentDataSet ], 'dir' ) == 7
    %% Plot basic datums
    plot_basic_data
    
    %% Plot grid datums
    plot_grid_data
else
    disp( 'ERROR> Input directories do not exist.' );
end
toc