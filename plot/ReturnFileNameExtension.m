function[ fileNameWithoutExtension ] = ReturnFileNameExtension( fileName )

fileNameWithoutExtension = fileName( length( fileName ) - ReturnFileExtensionLength( fileName ) + 2:end );
