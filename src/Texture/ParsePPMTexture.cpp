#include "ParsePPMTexture.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "Logger.h"

/** Load a ppm file from disk.
 @input filename The location of the PPM file.  If the file is not found, an error message
 will be printed and this function will return 0
 @input width This will be modified to contain the width of the loaded image, or 0 if file not found
 @input height This will be modified to contain the height of the loaded image, or 0 if file not found
 @return Returns the RGB pixel data as interleaved unsigned chars (R0 G0 B0 R1 G1 B1 R2 G2 B2 .... etc) or 0 if an error ocured
 **/
bool ParsePPMTexture(Texture* texture, const char* filename) {
    const int BUFSIZE = 128;
    FILE* fp;
    size_t read;
    unsigned char* rawData;
    char buf[3][BUFSIZE];
    char* retval_fgets;
    size_t retval_sscanf;
    
    //Open the texture file
    if ((fp=fopen(filename, "rb")) == NULL) {
        LogError("Error reading ppm file, could not locate %s\n", filename);
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
    texture->data = (void*) new unsigned char[texture->size];
    read = fread(texture->data, texture->size, 1, fp);
    fclose(fp);
    
    //If the read was a failure, error
    if(read != 1) {
        LogError("Error parsing ppm file, incomplete data.\n");
        delete[] texture->data;
        texture->width = 0;
        texture->height = 0;
        return false;
    }
    
    //Create ID for texture
    glGenTextures(1, &(texture->id));
    
    //Set this texture to be the one we are working with
    glBindTexture(GL_TEXTURE_2D, texture->id);
    
    //Generate the texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture->data);
    
    //Make sure no bytes are padded:
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    //Select GL_MODULATE to mix texture with quad color for shading:
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    //Use bilinear interpolation:
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //And unbind it!
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}