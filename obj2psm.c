// obj2psm - A small program to convert .obj files to .psm files to be used on the Sony Playstation, specifically with psn00bsdk
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"

void readMtl(const char* filename); // Reads the .mtl file and loads in simple diffuse materials to be used on untextured polys
void setMatName(const char* line); // Sets the reference name of the material, so when the .obj references it, it can be pushed to an index for use at PS runtime
void setMatDiffuse(const char* line); // Sets the colour of that material, so setRGB0() can be used on the poly tied to the material
void setVert(const char* line); // Sets the given coordinates as the vertice in the current index

MAT materials[255] = { 0,0 };
short indexes[4] = {0,0,0,0};
MODEL model = { 0 };

int main()
{
    FILE* filePointer;
    int bufferLength = 255;
    char buffer[255];
    char filename[255];
    char tempFilename[255];

    printf("Type in the filename without extension: ");
    int e = scanf("%s", filename);

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

    printf("%d", indexes[2]);

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
    const char delim[2] = " ";
    char* token;

    token = strtok(line, delim);
    token = strtok(NULL, delim);
    
    token[strcspn(token, "\n")] = 0;
    strcpy(materials[indexes[2]].name,token);
}

void setMatDiffuse(const char* line) {
    const char delim[2] = " ";
    char* token;
    float rgb[3];

    token = strtok(line, delim);

    for (int i = 0; i < 3; i++) {
        token = strtok(NULL, delim);
        rgb[i] = atof(token);
    }
    
    materials[indexes[2]].colour.r = (unsigned char)(rgb[0] * 255);
    materials[indexes[2]].colour.b = (unsigned char)(rgb[1] * 255);
    materials[indexes[2]].colour.g = (unsigned char)(rgb[2] * 255);
    indexes[2]++;
}

void setVert(const char* line) {

}