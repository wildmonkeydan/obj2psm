// obj2psm - A small program to convert .obj files to .psm files to be used on the Sony Playstation, specifically with psn00bsdk
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"

void readMtl(const char* filename); // Reads the .mtl file and loads in simple diffuse materials to be used on untextured polys
void setMatName(const char* line); // Sets the reference name of the material, so when the .obj references it, it can be pushed to an index for use at PS runtime
void setMatDiffuse(const char* line); // Sets the colour of that material, so setRGB0() can be used on the poly tied to the material

MAT materials[255] = { 0,0 };
short indexes[4] = {0,0,0,0};


int main()
{
    FILE* filePointer;
    int bufferLength = 255;
    char buffer[255];
    char filename[255];
    char tempFilename[255];

    printf("Type in the filename without extension: ");
    scanf("%s", filename);


    strcpy(tempFilename, filename);
    strcat(tempFilename, ".obj"); // Get the .obj from the input

    filePointer = fopen(tempFilename, "r");

    if (filePointer == NULL) {
        printf("uh oh");
        exit(EXIT_FAILURE);
    }

    fgets(buffer, bufferLength, filePointer);
    printf("%s", buffer);

    fclose(filePointer);

    strcpy(tempFilename, filename);
    strcat(tempFilename, ".mtl"); // Get the .mtl from the input
    readMtl(tempFilename);
    

    return 0;
}

void readMtl(const char* filename) {
    FILE* mtlFile = fopen(filename,"r");
    int bufferLength = 255;
    char buffer[255];

    if (mtlFile == NULL) {
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, bufferLength, mtlFile)) {
        switch (buffer[0]) { //Check the first letter of each line
        case 'n': //i.e. newmtl
            setMatName(buffer);
            break;
        case 'K': //Looking for diffuse info
            if (buffer[1] == 'd') {
                setMatDiffuse(buffer);
            }
        default:
            continue;
        }
    }

    fclose(mtlFile);
}

void setMatName(const char* line) {
    

}

void setMatDiffuse(const char* line) {

}