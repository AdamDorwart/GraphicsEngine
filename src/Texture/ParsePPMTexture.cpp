#include "ParsePPMTexture.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "Logger.h"

// Seems to be buggy with test.ppm. Needs more testing

bool ParsePPMTexture(Texture* texture, const char* filename, unsigned int texType, unsigned int side) {
    const int BUFSIZE = 128;
    FILE* fp;
    size_t read;
    unsigned char* rawData;
    char buf[3][BUFSIZE];
    char* retval_fgets;
    size_t retval_sscanf;
    
    //Open the texture file
    if ((fp=fopen((std::string(TEXTURE_PATH) + filename).c_str(), "rb")) == NULL) {
        LogError("Error reading ppm file, could not locate %s\n", (std::string(TEXTURE_PATH) + filename).c_str());
        texture->width = 0;
        texture->height = 0;
        return false;
    }
    
    //Read magic number:
    retval_fgets = fgets(buf[0], BUFSIZE, fp);
    
    //Read width and height:
    do {
        retval_fgets=fgets(buf[0], BUFSIZE, fp);
    } while(buf[0][0] == '#');
    
    //Set the width and height
    retval_sscanf=sscanf(buf[0], "%s %s", buf[1], buf[2]);
    texture->width  = atoi(buf[1]);
    texture->height = atoi(buf[2]);
    
    //Read maxval:
    do {
        retval_fgets=fgets(buf[0], BUFSIZE, fp);
    } while(buf[0][0] == '#');
    
    //Read image data:
    texture->size = texture->width * texture->height * 3;
    texture->data.resize(texture->size);
    read = fread(&(texture->data[0]), texture->size, 1, fp);
    fclose(fp);
    
    //If the read was a failure, error
    if(read != 1) {
        LogError("Error parsing ppm file, incomplete data.\n");
        texture->width = 0;
        texture->height = 0;
        return false;
    }

    texture->dataType = GL_UNSIGNED_BYTE;
    texture->colorType = GL_RGB;

    texture->setup(texType, side);

    return true;
}