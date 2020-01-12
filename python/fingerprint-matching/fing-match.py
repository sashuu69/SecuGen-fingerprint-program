from pysgfplib import *
from ctypes import *
import pymysql


fingerPrintImageWidth = 252
fingerPrintImageHeight = 330
constant_sg400_template_size = 400


def initialiseFingerprint():
    result = sgfplib.Create()
    if (result != SGFDxErrorCode.SGFDX_ERROR_NONE):
        print(1) # Failed to open Secugen library
        exit()
    
    result = sgfplib.Init(SGFDxDeviceName.SG_DEV_FDU07) 
    if (result != SGFDxErrorCode.SGFDX_ERROR_NONE):
        print(2) # Unable to initialize SecuGen library
        exit()

    result = sgfplib.OpenDevice(0)

    if (result != SGFDxErrorCode.SGFDX_ERROR_NONE):
        print(3) # Failed to open device
        exit()
    
    scanFingerPrint()


def scanFingerPrint():    
    sgfplib.SetLedOn(True) # Turn fingerprint on

    # Create fingerprint Image
    cImageBuffer1 = (c_char*fingerPrintImageWidth*fingerPrintImageHeight)()
    result = sgfplib.GetImage(cImageBuffer1)

    if (result != SGFDxErrorCode.SGFDX_ERROR_NONE):
        print(4) # Unable to capture first image
        sgfplib.SetLedOn(False)
        exit()

    # Create raw image from Secugen fingerprint sensor
    image1File = open ("/tmp/test1.raw", "wb")    
    image1File.write(cImageBuffer1)
    image1File.close()

    # Check fingerprint image quality
    cQuality = c_int(0)
    result = sgfplib.GetImageQuality(fingerPrintImageWidth, fingerPrintImageHeight, cImageBuffer1, byref(cQuality))
    if (result != 0):
        print(5) # Failed to get image quality
        exit()
    
    # If image quality less than 90
    if (cQuality.value <= 90):
        print(6) # Image quality less than 90
        exit()

    sgfplib.SetLedOn(False)

    cMinutiaeBuffer1 = (c_char*constant_sg400_template_size)()
    result = sgfplib.CreateSG400Template(cImageBuffer1, cMinutiaeBuffer1)

    if (result == SGFDxErrorCode.SGFDX_ERROR_NONE):
        minutiae1File = open ("/tmp/test1.min", "wb")
        minutiae1File.write(cMinutiaeBuffer1)
        minutiae1File.close()
    else:
        print(7) # Unable to create first template
        exit()
    
    try:
        connection = pymysql.connect(host='docker01.tharun.me', database='exam-marker', user='docker', password='docker')
        cursor = connection.cursor()
        cursor.execute("""SELECT fingerprint FROM students_details where university_id = "TVE17MCA042" """)

        record = cursor.fetchall()

        def writeTofile(data, filename):
            with open(filename, 'wb') as file:
                file.write(data)
        
        for row in record:
            fingerPrintTemplate = row[0]
            writeTofile(fingerPrintTemplate, "/tmp/test2.min")

        minFromDatabaseFile = open ("/tmp/test2.min", "rb")
        cMinutiaeBuffer2 = (c_char*constant_sg400_template_size)()
        cMinutiaeBuffer2 = minFromDatabaseFile.read()
        
        # Match template
        cMatched = c_bool(False)
        result = sgfplib.MatchTemplate(cMinutiaeBuffer1, cMinutiaeBuffer2, SGFDxSecurityLevel.SL_HIGHEST, byref(cMatched))
        if result != 0:
            print(9) # Matching error
            exit()
        if cMatched.value == True:
            print(10) # Matched
        else:
            print(11) # No Match

    except pymysql.Error as error :
        connection.rollback()
        dupError = format(error)
        print(8) # Data retrival failed

    finally:
        #closing database connection.
        cursor.close()
        connection.close()


if "__main__" == __name__:
    sgfplib = PYSGFPLib()
    initialiseFingerprint()
    