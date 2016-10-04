function printPlotToFile( plotHandle, imageDimensions, filePath, fileFormat, paintRenderer )

set( plotHandle,'PaperUnits','centimeters', 'PaperSize', imageDimensions, 'PaperPosition', [ 0 0 imageDimensions( 1 ) imageDimensions( 2 ) ] );

if length( ReturnFileNameExtension( filePath ) ) > 0
    outputPath = filePath;
else
    outputPath = [ filePath '.' lower( fileFormat ) ];
end

if strcmpi( fileFormat, 'eps' ) == 1
    if exist( 'paintRenderer', 'var' ) ~= 1
        print( plotHandle, outputPath, '-depsc', '-loose' );
    else
        print( plotHandle, outputPath, '-depsc', '-loose', '-painters' );
    end
elseif strcmpi( fileFormat, 'png' ) == 1
    if exist( 'paintRenderer', 'var' ) ~= 1
        print( plotHandle, outputPath, '-dpng', '-r300' );
    else
        print( plotHandle, outputPath, '-dpng', '-r300', '-painters' );
    end
end
