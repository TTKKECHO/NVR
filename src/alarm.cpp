#include "alarm.h"
#include "base64.h"

BOOL CALLBACK MessageCallback(LONG lCommand, NET_DVR_ALARMER *pAlarmer, char *pAlarmInfo, DWORD dwBufLen, void* pUser)
{




        std::string sdata;
		std::cout<<"发送："<<std::endl;
		std::cin>>sdata;
		amqp_bytes_t message_bytes = amqp_cstring_bytes(sdata.c_str());

		amqp_basic_properties_t properties;
		printf("\nflag:3\n");

		properties._flags = 0;

		properties._flags |= AMQP_BASIC_DELIVERY_MODE_FLAG;
		properties.delivery_mode = AMQP_DELIVERY_NONPERSISTENT;
	    amqp_basic_publish(state,2,amqp_cstring_bytes("topicExchange"),amqp_cstring_bytes("world"),1,0,&properties,message_bytes);

    std::cout<<"报警："<<lCommand<<std::endl;
    switch(lCommand) 
    {       
        
        case COMM_SNAP_MATCH_ALARM: //人脸比对结果信息
        {
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
			//face_name.assign(face_name2.begin(),face_name2.end());
			//printf("姓名：%s\r\n",struFaceMatchAlarm.struBlackListInfo.struBlackListInfo.struAttribute.byName);
			if(!c)printf("姓名1：%s\r\n",buffer);
			//std::cout<<"strlen:"<<struFaceMatchAlarm.dwSnapPicLen<<std::endl<<"buffer:"<<struFaceMatchAlarm.pSnapPicBuffer<<std::endl<<"type:"<<struFaceMatchAlarm.byPicTransType<<std::endl;
            
            std::string str=ReturnFileName(struFaceMatchAlarm.pSnapPicBuffer);
           // cout<<"str:"<<str<<endl;
            NET_DVR_GetPicture(pAlarmer->lUserID,(char *)str.data(),(char*)"./Image/test.jpeg");
            //cout<<"x:"<<struFaceMatchAlarm.struRegion.fX<<endl<<"y:"<<struFaceMatchAlarm.struRegion.fY<<endl<<"width:"<<struFaceMatchAlarm.struRegion.fWidth<<endl<<"height:"<<struFaceMatchAlarm.struRegion.fHeight<<endl;
            
        }
        break;
        
         case COMM_VCA_ALARM: //智能检测通用报警
        {
            if(pAlarmInfo == NULL)
            {
                return FALSE;                
            }
            printf("智能检测通用报警, Json数据内容：%s\n", pAlarmInfo);
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
            NET_VCA_FACESNAP_RESULT struFaceSnap = {0};
            memcpy(&struFaceSnap, pAlarmInfo, sizeof(NET_VCA_FACESNAP_RESULT));
            NET_DVR_TIME struAbsTime = {0};
            struAbsTime.dwYear = GET_YEAR(struFaceSnap.dwAbsTime);
            struAbsTime.dwMonth = GET_MONTH(struFaceSnap.dwAbsTime);
            struAbsTime.dwDay = GET_DAY(struFaceSnap.dwAbsTime);
            struAbsTime.dwHour = GET_HOUR(struFaceSnap.dwAbsTime);
            struAbsTime.dwMinute = GET_MINUTE(struFaceSnap.dwAbsTime);
            struAbsTime.dwSecond = GET_SECOND(struFaceSnap.dwAbsTime);
            std::cout<<"here"<<std::endl;
            //保存抓拍场景图片
            if (struFaceSnap.dwBackgroundPicLen > 0 && struFaceSnap.pBuffer2 != NULL)
            {      
                
                char cFilename[256] = {0};
                char chTime[128];
                sprintf(chTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",struAbsTime.dwYear,struAbsTime.dwMonth,struAbsTime.dwDay,struAbsTime.dwHour,struAbsTime.dwMinute,struAbsTime.dwSecond);
                
                sprintf(cFilename, "FaceSnapBackPic[%s][%s].jpg",struFaceSnap.struDevInfo.struDevIP.sIpV4, chTime);
                std::fstream imgFile;
                imgFile.open("Image/test2.jpg",std::ios::out|std::ios::binary);
                char * buffer = new char[struFaceSnap.dwFacePicLen];
                memcpy(buffer,struFaceSnap.pBuffer1,struFaceSnap.dwFacePicLen);
                imgFile.write(buffer,struFaceSnap.dwFacePicLen);
                imgFile.close();
                std::string imgdata;
                imgdata = base64_encode(buffer,struFaceSnap.dwFacePicLen);
                
                //发送数据
                HLC request;
                request.url = "https://www.liuguang.space:10086/post";
                request.data="{\"image\":\""+imgdata+"\"}";
                request.headers("Content-Type:application/json");
                request.post();
                printf("response:%s\n",request.response.c_str());

                imgFile.open("Image/test.jpg",std::ios::out|std::ios::binary);
                char * buffer2 = new char[struFaceSnap.dwBackgroundPicLen];
                memcpy(buffer2,struFaceSnap.pBuffer2,struFaceSnap.dwBackgroundPicLen);
                imgFile.write(buffer2,struFaceSnap.dwBackgroundPicLen);
                imgFile.close();
        

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