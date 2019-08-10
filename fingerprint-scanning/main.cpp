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

    char temp_location_to_save_raw[25];

    if (argv[1]) {
        strcat(temp_location_to_save_raw, "/tmp/");
        strcat(temp_location_to_save_raw, argv[1]);
    }
    else {
        return 0;
    }

    // Instantiate SGFPLib object
    err = CreateSGFPMObject(&sgfplib);
    if (!sgfplib) {
        printf("ERROR - Unable to instantiate FPM object.\n\n");
        return false;
    }

    // Init()
    err = sgfplib->Init(SG_DEV_FDU07);
    if (err != SGFDX_ERROR_NONE) {
        printf("ERROR - Unable to initialize device.\n\n");
        return 0;
    }
    if (err == SGFDX_ERROR_NONE) {

        // OpenDevice()
        err = sgfplib->OpenDevice(0);
        if (err == SGFDX_ERROR_NONE) {
            err = sgfplib->SetLedOn(true);
            err = sgfplib->GetDeviceInfo(&deviceInfo);
            if (err == SGFDX_ERROR_NONE) {
                imageBuffer1 = (BYTE*) malloc(deviceInfo.ImageHeight*deviceInfo.ImageWidth);
                err = sgfplib->GetImageEx(imageBuffer1,timeout,NULL,quality);
                if (err == SGFDX_ERROR_NONE) {
                    sprintf(kbBuffer,"%s.raw",temp_location_to_save_raw);
                    fp = fopen(kbBuffer,"wb");
                    fwrite (imageBuffer1 , sizeof (BYTE) , deviceInfo.ImageWidth*deviceInfo.ImageHeight , fp);
                    fclose(fp);
                    err = sgfplib->SetLedOn(false);
                    if (err == SGFDX_ERROR_NONE) {
                        err = sgfplib->GetImageQuality(deviceInfo.ImageWidth, deviceInfo.ImageHeight, imageBuffer1, &quality_of_image);
                        if (err == SGFDX_ERROR_NONE) {
                            if (quality_of_image >95) {
                                err = sgfplib->SetTemplateFormat(TEMPLATE_FORMAT_SG400);
                                if (err == SGFDX_ERROR_NONE) {
                                    err = sgfplib->GetMaxTemplateSize(&templateSizeMax);
                                    if (err == SGFDX_ERROR_NONE) {
                                        minutiaeBuffer1 = (BYTE*) malloc(templateSizeMax);
                                        fingerInfo.ImageQuality = quality_of_image;
                                        err = sgfplib->CreateTemplate(&fingerInfo, imageBuffer1, minutiaeBuffer1);
                                        if (err == SGFDX_ERROR_NONE) {
                                            printf("%ld",minutiaeBuffer1);
                                            DestroySGFPMObject(sgfplib);
                                            // system("rm -rf /tmp/*");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}