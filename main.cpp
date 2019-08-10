#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sgfplib.h"

LPSGFPM  sgfplib = NULL;

int main(int argc, char **argv) {
    long err;
    DWORD templateSize, templateSizeMax;
    char kbBuffer[100];
    int fingerLength = 0;
    char *finger;

    BYTE *imageBuffer1;
    BYTE *minutiaeBuffer1;

    FILE *fp = NULL;

    SGFingerInfo fingerInfo;
    SGDeviceInfoParam deviceInfo;

    DWORD timeout = 10000;
    DWORD quality = 85;
    DWORD quality_of_image = 0;

    
}