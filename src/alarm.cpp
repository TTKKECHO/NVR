#include "alarm.h"
#include "base64.h"

BOOL CALLBACK MessageCallback(LONG lCommand, NET_DVR_ALARMER *pAlarmer, char *pAlarmInfo, DWORD dwBufLen, void* pUser)
{
    Json::Value upload_data;        //声明json变量，用于存储报警上传信息
    Json::StyledWriter writer;      //json写入器
    std::cout<<"报警："<<lCommand<<std::endl;       //打印报警类型编号

    //报警信息处理
    switch(lCommand) 
    {       
        //人脸比对
        case COMM_SNAP_MATCH_ALARM: //人脸比对结果信息
        {
            printf("\n人脸比对报警\n");
            char chTime[128];
            //获取报警信息
            NET_VCA_FACESNAP_MATCH_ALARM struFaceMatchAlarm = {0};
            memcpy(&struFaceMatchAlarm, pAlarmInfo, sizeof(NET_VCA_FACESNAP_MATCH_ALARM));
            //获取设备报警时间
            NET_DVR_TIME struAbsTime = {0};
            struAbsTime.dwYear = GET_YEAR(struFaceMatchAlarm.struSnapInfo.dwAbsTime);
            struAbsTime.dwMonth = GET_MONTH(struFaceMatchAlarm.struSnapInfo.dwAbsTime);
            struAbsTime.dwDay = GET_DAY(struFaceMatchAlarm.struSnapInfo.dwAbsTime);
            struAbsTime.dwHour = GET_HOUR(struFaceMatchAlarm.struSnapInfo.dwAbsTime);
            struAbsTime.dwMinute = GET_MINUTE(struFaceMatchAlarm.struSnapInfo.dwAbsTime);
            struAbsTime.dwSecond = GET_SECOND(struFaceMatchAlarm.struSnapInfo.dwAbsTime);
            sprintf(chTime,"%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",struAbsTime.dwYear,struAbsTime.dwMonth,struAbsTime.dwDay,struAbsTime.dwHour,struAbsTime.dwMinute,struAbsTime.dwSecond);

            // printf("人脸比对结果上传[0x%x]：[%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d]fSimilarity[%f]FaceID[%d]BlackListID[%d]"
            // "Sex[%d]Glasses[%d]Age[%d]MatchPicNum[%d]PicTransType[%d]\n", lCommand, 
            // struAbsTime.dwYear, struAbsTime.dwMonth, struAbsTime.dwDay, struAbsTime.dwHour, struAbsTime.dwMinute, 
            // struAbsTime.dwSecond, struFaceMatchAlarm.fSimilarity, struFaceMatchAlarm.struSnapInfo.dwSnapFacePicID,
            // struFaceMatchAlarm.struBlackListInfo.struBlackListInfo.dwRegisterID, struFaceMatchAlarm.struSnapInfo.bySex,
            // struFaceMatchAlarm.struSnapInfo.byGlasses, struFaceMatchAlarm.struSnapInfo.byAge, 
            // struFaceMatchAlarm.byMatchPicNum, struFaceMatchAlarm.byPicTransType);


            char Rect[24]   = {'\0'};       //人脸位置
            char FDID[100]  = {'\0'};       //人脸库fdid
            char PID [100]  = {'\0'};       //人脸库对应图片pid
            char Name[100]  = {'\0'};       //人员姓名
            char Score[4]   = {'\0'};       //人脸相似度得分
            char Quality[4] = {'\0'};       //图片质量
			char *face_name = (char*)struFaceMatchAlarm.struBlackListInfo.struBlackListInfo.struAttribute.byName;   //人脸姓名（gbk）
            // char *face_buffer = new char[struFaceMatchAlarm.struSnapInfo.dwSnapFacePicLen];         

            sprintf(Score,"%1.3f",struFaceMatchAlarm.fSimilarity);      //获取人脸相似度
            sprintf(Quality,"%d",struFaceMatchAlarm.struSnapInfo.byFacePicQuality);     //获取图片质量
            memcpy(FDID,struFaceMatchAlarm.struBlackListInfo.pFDID,struFaceMatchAlarm.struBlackListInfo.dwFDIDLen);     //获取人脸库fdid
            memcpy(PID,struFaceMatchAlarm.struBlackListInfo.pPID,struFaceMatchAlarm.struBlackListInfo.dwPIDLen);        //获取人脸库图片pid
			int status =GBK_TO_UTF8(face_name,strlen(face_name),Name,strlen(face_name)+2);      //获取人员姓名
            // memcpy(face_buffer,struFaceMatchAlarm.struSnapInfo.pBuffer1,struFaceMatchAlarm.struSnapInfo.dwSnapFacePicLen);
            std::string img_background = returnImgData(struFaceMatchAlarm.pSnapPicBuffer);      //背景图片
            std::string img_face = returnImgData(struFaceMatchAlarm.struSnapInfo.pBuffer1);     //人脸图片
            // printf("imglen:%d\t%d",img_background.length(),img_face.length()); 
            sprintf(Rect,"%1.3f,%1.3f,%1.3f,%1.3f",struFaceMatchAlarm.struRegion.fX,struFaceMatchAlarm.struRegion.fY,struFaceMatchAlarm.struRegion.fWidth,struFaceMatchAlarm.struRegion.fHeight);       //人脸位置信息
            
            //写入上传数据
            upload_data["pid"] = PID;
            upload_data["name"] = Name;
            upload_data["face_rect"] = Rect;
            upload_data["facelib_fdid"] = FDID;
            upload_data["record_time"]  = chTime;
            upload_data["timestamp"]  = getLocalTime();
            upload_data["image_background"]=img_background;
            upload_data["image_face"]=img_face;
            upload_data["face_score"]= Score;
            upload_data["face_quality"]= Quality;
            //上传报警信息
            uploadAlarm("device.face",upload_data);
            // delete []face_buffer;
        }
        break;
        
        
         case COMM_VCA_ALARM: //智能检测通用报警
        {
            printf("\n智能检测通用报警\n");

            if(pAlarmInfo == NULL)
            {
                return FALSE;                
            }
            std::string original_data(pAlarmInfo);  //原始数据
            std::string filter_data;                //车辆车牌数据
            std::stringstream ss(original_data);    //将原始数据转化为数据流
            std::string data;                       //临时存储数据
            int i=0;
            //从原始数据中截取第7-12行数据，即后续处理所需的数据
            while(ss>>data)
            {
                if(i>7 && i<12)filter_data+=data;
                //printf("%d:%s\n",i,data.c_str());
                i++;
            }
            Json::Value object;
            Json::Reader reader;

            //将数据转换为json格式
            if(!reader.parse(filter_data,object,false))
            {
                printf("[JSON ERROR] alarm.cpp MessageCallback COMM_VCA_ALARM reader fail!");
                return TRUE;
            }

            //判断数据是否为空
            if(!object.isObject() || object["CaptureResult"].isNull())
            {
                printf("[JSON ERROR] alarm.cpp MessageCallback COMM_VCA_ALARM 1 json is null!");
                return TRUE;
            }
            //判断是否含有车辆数据
            if(object["CaptureResult"][0]["Vehicle"].isNull())
            {
                printf("[JSON ERROR] alarm.cpp MessageCallback COMM_VCA_ALARM 2 json is null!");
                return TRUE;
            }
            int size = object["CaptureResult"].size();          //数据总数
            
            // printf("size:%d\n",size);
            //遍历车辆识别结果列表。仅存储含有车牌信息的车辆数据
            for(int i = 0;i<size;i++)
            {
                //判断是否含有车牌数据
                if(object["CaptureResult"][i]["Vehicle"]["Property"][2]["value"].asString().length()<2) continue;

                Json::Value each;       //临时json，存储单辆车信息

                each["vehicle_rect"] = returntRect(object["CaptureResult"][i]["Vehicle"]["VehicleRect"]);       //车辆在图片中的位置
                each["plate_rect"] = returntRect(object["CaptureResult"][i]["Vehicle"]["PlateRect"]);           //车牌在图片中的位置
                each["plate"] = object["CaptureResult"][i]["Vehicle"]["Property"][2]["value"].asString();       //车牌信息
                each["image_background"] = returnImgData(object["CaptureResult"][i]["Vehicle"]["vehicleBackgroundImageURL"].asString());        //背景图片
                each["image_vehicle"] = returnImgData(object["CaptureResult"][i]["Vehicle"]["vehicleImageURL"].asString());     //车辆图片
                upload_data["vehicle_list"].append(each);       //将车辆信息添加到上传列表
                
            }
            //判断上传车辆列表是否为空
            if(upload_data["vehicle_list"].size()==0)  return TRUE;
           
           //上传车牌识别信息
            upload_data["record_time"]  = returntime(object["dateTime"].asString());
            upload_data["device_id"]    = device_info.device_id;
            upload_data["timestamp"]  = getLocalTime();
            uploadAlarm("device.vehicleinfo",upload_data);
            //printf("智能检测通用报警, Json数据内容：%s\n", filter_data.c_str());

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
/*人脸抓拍，关闭
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

        */
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
    Json::Value message,header,payload;
    Json::StyledWriter writer;

    header["namespace"] = "xinke.device.upload";
    header["name"] = name;
    header["messageId"] = getLocalTime();
    payload = data;
    message["header"]=header;
    message["payload"]=payload;
    std::string message_data = writer.write(message);
    int status = upload_ramq.publish(message_data);
    return status;
}


/**
 * @brief   返回图片数据
 * @author  liuguang
 * @date    2021/06/22 
 * @param   [in] buffer ：图片信息
 * @return  [string] 图片数据
 */
std::string returnImgData(BYTE *buffer )
{
    std::string filename;
    int filelen;
    ReturnFileName(buffer,filename,filelen);
    char *img_buffer = new char[IMG_BUFFER];
    DWORD lpdwRetLen;
    int status = NET_DVR_GetPicture_V30(device_info.user_id,(char *)filename.c_str(),img_buffer,IMG_BUFFER,&lpdwRetLen);
    printf("/nlen:%d\tLen:%d\n",filelen,lpdwRetLen);
    std::string imgdata = base64_encode(img_buffer,filelen);
    delete []img_buffer;
    return imgdata;
}

/**
 * @brief   返回图片数据
 * @author  liuguang
 * @date    2021/06/22 
 * @param   [in] url ：图片url
 * @return  [string] 图片数据
 */
std::string returnImgData(std::string url )
{
    std::string filename;
    int filelen;
    ReturnFileName(url,filename,filelen);
    char *img_buffer = new char[IMG_BUFFER];
    DWORD lpdwRetLen;
    int status = NET_DVR_GetPicture_V30(device_info.user_id,(char *)filename.c_str(),img_buffer,IMG_BUFFER,&lpdwRetLen);
    // printf("\nlen:%d\tLen:%d\n",filelen,lpdwRetLen);
    std::string imgdata = base64_encode(img_buffer,filelen);
    delete []img_buffer;
    return imgdata;
}



/**
 * @brief   返回指定时间格式
 * @author  liuguang
 * @date    2021/06/28 
 * @param   [in] src ：原始字符串，形如：2021-06-24T14:58:31+08:00
 * @return  [string] 指定格式时间字符串，形如：20210624145800
 */
std::string returntime(std::string src)
{
    std::string result;
    src = src.substr(0,src.length()-5);
    for(auto it : src)
    {
        if(it>='0' && it <='9') result+=it;
    }
    return result;
}


/**
 * @brief   返回目标位置
 * @author  liuguang
 * @date    2021/06/28
 * @param   [in] src ：输入坐标位置
 * @return  [string] 输出坐标字符串
 */
std::string returntRect(Json::Value src)
{
    std::string result;
    result += src["x"].asString();
    result += ",";
    result += src["y"].asString();
    result += ",";
    result += src["width"].asString();
    result += ",";
    result += src["height"].asString();
    return result;
}