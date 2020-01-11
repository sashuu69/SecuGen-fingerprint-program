from pysgfplib import *
from ctypes import *
from PIL import Image
import pymysql

fingerPrintImageWidth = 252
fingerPrintImageHeight = 330
constant_sg400_template_size = 400
fingerprintImageSize = (fingerPrintImageWidth, fingerPrintImageHeight)

sgfplib = PYSGFPLib()

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

    # Create png image from raw image
    fetchDataFromRAW = open("/tmp/test1.raw", "rb").read()
    PNGImage = Image.frombytes("L", fingerprintImageSize, fetchDataFromRAW)
    PNGImage.save("/tmp/test1.png")

    sgfplib.SetLedOn(False)

    cMinutiaeBuffer1 = (c_char*constant_sg400_template_size)()
    result = sgfplib.CreateSG400Template(cImageBuffer1, cMinutiaeBuffer1);

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

        def convertToBinaryData(filename):
            # Convert digital data to binary format
            with open(filename, 'rb') as file:
                binaryData = file.read()
                return binaryData
            binaryData.close()

        studFingerPrintTemp = convertToBinaryData("/tmp/test1.min")
        fingerprintInsertQuery = """INSERT INTO `students_details`(`user_id`, `batch`, `roll_no`, `university_id`, `admission_id`, `fingerprint`, `enrolled`) VALUES (%s,%s,%s,%s,%s,%s,%s) """
        insertStudentData = (100, 1, 46, "TVE17MCA042", 171152, studFingerPrintTemp, 100)
        result = cursor.execute(fingerprintInsertQuery, insertStudentData)
        connection.commit()

    except pymysql.Error as error :
        connection.rollback()
        dupError = format(error)
        print("Data insertion failed " + dupError)
        if dupError.find(' Duplicate entry') != -1:
            print("KTU ID already registered")
        else:
            print("Data insertion failed")
    finally:
        #closing database connection.
        cursor.close()
        connection.close()


if "__main__" == __name__:
    initialiseFingerprint()
    