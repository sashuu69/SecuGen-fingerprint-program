#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "sgfplib.h"
#include <mysql/mysql.h>
#include <string>

static char *host = "docker01.tharun.me";
static char *user = "docker";
static char *pass = "docker";
static char *dbname = "exam-marker";

unsigned int port = 3306;

static char *unix_socket = NULL;
unsigned int flag = 0;


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
        printf("FAIL EnableAutoOnEvent - [%ld]\n",result);  
    }
    else {
        StartAutoOn = true; 
    } 
    return StartAutoOn;
}

bool StopAutoOn(LPSGFPM m_sgfplib) {
    DWORD result;
    bool StopAutoOn = false;
    int errorr;

    //////////////////////////////////////////////////////////////////////////
    // EnableAutoOnEvent(false)  
    result = m_sgfplib->EnableAutoOnEvent(false,&msg_qid,NULL);
    if (result != SGFDX_ERROR_NONE) {
        errorr = 1;
        // printf("FAIL EnableAutoOnEvent - [%ld]\n",result);  
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


int main(int argc, char* argv) {
    // Fetching data from MySQL
    MYSQL *conn;
    MYSQL_RES *result;
    MYSQL_ROW row;
    char *studentRegistrationNumber = "TVE17MCA042";
    char *sqlSelectQuery = "SELECT `fingerprint` FROM `students_details` WHERE `university_id` = '";
    char *sqlQueryEnder = "'";

    int finalQuerySize = strlen(sqlSelectQuery) + strlen(studentRegistrationNumber) + strlen(sqlQueryEnder) + 1;
    char *finalQuery = (char *)malloc(finalQuerySize);
    strcpy(finalQuery,sqlSelectQuery);
    strcat(finalQuery, studentRegistrationNumber);
    strcat(finalQuery,sqlQueryEnder);

    conn = mysql_init(NULL); 
    if (!mysql_real_connect(conn, host, user, pass, dbname, port, unix_socket, flag)) {
        fprintf(stderr, "Error: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
        exit(1);
    }

    mysql_query(conn, finalQuery);
    result = mysql_store_result(conn);
    int num_fields = mysql_num_fields(result);
    row = mysql_fetch_row(result);

    printf("%s \n", row);
    printf("%s \n", row[0]);
    std::string str = row[0];
    BYTE* fingerPrintFromDatabase = new BYTE [str.size() + 1];
    printf("%s \n", fingerPrintFromDatabase);


    // Compare the fingerprint
    long err;
    BYTE* imageBuffer1;
    BYTE* minutiaeBuffer1;
    int msg_qid;
    SGDeviceInfoParam deviceInfo;
    DWORD quality_of_image = 0;
    DWORD templateSizeMax;
    SGFingerInfo fingerInfo;
    int errorr;
    BOOL matched;

    err = CreateSGFPMObject(&sgfplib);

    if (!sgfplib) {
        errorr = 2;
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
                        errorr = 3;
                    }
                    else {
                        FILE *fp = fopen("/tmp/exammarker_temp_fp.raw","wb");                        fwrite (imageBuffer1,sizeof (BYTE),deviceInfo.ImageWidth*deviceInfo.ImageHeight, fp);
                        fclose(fp);
                        fp = NULL;
                        err = sgfplib->GetImageQuality(deviceInfo.ImageWidth, deviceInfo.ImageHeight, imageBuffer1, &quality_of_image);
                        if (quality_of_image > 95) {
                            sgfplib->SetLedOn(false);
                            err = sgfplib->SetTemplateFormat(TEMPLATE_FORMAT_SG400);
                            err = sgfplib->GetMaxTemplateSize(&templateSizeMax);
                            minutiaeBuffer1 = (BYTE*) malloc(templateSizeMax);
                            fingerInfo.ImageQuality = quality_of_image;
                            err = sgfplib->CreateTemplate(&fingerInfo, imageBuffer1, minutiaeBuffer1);
                            if (err == SGFDX_ERROR_NONE) {
                                err = sgfplib->MatchTemplate(minutiaeBuffer1,fingerPrintFromDatabase, SL_HIGHEST, &matched);
                                if (matched == true) {	
                                    printf("<<MATCH>>\n");
                                    exit(1);
                                }	
                                else {	
                                    printf("<<NO MATCH>>\n");
                                    exit(1);
                                }
                            }
                        }
                    }
                }
            }
        }

        err = sgfplib->EnableAutoOnEvent(false,&msg_qid,NULL);
        msgctl(msg_qid, IPC_RMID, 0);

        ///////////////////////////////////////////////
        // closeDevice()
        err = sgfplib->CloseDevice();
        ///////////////////////////////////////////////

        ///////////////////////////////////////////////
        // Destroy FPLib object
        err = DestroySGFPMObject(sgfplib);
        ///////////////////////////////////////////////

        free(imageBuffer1);
        imageBuffer1 = NULL;	
    }

    return EXIT_SUCCESS;
}