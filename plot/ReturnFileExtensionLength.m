function[ fileExtensionLength ] = ReturnFileExtensionLength( fileName )

count = 0;
found = 0;

fileExtensionLength = 0;

for index = length( fileName ):-1:1
    if strcmp( fileName( index ), '.' ) == 1
        found = 1;
        break;
    end
    count = count + 1;
end

if found == 1
	fileExtensionLength = count + 1;
end

