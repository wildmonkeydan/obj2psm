// obj2psm - A small program to convert .obj files to .psm files to be used on the Sony Playstation, specifically with psn00bsdk
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "defines.h"

void readMtl(const char* filename); // Reads the .mtl file and loads in simple diffuse materials to be used on untextured polys
void readObj(const char* filename); // Reads the .obj file and loads in poly, UV, and normal data
void setMatName(const char* line); // Sets the reference name of the material, so when the .obj references it, it can be pushed to an index for use at PS runtime
void setMatDiffuse(const char* line); // Sets the colour of that material, so setRGB0() can be used on the poly tied to the material
void setVert(const char* line); // Sets the given coordinates as the vertice in the current index
void setUV(const char* line); // Sets the given texcoords as the next index, rounded to fit into a texpage (256*256 16-bit px)
void setNorm(const char* line); // Sets the given coordinates as the normal of the current index
void setFace(const char* line); // Sets the given face info as the current face index (dependant on wether face is textured)
void setCurrentMat(const char* line); // Sets the given material name to be applied to upcoming untextured faces
void saveModel(const char* filename); // Saves the loaded model as a .psm
void assembleModel(); // Assembles all the data needed in the model
void initModel(); // Initalizes model with a cap of 4,000 polys of each type

MAT materials[255] = { 0,0 };
uint8_t currentMat = 0;
short indexes[6] = {0,0,0,0,0,0};
MODEL model = { 0 };
int fidelity = 4096;

int main()
{
    int bufferLength = 255;
    char buffer[255];
    char filename[255];
    char tempFilename[255];

    initModel();

    printf("Type in the filename without extension: ");
    int e = scanf("%s", filename);


    strcpy(tempFilename, filename);
    strcat(tempFilename, ".mtl"); // Get the .mtl from the input
    readMtl(tempFilename);

    printf(".mtl read success!\n");


    strcpy(tempFilename, filename);
    strcat(tempFilename, ".obj"); // Get the .obj from the input
    readObj(tempFilename);

    printf(".obj read success!\n");



    printf("%d", indexes[2]);

    printf("Type in the filename to save to without extension: ");
    int d = scanf("%s", filename);

    strcpy(tempFilename, filename);
    strcat(tempFilename, ".psm");
    saveModel(tempFilename);

    printf("\nModel Info:\nUntextured tris: %d\nTextured tris: %d\nVerts: %d\nNorms: %d\nUV's: %d", model.h.numUntex, model.h.numTex, model.h.numVerts, model.h.numNorms, model.h.numUV);
    printf("\nFirst Face - x: %d  y: %d  z: %d  -  Norm: %d", model.texFaces[0].v[0], model.texFaces[0].v[1], model.texFaces[0].v[2], model.texFaces[0].n);
    printf("\nSecond Vert - x: %d  y: %d  z: %d", model.vIndex[1].vx, model.vIndex[1].vy, model.vIndex[1].vz);
    printf("\n%d", model.uvIndex[0].u);

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

void readObj(const char* filename) {
    FILE* objFile = fopen(filename, "r");
    int bufferLength = 255;

    char buffer[255];

    if (objFile == NULL) {
        printf("Uh Oh");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, bufferLength, objFile)) {
        switch (buffer[0]) { //Check the first letter of each line
        case 'v': //Looking for vertex-related info
            printf("v");
            if (buffer[1] == ' ') {
                printf("vert");
                setVert(buffer);
            }
            if (buffer[1] == 't') {
                setUV(buffer);
            }
            if (buffer[1] == 'n') {
                setNorm(buffer);
            }
            break;
        case 'f': //Looking for face info
            printf("f");
            setFace(buffer);
            break;
        case 'u': //Looking for material info
            printf("u");
            setCurrentMat(buffer);
        default:
            continue;
        }
    }
    fclose(objFile);
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
    materials[indexes[2]].colour.g = (unsigned char)(rgb[1] * 255);
    materials[indexes[2]].colour.b = (unsigned char)(rgb[2] * 255);
    indexes[2]++;
}

void setVert(const char* line) {
    const char delim[2] = " ";
    char* token;
    float verts[3];
    int conv[3];

    token = strtok(line, delim);
    printf("\n strtok ok");

    for (int i = 0; i < 3; i++) {
        token = strtok(NULL, delim);
        verts[i] = atof(token);
        conv[i] = (int)(verts[i] * fidelity);
    }

    printf("\n for loop ok");
    model.vIndex[indexes[0]] = (VECTOR){ conv[0],conv[1],conv[2] };
    printf("\n model write ok");
    indexes[0]++;
}

void setUV(const char* line) {
    const char delim[2] = " ";
    char* token;
    float raw[2];
    uint8_t conv[2];

    token = strtok(line, delim);

    for (int i = 0; i < 2; i++) {
        token = strtok(NULL, delim);
        raw[i] = atof(token);
        conv[i] = (uint8_t)(raw[i] * 255);
    }
    model.uvIndex[indexes[3]].u = conv[0];
    model.uvIndex[indexes[3]].v = conv[1];
    indexes[3]++;
}

void setNorm(const char* line) {
    const char delim[2] = " ";
    char* token;
    float raw[3];
    int conv[3];

    token = strtok(line, delim);

    for (int i = 0; i < 3; i++) {
        token = strtok(NULL, delim);
        raw[i] = atof(token);
        conv[i] = (int)(raw[i] * 4098);
    }
    model.nIndex[indexes[1]] = (VECTOR){ conv[0],conv[1],conv[2] };
    indexes[1]++;
}

void setFace(const char* line) {
    const char delim[2] = " ";
    const char slash[2] = "/";
    char* token;
    char* infoone;
    char* infotwo;
    char* infothree;
    bool isFlat = false;
    int raw[3];
    FTRI untex;
    FTTRI tex;

    token = strtok(line, delim);

    infoone = strtok(NULL, delim);
    infotwo = strtok(NULL, delim);
    infothree = strtok(NULL, delim);

    printf("\n%s %s %s", infoone, infotwo, infothree);

    token = strtok(infoone, slash);
    raw[0] = atoi(token);
    token = strtok(NULL, slash);
    raw[1] = atoi(token);
    token = strtok(NULL, slash);
    raw[2] = atoi(token);


    printf("  %d %d %d", raw[0], raw[1], raw[2]);
    if (raw[1] == 0) {
        isFlat = true;
    }

    if (isFlat) {
        untex.v[0] = raw[0]--;
        untex.mat = currentMat;
        untex.n = raw[2]--;
    }
    else {
        tex.v[0] = raw[0]--;
        tex.t[0] = raw[1]--;
        tex.n = raw[2]--;
    }



    token = strtok(infotwo, slash);
    raw[0] = atoi(token);
    token = strtok(NULL, slash);
    raw[1] = atoi(token);
    token = strtok(NULL, slash);
    raw[2] = atoi(token);

    if (raw[1] == 0) {
        isFlat = true;
    }

    if (isFlat) {
        untex.v[1] = raw[0]--;
        untex.mat = currentMat;
        untex.n = raw[2]--;
    }
    else {
        tex.v[1] = raw[0]--;
        tex.t[1] = raw[1]--;
        tex.n = raw[2]--;
    }



    token = strtok(infothree, slash);
    raw[0] = atoi(token);
    token = strtok(NULL, slash);
    raw[1] = atoi(token);
    token = strtok(NULL, slash);
    raw[2] = atoi(token);

    if (raw[1] == 0) {
        isFlat = true;
    }

    if (isFlat) {
        untex.v[2] = raw[0]--;
        untex.mat = currentMat;
        untex.n = raw[2]--;
    }
    else {
        tex.v[2] = raw[0]--;
        tex.t[2] = raw[1]--;
        tex.n = raw[2]--;
    }

    if (isFlat) {
        model.untexFaces[indexes[4]] = untex;
        indexes[4]++;
    }
    else {
        model.texFaces[indexes[5]] = tex;
        indexes[5]++;
    }
}

void setCurrentMat(const char* line) {
    const char delim[2] = " ";
    char* token;
    bool done = false;
    int counter = 0;

    token = strtok(line, delim);
    token = strtok(NULL, delim);

    while (done == false) {
        
        if (materials[counter].name == token) {
            currentMat = counter;
            done = true;
        }
        if (counter = 255) {
            done = true;
        }
        counter++;
    }
}

void saveModel(const char* filename) {
    FILE* out = fopen(filename, "wb");
    assembleModel();
    uint32_t numBytes = sizeof(HEADER)
        + (indexes[0] * sizeof(VECTOR))
        + (indexes[1] * sizeof(VECTOR))
        + (indexes[2] * sizeof(COLVECTOR))
        + (indexes[3] * sizeof(UV_COORDS))
        + (indexes[4] * sizeof(FTRI))
        + (indexes[5] * sizeof(FTTRI));
    if (out != NULL) {
        fwrite(&model.h, sizeof(unsigned char), 11, out);
        for (int i = 0; i < model.h.numVerts; i++) {
            fwrite(&model.vIndex[i], sizeof(VECTOR), 1, out);
        }
        for (int i = 0; i < model.h.numNorms; i++) {
            fwrite(&model.nIndex[i], sizeof(VECTOR), 1, out);
        }
        for (int i = 0; i < model.h.numMat; i++) {
            fwrite(&model.matIndex[i], sizeof(COLVECTOR), 1, out);
        }
        for (int i = 0; i < model.h.numUV; i++) {
            fwrite(&model.uvIndex[i], sizeof(UV_COORDS), 1, out);
        }
        for (int i = 0; i < model.h.numUntex; i++) {
            fwrite(&model.untexFaces[i], sizeof(FTRI), 1, out);
        }
        for (int i = 0; i < model.h.numTex; i++) {
            fwrite(&model.texFaces[i], sizeof(FTTRI), 1, out);
        }
    }
    else {
        printf("Output file error\n");
    }
    fclose(out);
}

void assembleModel() {
    model.h = (HEADER){ indexes[4],indexes[5],indexes[0],indexes[1],indexes[3],indexes[2] };
    for (int i = 0; i < model.h.numMat; i++) {
        model.matIndex[i] = materials[i].colour;
    }
}

void initModel() {
    model.vIndex = (VECTOR*)malloc(4096 * sizeof(VECTOR));
    model.nIndex = (VECTOR*)malloc(4096 * sizeof(VECTOR));
    model.matIndex = (COLVECTOR*)malloc(256 * sizeof(COLVECTOR));
    model.uvIndex = (UV_COORDS*)malloc(4096 * sizeof(UV_COORDS));
    model.untexFaces = (FTRI*)malloc(4096 * sizeof(FTRI));
    model.texFaces = (FTTRI*)malloc(4096 * sizeof(FTTRI));
}