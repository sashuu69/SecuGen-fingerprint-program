#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sgfplib.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

LPSGFPM  sgfplib = NULL;
int   msg_qid;
key_t key;
struct msgbuf qbuf;

bool StartAutoOn(LPSGFPM m_sgfplib) {
  DWORD autoOnStatus;
  bool StartAutoOn = false;

  key = ftok(".", 'a');

  if((msg_qid = msgget(key, IPC_CREAT|0660)) == -1) {
    return false;
  }
  autoOnStatus = m_sgfplib->EnableAutoOnEvent(true,&msg_qid,NULL);  
  if (autoOnStatus != SGFDX_ERROR_NONE) {
    printf("FAIL\n");  
  }
  else {
    StartAutoOn = true;  
  }  
  return StartAutoOn;
}

long fingerpresent() {
    int fingerPresent = 0;

#ifdef _FDU07
 #if defined(_XOPEN_SOURCE)
   #if defined(LINUX3)
           qbuf.mtype = FDU07_MSG;
   #else
           qbuf._mtype = FDU07_MSG;
   #endif
 #else
           qbuf.mtype = FDU07_MSG;
 #endif
#endif

    #if defined(_XOPEN_SOURCE)
    #if defined(LINUX3)
	   msgrcv(msg_qid, (struct msgbuf *)&qbuf, MAX_SEND_SIZE, qbuf.mtype, 0);
	   printf("Type: %ld Text: %s\n", qbuf.mtype, qbuf.mtext);
	   if (strlen(qbuf.mtext) > 0)
	   {
	      fingerPresent= atol(qbuf.mtext);
	   }
    #else
	   msgrcv(msg_qid, (struct msgbuf *)&qbuf, MAX_SEND_SIZE, qbuf.mtype, 0);
    //    if (strcmp(qbuf.mtext,"1") == 0)
    //     printf("Type: %ld Text: %s\n", qbuf.mtype, qbuf.mtext);
	   if (strlen(qbuf.mtext) > 0)
	   {
	      fingerPresent= atol(qbuf.mtext);
	   }
   #endif
#else
   msgrcv(msg_qid, (struct msgbuf *)&qbuf, MAX_SEND_SIZE, qbuf.mtype, 0);
   printf("Type: %ld Text: %s\n", qbuf.mtype, qbuf.mtext);
   if (strlen(qbuf.mtext) > 0)
   {
      fingerPresent= atol(qbuf.mtext);
   }
#endif
   return fingerPresent;
}

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
            err = sgfplib->GetDeviceInfo(&deviceInfo);
            if (err == SGFDX_ERROR_NONE) {
                imageBuffer1 = (BYTE*) malloc(deviceInfo.ImageWidth*deviceInfo.ImageHeight);
                err = sgfplib->SetLedOn(true);
                if (StartAutoOn(sgfplib)) {
                    while (1) {
                        if (fingerpresent ()) {
                            printf("hi");
                            // err = sgfplib->GetImageEx(imageBuffer1,timeout,NULL,quality);
                            // if (err == SGFDX_ERROR_NONE) {
                            //     sprintf(kbBuffer,"%s.raw","/tmp/temp_fingerprint");
                            //     fp = fopen(kbBuffer,"wb");
                            //     fwrite (imageBuffer1 , sizeof (BYTE) , deviceInfo.ImageWidth*deviceInfo.ImageHeight , fp);
                            //     fclose(fp);
                            //     err = sgfplib->SetLedOn(false);
                            //     if (err == SGFDX_ERROR_NONE) {
                            //         err = sgfplib->GetImageQuality(deviceInfo.ImageWidth, deviceInfo.ImageHeight, imageBuffer1, &quality_of_image);
                            //         if (err == SGFDX_ERROR_NONE) {
                            //             if (quality_of_image >95) {
                            //                 err = sgfplib->SetTemplateFormat(TEMPLATE_FORMAT_SG400);
                            //                 if (err == SGFDX_ERROR_NONE) {
                            //                     err = sgfplib->GetMaxTemplateSize(&templateSizeMax);
                            //                     if (err == SGFDX_ERROR_NONE) {
                            //                         minutiaeBuffer1 = (BYTE*) malloc(templateSizeMax);
                            //                         fingerInfo.ImageQuality = quality_of_image;
                            //                         err = sgfplib->CreateTemplate(&fingerInfo, imageBuffer1, minutiaeBuffer1);
                            //                         if (err == SGFDX_ERROR_NONE) {
                            //                             printf("%s",minutiaeBuffer1);
                            //                             DestroySGFPMObject(sgfplib);
                            //                             // system("rm -f /tmp/temp_fingerprint.raw");
                            //                         }
                            //                     }
                            //                 }
                            //             }
                            //         }
                            //     }
                            // }
                        }
                    }
                }   
            }
        }
    }
}