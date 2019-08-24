#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "sgfplib.h"

int   msg_qid;
key_t key;
struct msgbuf qbuf;


LPSGFPM  sgfplib = NULL;

bool StartAutoOn(LPSGFPM m_sgfplib) {
    DWORD result;
    bool StartAutoOn = false;

    key = ftok(".", 'a');

    if((msg_qid = msgget(key, IPC_CREAT|0660)) == -1)
        return false;

    result = m_sgfplib->EnableAutoOnEvent(true,&msg_qid,NULL);
    if (result != SGFDX_ERROR_NONE) {
        printf("FAIL - [%ld]\n",result);  
    }
    else {
        StartAutoOn = true; 
    } 
    return StartAutoOn;
}

bool StopAutoOn(LPSGFPM m_sgfplib) {
    DWORD result;
    bool StopAutoOn = false;

    //////////////////////////////////////////////////////////////////////////
    // EnableAutoOnEvent(false)  
    result = m_sgfplib->EnableAutoOnEvent(false,&msg_qid,NULL);
    if (result != SGFDX_ERROR_NONE) {
        printf("FAIL - [%ld]\n",result);  
    }
    else {
        StopAutoOn = true; 
    }
    //////////////////////////////////////////////////////////////////////////

    msgctl(msg_qid, IPC_RMID, 0);

    return StopAutoOn;
}

long fingerPresent()
{
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
#ifdef _FDU06
 #if defined(_XOPEN_SOURCE)
   #if defined(LINUX3)
           qbuf.mtype = FDU06_MSG;
   #else
           qbuf._mtype = FDU06_MSG;
   #endif
 #else
           qbuf.mtype = FDU06_MSG;
 #endif
#endif
#ifdef _FDU05
 #if defined(_XOPEN_SOURCE)
   #if defined(LINUX3)
           qbuf.mtype = FDU05_MSG;
   #else
           qbuf._mtype = FDU05_MSG;
   #endif
 #else
           qbuf.mtype = FDU05_MSG;
 #endif
#endif
#ifdef _FDU04
 #if defined(_XOPEN_SOURCE)
    #if defined(LINUX3)
           qbuf.mtype = FDU04_MSG;
    #else
           qbuf._mtype = FDU04_MSG;
    #endif
 #else
           qbuf.mtype = FDU04_MSG;
 #endif
#endif
#ifdef _FDU03
 #if defined(_XOPEN_SOURCE)
    #if defined(LINUX3)
            qbuf.mtype = FDU03_MSG;
    #else
            qbuf._mtype = FDU03_MSG;
    #endif
 #else
            qbuf.mtype = FDU03_MSG;
 #endif
#endif

#if defined(_XOPEN_SOURCE)
    #if defined(LINUX3)
	   msgrcv(msg_qid, (struct msgbuf *)&qbuf, MAX_SEND_SIZE, qbuf.mtype, 0);
	   if (strlen(qbuf.mtext) > 0)
	   {
	      fingerPresent= atol(qbuf.mtext);
	   }
    #else
	   msgrcv(msg_qid, (struct msgbuf *)&qbuf, MAX_SEND_SIZE, qbuf.mtype, 0);
	   if (strlen(qbuf.mtext) > 0)
	   {
	      fingerPresent= atol(qbuf.mtext);
	   }
   #endif
#else
   msgrcv(msg_qid, (struct msgbuf *)&qbuf, MAX_SEND_SIZE, qbuf.mtype, 0);
   if (strlen(qbuf.mtext) > 0)
   {
      fingerPresent= atol(qbuf.mtext);
   }
#endif
   return fingerPresent;
}


// ---------------------------------------------------------------- main() ---
int main(int argc, char **argv) {

    long err;
    BYTE* imageBuffer1;
    int   msg_qid;
    SGDeviceInfoParam deviceInfo;
    DWORD quality_of_image = 0;

    err = CreateSGFPMObject(&sgfplib);
    if (!sgfplib) {
        printf("ERROR - Unable to instantiate FPM object\n");
     	return false;
    }

    if (err == SGFDX_ERROR_NONE) {
        ///////////////////////////////////////////////
        // Init()
        err = sgfplib->Init(SG_DEV_FDU07);
        ///////////////////////////////////////////////

        ///////////////////////////////////////////////
        // OpenDevice()
        err = sgfplib->OpenDevice(0);
        ///////////////////////////////////////////////

        ///////////////////////////////////////////////
        // getDeviceInfo()
        err = sgfplib->GetDeviceInfo(&deviceInfo);
        ///////////////////////////////////////////////

 	    imageBuffer1 = (BYTE*) malloc(deviceInfo.ImageWidth*deviceInfo.ImageHeight);
        
        ///////////////////////////////////////////////
        // Turn fingerprint LED ON
        sgfplib->SetLedOn(true);
        ///////////////////////////////////////////////

        if (StartAutoOn(sgfplib)) {
            while (1) {
                if (fingerPresent()) {
	                if (!StopAutoOn(sgfplib)) {
                        break;                
                    }
                    err = sgfplib->GetImage(imageBuffer1);  
                    if (err != SGFDX_ERROR_NONE) {
                        printf("FAIL - [%ld]\n",err);
                    }
                    else {
                        FILE *fp = fopen("/tmp/exammarker_temp_fp.raw","wb");
                        fwrite (imageBuffer1,sizeof (BYTE),deviceInfo.ImageWidth*deviceInfo.ImageHeight, fp);
                        fclose(fp);
                        fp = NULL;
                        err = sgfplib->GetImageQuality(deviceInfo.ImageWidth, deviceInfo.ImageHeight, imageBuffer1, &quality_of_image);
                        if (quality_of_image > 95) {
                            sgfplib->SetLedOn(false);
                            err = sgfplib->SetTemplateFormat(TEMPLATE_FORMAT_SG400);
                        }

                    }
             printf(".............................................................\n");
             printf("Press 'X' to exit, any other key to continue >> ");
             if (getc(stdin) == 'X')
               break;
  	     if(!StartAutoOn(sgfplib))
             {
                 printf("StartAutoOn() returned false.\n");
                 break;
             }
           }
         }
        }

        //////////////////////////////////////////////////////////////////////////
        // EnableAutoOnEvent(false)
        printf("Call sgfplib->EnableAutoOnEvent(false) ... \n");
        err = sgfplib->EnableAutoOnEvent(false,&msg_qid,NULL);
        printf("EnableAutoOnEvent returned : [%ld]\n", err);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        // Remove Message Queue //////////////////////////////////////////////////
        msgctl(msg_qid, IPC_RMID, 0);
        // Remove Message Queue //////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////////
        // closeDevice()
        printf("\nCall fplib->CloseDevice()\n");
        err = sgfplib->CloseDevice();
        printf("CloseDevice returned : [%ld]\n",err);

        ///////////////////////////////////////////////
        // Destroy FPLib object
        printf("\nCall DestroySGFPMObject(fplib)\n");
        err = DestroySGFPMObject(sgfplib);
        printf("DestroySGFPMObject returned : [%ld]\n",err);

        free(imageBuffer1);
        imageBuffer1 = NULL;
		
     }
     return 0;
}
