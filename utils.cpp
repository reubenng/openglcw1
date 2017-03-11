#include "utils.h"

char* filetobuf(char *file){
	FILE *fptr;
	long length;
    char *buf;
    fptr = fopen(file, "r"); // open file
    if(!fptr) cout << "Fail to open " << file << endl;
    fseek(fptr, 0, SEEK_END); // Seek to the end of the file
    length = ftell(fptr); /* Find out how many bytes into the file we are */
    buf = (char*)malloc(length + 1); /* Allocate a buffer for the entire length of the file and a null terminator */
    fseek(fptr, 0, SEEK_SET); /* Go back to the beginning of the file */
    fread(buf, length, 1, fptr); /* Read the contents of the file in to the buffer */
    fclose(fptr); /* Close the file */
    buf[length] = 0; /* Null terminator */
    return buf; /* Return the buffer */
}

void check(char *where){
    const char *what;
    int err = glGetError();
    if(!err) return;
    if(err == GL_INVALID_ENUM) what = "GL_INVALID_ENUM";
    else if(err == GL_INVALID_VALUE) what = "GL_INVALID_VALUE";
    else if(err == GL_INVALID_OPERATION) what = "GL_INVALID_OPERATION";
    else if(err == GL_INVALID_FRAMEBUFFER_OPERATION) what = "GL_INVALID_FRAMEBUFFER_OPERATION";
    else if(err == GL_OUT_OF_MEMORY) what = "GL_OUT_OF_MEMORY";
    else what = "Error Unknown";
    cout << "Error (" << err << ") " << what << " at " << where << endl;
    exit(1);
}

GLuint loadpicture(const char* path){
    unsigned char header[54]; // 54 bytes header
    unsigned char* data; // rgb data
    unsigned int dataPos, imgsize, w, h; // width, height
    GLuint texture;
    // file reading: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
    FILE * file = fopen(path, "rb");
    if (!file)
    {
        cout << path << " not found" << endl;
        exit(0);
    }
    if (fread(header, 1, 54, file) != 54)
    {
        cout << "Not a correct BMP file" << endl;
        return 0;
    }
    if (header[0]!='B' || header[1]!='M')
    {
        cout << "Not a correct BMP file" << endl;
        return 0;
    }
    // Make sure this is a 24bpp file
    if ( *(int*)&(header[0x1E]) != 0  ) {cout << "Not a correct BMP file" << endl;    return 0;}
    if ( *(int*)&(header[0x1C]) != 24 ) {cout << "Not a correct BMP file" << endl;    return 0;}

    // Read the information about the image
    dataPos   = *(int*)&(header[0x0A]);
    imgsize = *(int*)&(header[0x22]);
    w     = *(int*)&(header[0x12]);
    h    = *(int*)&(header[0x16]);
    if (imgsize == 0)    imgsize = w * h * 3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos == 0)      dataPos = 54; // The BMP header is done that way
    data = new unsigned char [imgsize];
    fread(data, 1, imgsize, file);
    fclose (file);

    glGenTextures(1, &texture); // generate
    glBindTexture(GL_TEXTURE_2D, texture); // bind
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    return texture;
}