#include "facelib.h"
#include "Common.h"
#include <time.h>
#include <unistd.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

using namespace std;

//FDID: EE96E5C51DE14E3EA22ABA2FED01E5DA
int add_face(long lUserID,std::string FDID,std::string filepath)
{	
    //step 1:NET_DVR_UploadFile_V40
    std::string filename;
    filename = filepath.substr(0,filepath.size()-4);
    printf("%s\n",filename.c_str());
    NET_DVR_FACELIB_COND struInput = {0};
	char fdid[NET_SDK_MAX_FDID_LEN];
    sprintf(fdid,"%s",FDID.c_str());
    printf("%s\n",FDID.c_str());

	struInput.dwSize = sizeof(struInput);
	memcpy(struInput.szFDID,fdid,NET_SDK_MAX_FDID_LEN);
	struInput.byConcurrent = 0;
	struInput.byCover = 1;
	struInput.byCustomFaceLibID = 0;
     
    int uploaHandle = NET_DVR_UploadFile_V40(
        lUserID,IMPORT_DATA_TO_FACELIB,&struInput,sizeof(NET_DVR_FACELIB_COND),NULL,NULL,0);
    
    if(-1 == uploaHandle)
    {
        printf("add_face fail, error:%d.\n", NET_DVR_GetLastError());          
		return HPR_ERROR;
    }    
    printf("NET_DVR_UploadFile_V40\n%d\n", uploaHandle); 
    
    
    NET_DVR_SEND_PARAM_IN   struImageInfo = { 0 };
    
    //图片格式：1- jpg，2- bmp，3- png，4- SWF，5- GIF 
    struImageInfo.byPicType = returnFileType(filepath);
    printf("picType:%d\n", returnFileType(filepath)); 
    
           
    FILE* pFile;
    long length;
    printf("filepath:%s\n", filepath.c_str()); 
    
    pFile = fopen(filepath.c_str(), "rb");
    if (pFile == NULL)
    {
        printf("openFileFailed\n"); 

        return FALSE;
    }
   
    //curpos = ftell(pFile);
    fseek(pFile, 0L, SEEK_END);
    length = ftell(pFile);
    fseek(pFile, 0L, SEEK_SET);
    
    BYTE *pSendPicData = new BYTE[length];
    memset(pSendPicData, 0, length);
    fread(pSendPicData, length, 1, pFile);
    fclose(pFile);

    struImageInfo.pSendData = pSendPicData;
    struImageInfo.dwSendDataLen = length;
    
    printf("%s:%ld,%d\n",filepath.c_str(),length,struImageInfo.dwSendDataLen); 
    filename +=".xml";
    printf("filename:%s\n", filename.c_str()); 

    pFile = fopen(filename.c_str(), "rb");
    if (pFile == NULL)
    {
        return FALSE;
    }
   
    //curpos = ftell(pFile);
    fseek(pFile, 0L, SEEK_END);
    length = ftell(pFile);
    fseek(pFile, 0L, SEEK_SET);
    
    BYTE *pSendAppendData = new BYTE[length];
    memset(pSendAppendData, 0, length);
    fread(pSendAppendData, length, 1, pFile);
    fclose(pFile);
    
    struImageInfo.pSendAppendData = pSendAppendData;
    struImageInfo.dwSendAppendDataLen = length;

    printf("%s:%ld,%d\n",filename.c_str(),length,struImageInfo.dwSendAppendDataLen); 
    
    if (NET_DVR_UploadSend(uploaHandle, &struImageInfo, NULL) < 0)
    {
        delete[] pSendPicData;
        pSendPicData = NULL;
        
        delete[] pSendAppendData;
        pSendAppendData = NULL;
        
        printf("NET_DVR_UploadSend fail, error:%d.\n", NET_DVR_GetLastError()); 
        NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup(); 
		return HPR_ERROR;
    }
     
    DWORD dwProgress = 0;
    int iStatus = -1;
    while (true) {
        iStatus = NET_DVR_GetUploadState(uploaHandle,&dwProgress);
		        
        if (iStatus == 1) {
            printf("上传成功\n");
            // NET_DVR_GetUploadResult(m_lUploadHandle, HCNetSDK.IMPORT_DATA_TO_FACELIB, 12);
            break;
        } else if (iStatus == 2) {
            printf("正在上传:%d\n",dwProgress);
            sleep(1);
        } else if (iStatus == 29) {
            printf("图片未识别到目标\n");
            break;
        } else {
            printf("其他错误：%d\n",iStatus);            
            break;
        }       
    }
    printf("NET_DVR_GetUploadState!\n");
    
    return HPR_OK;
}

int get_FDLib_capabilities(long lUserID)
{	    
    //GET /ISAPI/Intelligent/FDLib/<FDID>

    
    //删除人脸库
    /*
    NET_DVR_XML_CONFIG_INPUT lpInputParam = {0};
    lpInputParam.dwSize = sizeof(lpInputParam);    
    char *requestUrl = "DELETE /ISAPI/Intelligent/FDLib";
    lpInputParam.lpRequestUrl = requestUrl;
    lpInputParam.dwRequestUrlLen = strlen(requestUrl);
    lpInputParam.lpInBuffer = NULL;
    lpInputParam.dwInBufferSize = 0;
    lpInputParam.dwRecvTimeOut = 0;
    lpInputParam.byForceEncrpt = 0;
            
    NET_DVR_XML_CONFIG_OUTPUT lpOutputParam = {0};
    lpOutputParam.dwSize = sizeof(lpOutputParam);
    lpOutputParam.lpOutBuffer = NULL;   
    lpOutputParam.dwOutBufferSize = 0;
    lpOutputParam.dwReturnedXMLSize = 0;
    lpOutputParam.lpStatusBuffer = NULL;
    lpOutputParam.dwStatusSize = 0;
    */
    
    //查询人脸库
    ///*
    NET_DVR_XML_CONFIG_INPUT lpInputParam = {0};
    lpInputParam.dwSize = sizeof(lpInputParam);    
    char requestUrl[] = "GET /ISAPI/Intelligent/FDLib";
    lpInputParam.lpRequestUrl = requestUrl;
    lpInputParam.dwRequestUrlLen = strlen(requestUrl);
    lpInputParam.lpInBuffer = NULL;
    lpInputParam.dwInBufferSize = 0;
    lpInputParam.dwRecvTimeOut = 0;
    lpInputParam.byForceEncrpt = 0;
            
    NET_DVR_XML_CONFIG_OUTPUT lpOutputParam = {0};
    lpOutputParam.dwSize = sizeof(lpOutputParam);
    char *FDLibBaseCfgList = new char[1024*1024];
    lpOutputParam.lpOutBuffer = FDLibBaseCfgList;   
    lpOutputParam.dwOutBufferSize = 1024*1024;
    lpOutputParam.dwReturnedXMLSize = 0;
    lpOutputParam.lpStatusBuffer = NULL;
    lpOutputParam.dwStatusSize = 0;
    //*/
  /*  
    bool ret = NET_DVR_STDXMLConfig(lUserID, &lpInputParam, &lpOutputParam);
    if(!ret)
    {
        printf("NET_DVR_STDXMLConfig fail, error:%d.\n", NET_DVR_GetLastError()); 
        NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup(); 
		return HPR_ERROR;
    }  
     */ 
    printf("FDLibBaseCfgList\n%s\n", FDLibBaseCfgList); 
        	    
    return HPR_OK;
} 

//xml
int xml()
{    
    xmlChar *result = NULL;
    int size = 0;
    
    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");  //定义文档和节点指针
    
    //创建一个root节点，设置其内容和属性，然后加入根结点
    xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST "CreateFDLibList"); 
    xmlNewProp(root_node,BAD_CAST "version",BAD_CAST "2.0");    
    xmlNewProp(root_node,BAD_CAST "xmlns",BAD_CAST "http://www.hikvision.com/ver20/XMLSchema");
    
    xmlDocSetRootElement(doc,root_node);        //设置根节点
          
    //创建son节点
    xmlNodePtr son_node = xmlNewNode(NULL,BAD_CAST "CreateFDLib");
    xmlAddChild(root_node,son_node);
    
    //创建grandson节点
    xmlNodePtr grandson = xmlNewNode(NULL, BAD_CAST "id");
    xmlNodePtr content = xmlNewText(BAD_CAST "1");
    xmlAddChild(grandson,content);
    xmlAddChild(son_node,grandson);
    
    grandson = xmlNewNode(NULL, BAD_CAST "name");
    content = xmlNewText(BAD_CAST "测试");
    xmlAddChild(grandson,content);
    xmlAddChild(son_node,grandson);
    
    grandson = xmlNewNode(NULL, BAD_CAST "thresholdValue");    
    content = xmlNewText(BAD_CAST "70");
    xmlAddChild(grandson,content);
    xmlAddChild(son_node,grandson);
    
    grandson = xmlNewNode(NULL, BAD_CAST "customInfo");
    xmlAddChild(son_node,grandson);
    
    grandson = xmlNewNode(NULL, BAD_CAST "customFaceLibID");
    content = xmlNewText(BAD_CAST "xinke001");
    xmlAddChild(grandson,content);
    xmlAddChild(son_node,grandson);
        
    //存储xml文档
    xmlKeepBlanksDefault(0);
    xmlDocDumpFormatMemoryEnc(doc, &result, &size, "UTF-8", 1);

    int nRel = xmlSaveFile("CreateXml.xml",doc);
    if (nRel != -1)
    {
        printf("一个xml文档被创建，写入%d个字节\n", nRel);
    }
    //释放文档内节点动态申请的内存
    xmlFreeDoc(doc);
    return 1;
}

int testxml()
{    
    //xmlChar *result = NULL;
    int size = 0;
    
    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");  //定义文档和节点指针
    
    //创建一个root节点，设置其内容和属性，然后加入根结点
    xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST "CreateFDLibList"); 
    xmlNewProp(root_node,BAD_CAST "version",BAD_CAST "2.0");    
    xmlNewProp(root_node,BAD_CAST "xmlns",BAD_CAST "http://www.hikvision.com/ver20/XMLSchema");
    
    xmlDocSetRootElement(doc,root_node);        //设置根节点
          
    //创建son节点
    xmlNodePtr son_node = xmlNewNode(NULL,BAD_CAST "CreateFDLib");
    xmlAddChild(root_node,son_node);
    
    //创建grandson节点
    xmlNodePtr grandson = xmlNewNode(NULL, BAD_CAST "id");
    xmlNodePtr content = xmlNewText(BAD_CAST "1");
    xmlAddChild(grandson,content);
    xmlAddChild(son_node,grandson);
    
    grandson = xmlNewNode(NULL, BAD_CAST "name");
    content = xmlNewText(BAD_CAST "测试");
    xmlAddChild(grandson,content);
    xmlAddChild(son_node,grandson);
    
    grandson = xmlNewNode(NULL, BAD_CAST "thresholdValue");    
    content = xmlNewText(BAD_CAST "70");
    xmlAddChild(grandson,content);
    xmlAddChild(son_node,grandson);
    
    grandson = xmlNewNode(NULL, BAD_CAST "customInfo");
    xmlAddChild(son_node,grandson);
    
    grandson = xmlNewNode(NULL, BAD_CAST "customFaceLibID");
    content = xmlNewText(BAD_CAST "xinke001");
    xmlAddChild(grandson,content);
    xmlAddChild(son_node,grandson);
       
    //存储xml文档
    xmlKeepBlanksDefault(0);
    
    xmlBufferPtr buffer = xmlBufferCreate();   
    xmlNodeDump(buffer, doc, root_node, 0, 1);
    
    printf("%s\n", buffer->content);
/*
    xmlDocDumpFormatMemoryEnc(doc, &result, &size, "UTF-8", 1);

    int nRel = xmlSaveFile("CreateXml.xml",doc);
    if (nRel != -1)
    {
        printf("一个xml文档被创建，写入%d个字节\n", nRel);
    }
*/
    //释放文档内节点动态申请的内存
    xmlFreeDoc(doc);

    return 1;
}

//查询指定人脸库人员
//<PID>1CB25CF5D84642139EEFF90FA287678B</PID>
int get_FDLibPics(long lUserID,char *FDID)
{	    
    //GET /ISAPI/Intelligent/FDLib/<FDID>
    char lpInBuffer[1024] = {'\0'};
    int urlsize = snprintf(lpInBuffer, 1024, "<FDSearchDescription version=\"2.0\" xmlns=\"http://www.std-cgi.org/ver20/XMLSchema\"><searchID>%s</searchID><searchResultPosition>0</searchResultPosition><maxResults>50</maxResults><FDID>%s</FDID><startTime>1900-12-12T00:00:00Z</startTime><endTime>2099-12-31T00:00:00Z</endTime></FDSearchDescription>",getTimeStamp(MSEC).c_str(),FDID);
    printf("lpInBuffer:%d,%d\n%s\n", urlsize,(int)strlen(lpInBuffer),lpInBuffer);
 
    //查询人脸库
    ///*
    NET_DVR_XML_CONFIG_INPUT lpInputParam = {0};
    lpInputParam.dwSize = sizeof(lpInputParam);    
    char requestUrl[] = "POST /ISAPI/Intelligent/FDLib/FDSearch";
    lpInputParam.lpRequestUrl = requestUrl;
    lpInputParam.dwRequestUrlLen = strlen(requestUrl);
    lpInputParam.lpInBuffer = lpInBuffer;
    lpInputParam.dwInBufferSize = strlen(lpInBuffer);
    lpInputParam.dwRecvTimeOut = 0;
    lpInputParam.byForceEncrpt = 0;
            
    NET_DVR_XML_CONFIG_OUTPUT lpOutputParam = {0};
    lpOutputParam.dwSize = sizeof(lpOutputParam);
    char *FDLibBaseCfgList = new char[1024*1024];
    lpOutputParam.lpOutBuffer = FDLibBaseCfgList;   
    lpOutputParam.dwOutBufferSize = 1024*1024;
    lpOutputParam.dwReturnedXMLSize = 0;
    lpOutputParam.lpStatusBuffer = NULL;
    lpOutputParam.dwStatusSize = 0;
    //*/
    
    bool ret = NET_DVR_STDXMLConfig(lUserID, &lpInputParam, &lpOutputParam);
    if(!ret)
    {
        printf("NET_DVR_STDXMLConfig fail, error:%d.\n", NET_DVR_GetLastError()); 
        NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup(); 
		return HPR_ERROR;
    }    
    printf("FDLibBaseCfgList\n%s\n", FDLibBaseCfgList); 
        	    
    return HPR_OK;
} 

//人脸库的管理
//新增
int create_FDLib(long lUserID,char *name,char *customFaceLibID)
{	    
    //GET /ISAPI/Intelligent/FDLib/<FDID>
    char lpInBuffer[1024] = {'\0'};
    int urlsize = snprintf(lpInBuffer, 1024, "<CreateFDLibList version=\"2.0\" xmlns=\"http://www.isapi.org/ver20/XMLSchema\"><CreateFDLib><id>1</id><name>%s</name><thresholdValue>70</thresholdValue><customFaceLibID>%s</customFaceLibID></CreateFDLib></CreateFDLibList>", name,customFaceLibID);
    printf("lpInBuffer:%d,%d\n%s\n", urlsize,(int)strlen(lpInBuffer),lpInBuffer); 
 
    //查询人脸库
    ///*
    NET_DVR_XML_CONFIG_INPUT lpInputParam = {0};
    lpInputParam.dwSize = sizeof(lpInputParam);    
    char requestUrl[] = "POST /ISAPI/Intelligent/FDLib";
    lpInputParam.lpRequestUrl = requestUrl;
    lpInputParam.dwRequestUrlLen = strlen(requestUrl);
    lpInputParam.lpInBuffer = lpInBuffer;
    lpInputParam.dwInBufferSize = urlsize;
    lpInputParam.dwRecvTimeOut = 0;
    lpInputParam.byForceEncrpt = 0;
            
    NET_DVR_XML_CONFIG_OUTPUT lpOutputParam = {0};
    lpOutputParam.dwSize = sizeof(lpOutputParam);
    char *FDLibBaseCfgList = new char[1024*1024];
    lpOutputParam.lpOutBuffer = FDLibBaseCfgList;   
    lpOutputParam.dwOutBufferSize = 1024*1024;
    lpOutputParam.dwReturnedXMLSize = 0;
    lpOutputParam.lpStatusBuffer = NULL;
    lpOutputParam.dwStatusSize = 0;
    //*/
    
    bool ret = NET_DVR_STDXMLConfig(lUserID, &lpInputParam, &lpOutputParam);
    if(!ret)
    {
        printf("NET_DVR_STDXMLConfig fail, error:%d.\n", NET_DVR_GetLastError()); 
        NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup(); 
		return HPR_ERROR;
    }    
    printf("FDLibBaseCfgList\n%s\n", FDLibBaseCfgList); 
        	    
    return HPR_OK;
} 

//查询指定人脸库基本信息
int get_FDLib(long lUserID,char *FDID)
{	    
    //GET /ISAPI/Intelligent/FDLib/<FDID>
    char requestUrl[256] = {'\0'};
    int urlsize = snprintf(requestUrl, 256, "GET /ISAPI/Intelligent/FDLib/%s", FDID);
    printf("requestUrl%d,%d\n", urlsize,(int)strlen(requestUrl)); 
 
    //查询人脸库
    ///*
    NET_DVR_XML_CONFIG_INPUT lpInputParam = {0};
    lpInputParam.dwSize = sizeof(lpInputParam);    
    //char *requestUrl = "GET /ISAPI/Intelligent/FDLib";
    lpInputParam.lpRequestUrl = requestUrl;
    lpInputParam.dwRequestUrlLen = strlen(requestUrl);
    lpInputParam.lpInBuffer = NULL;
    lpInputParam.dwInBufferSize = 0;
    lpInputParam.dwRecvTimeOut = 0;
    lpInputParam.byForceEncrpt = 0;
            
    NET_DVR_XML_CONFIG_OUTPUT lpOutputParam = {0};
    lpOutputParam.dwSize = sizeof(lpOutputParam);
    char *FDLibBaseCfgList = new char[1024*1024];
    lpOutputParam.lpOutBuffer = FDLibBaseCfgList;   
    lpOutputParam.dwOutBufferSize = 1024*1024;
    lpOutputParam.dwReturnedXMLSize = 0;
    lpOutputParam.lpStatusBuffer = NULL;
    lpOutputParam.dwStatusSize = 0;
    //*/
    
    bool ret = NET_DVR_STDXMLConfig(lUserID, &lpInputParam, &lpOutputParam);
    if(!ret)
    {
        printf("NET_DVR_STDXMLConfig fail, error:%d.\n", NET_DVR_GetLastError()); 
        NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup(); 
		return HPR_ERROR;
    }    
    printf("FDLibBaseCfgList\n%s\n", FDLibBaseCfgList); 
        	    
    return HPR_OK;
} 

//查询人脸库基本信息列表
int get_FDLibList(long lUserID)
{	   
    //查询人脸库
    ///*
    NET_DVR_XML_CONFIG_INPUT lpInputParam = {0};
    lpInputParam.dwSize = sizeof(lpInputParam);    
    char requestUrl[] = "GET /ISAPI/Intelligent/FDLib";
    lpInputParam.lpRequestUrl = requestUrl;
    lpInputParam.dwRequestUrlLen = strlen(requestUrl);
    lpInputParam.lpInBuffer = NULL;
    lpInputParam.dwInBufferSize = 0;
    lpInputParam.dwRecvTimeOut = 0;
    lpInputParam.byForceEncrpt = 0;
            
    NET_DVR_XML_CONFIG_OUTPUT lpOutputParam = {0};
    lpOutputParam.dwSize = sizeof(lpOutputParam);
    char *FDLibBaseCfgList = new char[1024*1024];
    lpOutputParam.lpOutBuffer = FDLibBaseCfgList;   
    lpOutputParam.dwOutBufferSize = 1024*1024;
    lpOutputParam.dwReturnedXMLSize = 0;
    lpOutputParam.lpStatusBuffer = NULL;
    lpOutputParam.dwStatusSize = 0;
    //*/
    
    bool ret = NET_DVR_STDXMLConfig(lUserID, &lpInputParam, &lpOutputParam);
    if(!ret)
    {
        printf("NET_DVR_STDXMLConfig fail, error:%d.\n", NET_DVR_GetLastError()); 
        NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup(); 
		return HPR_ERROR;
    }    
    printf("get_FDLiblist\n%s\n", FDLibBaseCfgList); 
        	    
    return HPR_OK;
} 



int returnFileType(std::string filepath)
{
    std::string filetype;
    filetype = filepath.substr(filepath.size()-3,filepath.size());
    if(filetype == "jpg" || filetype == "JPG") return JPG;
    if(filetype == "png" || filetype == "PNG") return PNG;
    if(filetype == "bmp" || filetype == "BMP") return BMP;
    if(filetype == "gif" || filetype == "GIF") return GIF;
    if(filetype == "swf" || filetype == "SWF") return SWF;
    if(filetype == "xml" || filetype == "XML") return XML;
}
