#include "alarm.h"
#include "base64.h"

BOOL CALLBACK MessageCallback(LONG lCommand, NET_DVR_ALARMER *pAlarmer, char *pAlarmInfo, DWORD dwBufLen, void* pUser)
{
    Json::Value upload_data;
    Json::StyledWriter writer;

    std::cout<<"报警："<<lCommand<<std::endl;
    switch(lCommand) 
    {       
        
        case COMM_SNAP_MATCH_ALARM: //人脸比对结果信息
        {
            printf("\n人脸比对报警\n");
            char chTime[128];
            NET_VCA_FACESNAP_MATCH_ALARM struFaceMatchAlarm = {0};
            memcpy(&struFaceMatchAlarm, pAlarmInfo, sizeof(NET_VCA_FACESNAP_MATCH_ALARM));
            
            NET_DVR_TIME struAbsTime = {0};
            struAbsTime.dwYear = GET_YEAR(struFaceMatchAlarm.struSnapInfo.dwAbsTime);
            struAbsTime.dwMonth = GET_MONTH(struFaceMatchAlarm.struSnapInfo.dwAbsTime);
            struAbsTime.dwDay = GET_DAY(struFaceMatchAlarm.struSnapInfo.dwAbsTime);
            struAbsTime.dwHour = GET_HOUR(struFaceMatchAlarm.struSnapInfo.dwAbsTime);
            struAbsTime.dwMinute = GET_MINUTE(struFaceMatchAlarm.struSnapInfo.dwAbsTime);
            struAbsTime.dwSecond = GET_SECOND(struFaceMatchAlarm.struSnapInfo.dwAbsTime);
            sprintf(chTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",struAbsTime.dwYear,struAbsTime.dwMonth,struAbsTime.dwDay,struAbsTime.dwHour,struAbsTime.dwMinute,struAbsTime.dwSecond);

            printf("人脸比对结果上传[0x%x]：[%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d]fSimilarity[%f]FaceID[%d]BlackListID[%d]"
            "Sex[%d]Glasses[%d]Age[%d]MatchPicNum[%d]PicTransType[%d]\n", lCommand, 
            struAbsTime.dwYear, struAbsTime.dwMonth, struAbsTime.dwDay, struAbsTime.dwHour, struAbsTime.dwMinute, 
            struAbsTime.dwSecond, struFaceMatchAlarm.fSimilarity, struFaceMatchAlarm.struSnapInfo.dwSnapFacePicID,
            struFaceMatchAlarm.struBlackListInfo.struBlackListInfo.dwRegisterID, struFaceMatchAlarm.struSnapInfo.bySex,
            struFaceMatchAlarm.struSnapInfo.byGlasses, struFaceMatchAlarm.struSnapInfo.byAge, 
            struFaceMatchAlarm.byMatchPicNum, struFaceMatchAlarm.byPicTransType);
			char *face_name = (char*)struFaceMatchAlarm.struBlackListInfo.struBlackListInfo.struAttribute.byName;
			char buffer[255];
			int c=GBK_TO_UTF8(face_name,strlen(face_name),buffer,strlen(face_name)+2);
            char * face_buffer = new char[struFaceMatchAlarm.struSnapInfo.dwSnapFacePicLen];
            std::cout<<"长度："<<struFaceMatchAlarm.struSnapInfo.dwSnapFacePicLen<<std::endl;
            memcpy(face_buffer,struFaceMatchAlarm.struSnapInfo.pBuffer1,struFaceMatchAlarm.struSnapInfo.dwSnapFacePicLen);
            std::ofstream f;
            f.open("./Image/face.txt");
            f<<face_buffer;
            f.close();


			//face_name.assign(face_name2.begin(),face_name2.end());
			//printf("姓名：%s\r\n",struFaceMatchAlarm.struBlackListInfo.struBlackListInfo.struAttribute.byName);
			if(!c)printf("姓名：%s\r\n",buffer);
			//std::cout<<"strlen:"<<struFaceMatchAlarm.dwSnapPicLen<<std::endl<<"buffer:"<<struFaceMatchAlarm.pSnapPicBuffer<<std::endl<<"type:"<<struFaceMatchAlarm.byPicTransType<<std::endl;
            
            std::string str=ReturnFileName(struFaceMatchAlarm.pSnapPicBuffer);
           // cout<<"str:"<<str<<endl;
            NET_DVR_GetPicture(pAlarmer->lUserID,(char *)str.data(),(char*)"./Image/test.jpeg");
            //cout<<"x:"<<struFaceMatchAlarm.struRegion.fX<<endl<<"y:"<<struFaceMatchAlarm.struRegion.fY<<endl<<"width:"<<struFaceMatchAlarm.struRegion.fWidth<<endl<<"height:"<<struFaceMatchAlarm.struRegion.fHeight<<endl;
            upload_data["name"] = buffer;
            uploadAlarm("device.face",upload_data);
        }
        break;
        
         case COMM_VCA_ALARM: //智能检测通用报警
        {
            printf("\n智能检测通用报警\n");

            if(pAlarmInfo == NULL)
            {
                return FALSE;                
            }
           // printf("智能检测通用报警, Json数据内容：%s\n", pAlarmInfo);
            Json::Value jsdata;
            // if(!reader.parse(pAlarmInfo,jsdata,false)){return FALSE;}
            // std::string strdata = writer.write(jsdata);
            // printf("\njsdata:%s\n",strdata.c_str());





        return TRUE;

        }
        break;


        case COMM_UPLOAD_PLATE_RESULT:
        {
            printf("看，是车牌\n");
        }
        break;

        case COMM_ITS_PLATE_RESULT:
        {
            printf("看，是车辆\n");
        }
        break;

        case COMM_UPLOAD_FACESNAP_RESULT: //人脸抓拍报警信息
        {
            printf("\n人脸抓拍报警信息\n");
            //upload_data["device_id"] = device_info.device_id;


            NET_VCA_FACESNAP_RESULT struFaceSnap = {0};
            memcpy(&struFaceSnap, pAlarmInfo, sizeof(NET_VCA_FACESNAP_RESULT));
            NET_DVR_TIME struAbsTime = {0};
            struAbsTime.dwYear = GET_YEAR(struFaceSnap.dwAbsTime);
            struAbsTime.dwMonth = GET_MONTH(struFaceSnap.dwAbsTime);
            struAbsTime.dwDay = GET_DAY(struFaceSnap.dwAbsTime);
            struAbsTime.dwHour = GET_HOUR(struFaceSnap.dwAbsTime);
            struAbsTime.dwMinute = GET_MINUTE(struFaceSnap.dwAbsTime);
            struAbsTime.dwSecond = GET_SECOND(struFaceSnap.dwAbsTime);
            //保存抓拍场景图片
            if (struFaceSnap.dwBackgroundPicLen > 0 && struFaceSnap.pBuffer2 != NULL)
            {      
                
                char cFilename[256] = {0};
                char chTime[128];
                sprintf(chTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",struAbsTime.dwYear,struAbsTime.dwMonth,struAbsTime.dwDay,struAbsTime.dwHour,struAbsTime.dwMinute,struAbsTime.dwSecond);

                sprintf(cFilename, "FaceSnapBackPic[%s][%s].jpg",struFaceSnap.struDevInfo.struDevIP.sIpV4, chTime);
                char * buffer = new char[struFaceSnap.dwFacePicLen];

                memcpy(buffer,struFaceSnap.pBuffer1,struFaceSnap.dwFacePicLen);
                std::string imgdata = base64_encode(buffer,struFaceSnap.dwFacePicLen);

                char * backbuffer = new char[struFaceSnap.dwBackgroundPicLen];
                memcpy(backbuffer,struFaceSnap.pBuffer2,struFaceSnap.dwBackgroundPicLen);
                std::string backImgdata = base64_encode(backbuffer,struFaceSnap.dwBackgroundPicLen);
                char * rect = new char[24];
                sprintf(rect,"%1.3f,%1.3f,%1.3f,%1.3f",struFaceSnap.struRect.fX,struFaceSnap.struRect.fY,struFaceSnap.struRect.fHeight,struFaceSnap.struRect.fWidth);
               // rect[23]= '\0';
                std::string face_rect(rect);
                upload_data["image_face"] = imgdata;
                upload_data["image_background"] = backImgdata;
                upload_data["device_id"] = device_info.device_id;
                upload_data["record_time"] = chTime;
                upload_data["upload_time"] =getLocalTime(MSEC);
                upload_data["face_rect"] = face_rect;
               // uploadAlarm("device.faceinfo",upload_data);
               delete []buffer;
               delete []backbuffer;
               delete []rect;
        

            }

            printf("人脸抓拍报警[0x%x]: Abs[%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d] Dev[ip:%s,port:%d,ivmsChan:%d] \n",\
                lCommand, struAbsTime.dwYear, struAbsTime.dwMonth, struAbsTime.dwDay, struAbsTime.dwHour, \
                struAbsTime.dwMinute, struAbsTime.dwSecond, struFaceSnap.struDevInfo.struDevIP.sIpV4, \
                struFaceSnap.struDevInfo.wPort, struFaceSnap.struDevInfo.byIvmsChannel);
            printf("人脸图id：%d \r\n人脸图片传输方式：%d \r\n",struFaceSnap.dwFacePicID,struFaceSnap.byUploadEventDataType);
            printf("人脸区域：（%f,%f,%f,%f) \n",struFaceSnap.struRect.fX,struFaceSnap.struRect.fY,struFaceSnap.struRect.fHeight,struFaceSnap.struRect.fWidth);

        }
        break;
        default:
        printf("其他报警，报警信息类型: 0x%x\n", lCommand);
        break;
    }

    return TRUE;
}

/**
 * @brief   上传报警信息
 * @author  liuguang
 * @date    2021/05/31 
 * @param   [in] name ：上传信息类型
 * @param   [in] data ：上传信息内容
 * @return  [int] 0为失败，1为成功
 */
int uploadAlarm(std::string name,Json::Value data)
{
    RAMQ upload_ramq;
    Json::Value config = getConfig();
    upload_ramq.setconfig(config[1]);
    Json::Value message;
    Json::Value header;
    Json::Value payload;
    Json::StyledWriter writer;
    header["namespace"] = "xinke.device.upload";
    header["name"] = name;
    header["messageId"] = getLocalTime();
    payload = data;
    message["header"]=header;
    message["payload"]=(payload);
    std::string message_data = writer.write(message);
    int status = upload_ramq.publish(message_data);
    return status;
}
