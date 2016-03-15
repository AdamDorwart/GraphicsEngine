/* Credit goes to this StackOverflow
 * http://stackoverflow.com/questions/7046270/loading-a-tga-file-and-using-it-with-opengl
 */

#include "ParseTGATexture.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include "Logger.h"

typedef struct
{
    unsigned char imageTypeCode;
    short int imageWidth;
    short int imageHeight;
    unsigned char bitCount;
    std::vector<unsigned char> imageData;
} TGAFILE;

bool ParseTGATexture(Texture* texture, const char* filename, unsigned int texType, unsigned int side) {
    FILE *filePtr;
    unsigned char ucharBad;
    short int sintBad;
    long imageSize;
    int colorMode;
    unsigned char colorSwap;

    TGAFILE tgaFile;

    // Open the TGA file.
    filePtr = fopen((std::string(TEXTURE_PATH) + filename).c_str(), "rb");
    if (filePtr == NULL) {
        LogError("Error reading tga file, could not locate %s\n", (std::string(TEXTURE_PATH) + filename).c_str());
        return false;
    }

    // Read the two first bytes we don't need.
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);

    // Which type of image gets stored in imageTypeCode.
    fread(&tgaFile.imageTypeCode, sizeof(unsigned char), 1, filePtr);

    // For our purposes, the type code should be 2 (uncompressed RGB image)
    // or 3 (uncompressed black-and-white images).
    if (tgaFile.imageTypeCode != 2 && tgaFile.imageTypeCode != 3) {
        LogError("Error parsing tga texture. Unsupported format.\n");
        fclose(filePtr);
        return false;
    }

    // Read 13 bytes of data we don't need.
    fread(&sintBad, sizeof(short int), 1, filePtr);
    fread(&sintBad, sizeof(short int), 1, filePtr);
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
    fread(&sintBad, sizeof(short int), 1, filePtr);
    fread(&sintBad, sizeof(short int), 1, filePtr);

    // Read the image's width and height.
    fread(&tgaFile.imageWidth, sizeof(short int), 1, filePtr);
    fread(&tgaFile.imageHeight, sizeof(short int), 1, filePtr);

    // Read the bit depth.
    fread(&tgaFile.bitCount, sizeof(unsigned char), 1, filePtr);

    // Read one byte of data we don't need.
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);

    // Color mode -> 3 = BGR, 4 = BGRA.
    colorMode = tgaFile.bitCount / 8;
    imageSize = tgaFile.imageWidth * tgaFile.imageHeight * colorMode;

    // Allocate memory for the image data.
    tgaFile.imageData.resize(imageSize);

    // Read the image data.
    fread(&(tgaFile.imageData[0]), sizeof(unsigned char), imageSize, filePtr);

    // Change from BGR to RGB so OpenGL can read the image data.
    for (int imageIdx = 0; imageIdx < imageSize - 2; imageIdx += colorMode) {
        colorSwap = tgaFile.imageData[imageIdx];
        tgaFile.imageData[imageIdx] = tgaFile.imageData[imageIdx + 2];
        tgaFile.imageData[imageIdx + 2] = colorSwap;
    }

    fclose(filePtr);

    texture->width = tgaFile.imageWidth;
    texture->height = tgaFile.imageHeight;
    texture->size = imageSize;
    texture->data = tgaFile.imageData;
    texture->dataType = GL_UNSIGNED_BYTE;
    if (colorMode == 3) { //RGB
        texture->colorType = GL_RGB;
    } else if (colorMode == 4) { //RGBA
        texture->colorType = GL_RGBA;
    } else if (colorMode == 1) { // Monochrome
        texture->colorType = GL_RED;
    } else {
        LogError("Error parsing tga texture. Unsupported color mode %d.\n", colorMode);
        return false;
    }
    
    texture->setup(texType, side);
    return true;
}