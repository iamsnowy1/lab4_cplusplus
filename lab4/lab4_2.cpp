#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <iostream>

void readLargeFile(const char* inputFile, const char* outputFile) {
    FILE* inFile = fopen(inputFile, "rb");
    FILE* outFile = fopen(outputFile, "wb");

    if (!inFile || !outFile) {
        std::cerr << "Error opening files.\n";
        return;
    }

    char buffer[4096];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), inFile)) > 0) {
        fwrite(buffer, 1, bytesRead, outFile);
    }

    fclose(inFile);
    fclose(outFile);
}
