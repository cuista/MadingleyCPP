tic
%clear

%% User Defined Parameters
optionCurrentDataSet            = '2016-10-06_12-12-52';
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

%% Input Formatting
if strcmp( optionOutputDirectory( end ), '/' ) == 0
    optionOutputDirectory = [ optionOutputDirectory '/' ];
end

if strcmp( optionCurrentDataSet( end ), '/' ) == 0
    optionCurrentDataSet = [ optionCurrentDataSet '/' ];
end

%% Plot Script Execution
if exist( [ optionOutputDirectory optionCurrentDataSet ], 'dir' ) == 7
    plot_data
else
    disp( 'ERROR> Input directories do not exist.' );
end
toc