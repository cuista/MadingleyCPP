clear;

fileName = 'LATEST_OUTPUT';
basePath = '/home/philju/Dropbox/Development/MadingleyCPP/output/v35_3/';

xAxisText = 'Time step';
frequencyText = 'Frequency';
concentrationText = 'Kg/km^2';
fileFormat = 'png';
optionPlotImageWidth = 12; % cm
optionPlotImageHeight = 10; % cm

filePath = [ basePath fileName ];

if exist( filePath, 'file' ) == 2
    
    %uiimport( filePath );
    
    A = importdata( filePath );
    
    for datumIndex = 1:size( A.data, 2 )
        if datumIndex == 1
            timeSteps = A.data( :, 1 );
            maxTimeSteps = length( timeSteps );
        else
            dataSet = A.data( :, datumIndex );
            dataSetName = A.textdata{ datumIndex };
            handle = figure;
            plot( timeSteps, dataSet );
            title( dataSetName );
            xlabel( xAxisText );
            xlim( [ 1 maxTimeSteps ] );
            printPlotToFile( handle, [ optionPlotImageWidth optionPlotImageHeight ], [ basePath fileName '_' dataSetName ], fileFormat );
        end
    end
else
    disp( 'You''ve fucked up somewhere...' );
end