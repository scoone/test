/*
 * PrivateProtocolData.h
 *
 *  Created on: 2016-6-29
 *      Author: xiewx
 */

#ifndef PRIVATEPROTOCOLDATA_H_
#define PRIVATEPROTOCOLDATA_H_

#define BROADCAST_ADDR  "255.255.255.255"
#define BROADCAST_PORT	6537 // 0x1989
#define CONNECT_PORT	6553 // 0x1999

#define RECV_BUFFSIZE	1024

#ifndef INVALID_SOCKET
#define INVALID_SOCKET		-1
#endif

#define STREAM_PACKET_SPLIT	">>"
#define DATAGRAM_PACKET_SPLIT	":"
#define PACKET_VERSION	"2"
#define PACKET_SENDER_TYPE	"TV"
#define PACKET_SENDER_NAME	"TV-000" // ip last 3 bit

/*
 * public static final int SENDVERSIONPORT = 0x1996;//发送版本信息,Version 4,若不发，遥控器不能使用
	public static final int CAPTURE_PORT = 0x199A;//截屏
	public static final int AUDIO_PORT = 0x10EC;//语音
	public static final int HTTP_PORT = 0x1F35;//http服务端口，上传图片客户端获取
 * */

#define MMTYPE_AUDIO    "Audio"
#define MMTYPE_VIDEO    "Video"
#define MMTYPE_IMAGE    "Image"

#define MMSTATUS_OK "OK"
#define MMSTATUS_ERROR_OCCURRED "ERROR_OCCURRED"

#define MMSTATE_STOPPED "STOPPED"
#define MMSTATE_PLAYING "PLAYING"
#define MMSTATE_TRANSITIONING "TRANSITIONING"
#define MMSTATE_PAUSED_PLAYBACK "PAUSED_PLAYBACK"
#define MMSTATE_PLAYER_EXIT "PLAYER_EXIT"
#define MMSTATE_COMPLETED "COMPLETED"

#define CLIENTTYPE "clienttype"
#define ONLINE  "YES"

#define NPP_TRACE	printf("-->%s (%s line:%d)\n", __FUNCTION__, __FILE__, __LINE__)

typedef enum
{
	EN_CMD_UNKNOWN,
	EN_CMD_ONLINE,
	EN_CMD_OFFLINE,
	EN_CMD_ANSONLINE,

    EN_CMD_MEDIA_STOP = 129,
    EN_CMD_MEDIA_SET_VOL = 130,
    EN_CMD_MEDIA_SET_MUTE = 131,
    EN_CMD_MEDIA_SEEK = 132,
    EN_CMD_MEDIA_PREPARE = 133,
    EN_CMD_MEDIA_PLAY = 134,
    EN_CMD_MEDIA_PAUSE = 135,
    EN_CMD_MEDIA_GET_VOL = 136,
    EN_CMD_MEDIA_GET_SUPPORTED = 137,
    EN_CMD_MEDIA_GET_PLAYSTATUS = 138, // "OK" or  "ERROR_OCCURRED"
    EN_CMD_MEDIA_GET_PLAYSTATE = 139,
    EN_CMD_MEDIA_GET_MUTE = 140,
    EN_CMD_MEDIA_GET_DURATION = 141,
    EN_CMD_MEDIA_GET_POSITION = 142,
    EN_CMD_MEDIA_GET_TRANSPORT = 143,
    EN_CMD_MEDIA_SET_PLAYSTATUS = 144,
    EN_CMD_MEDIA_SET_PLAYSTATE = 145,
    EN_CMD_MEDIA_SET_DURATION = 146,
    EN_CMD_MEDIA_SET_POSITION = 147,
    EN_CMD_MEDIA_SET_TRANSPORT = 148,

    EN_CMD_KEY = 149,
    EN_CMD_ISONLINE   = 150,
    EN_CMD_MUSE = 151,

    EN_CMD_MEDIA_PHOTO_LIST = 152,
    EN_CMD_MEDIA_MUSIC_LIST = 153,
    EN_CMD_MEDIA_VIDEO_LIST = 154,

    EN_CMD_INPUTSTR = 155,
    EN_CMD_AIWI_GAME = 156,
    EN_CMD_VOICE_INPUT = 157,
    EN_CMD_LOAD = 158,
    EN_CMD_GET_CLIENTTYPE = 159,
    EN_CMD_PERSONALTV_ACTIVE_CONN = 160,

    EN_CMD_MEDIA_PREVIOUS = 176,
    EN_CMD_MEDIA_NEXT = 177,
    EN_CMD_MEDIA_NO_PREV_FILE = 178,
    EN_CMD_MEDIA_NO_NEXT_FILE = 179,
    EN_CMD_MEDIA_PIC_ROTATE = 180,
    EN_CMD_MEDIA_SET_PIC_SLIDE_SHOW = 181,
    EN_CMD_MEDIA_SET_MUSIC_PLAYMODE = 182,

    EN_CMD_GET_SYS_VOL = 183, // 0Xb7
    EN_CMD_SET_SYS_VOL = 184,
    EN_CMD_SEND_PLAYING_NAME = 185,
    EN_CMD_GET_PLAYING_NAME = 186,
    EN_CMD_PIC_PLAYER_EXIT = 187,
    EN_CMD_PLAY_SEEK = 188,
    EN_CMD_SEND_PHONE_NAME = 189,
    EN_CMD_MEDIA_PIC_ZOOM = 190,
    EN_CMD_PERSONALTV_SHAKE = 191,

    EN_CMD_YOUTUBE_PLAYER = 209,
    EN_CMD_EXPAND_PLAYER = 222, // such as tentplayer
    EN_CMD_APP_MANAGE = 223,
    EN_CMD_UNKNOWN_PLAYER = 224,
    EN_CMD_SCREENCAPTURE = 225, // 0xE1
    EN_CMD_MEDIA_PLAYLIST = 229, // 0xE5
    EN_CMD_GET_PLAYONLY = 230,
    EN_CMD_SET_PLAYONLY = 231,
    EN_CMD_PICTURE_ZOOM = 232
}EN_COMMAND_NO;

typedef enum
{
    TR_KEY_1 = 1,
    TR_KEY_2 = 2,
    TR_KEY_3 = 3,
    TR_KEY_4 = 4,
    TR_KEY_5 = 5,
    TR_KEY_6 = 6,
    TR_KEY_7 = 7,
    TR_KEY_8 = 8,
    TR_KEY_9 = 9,
    TR_KEY_0 = 10,
    TR_KEY_UP = 11,
    TR_KEY_DOWN = 12,
    TR_KEY_LEFT = 13,
    TR_KEY_RIGHT = 14,
    TR_KEY_OK = 15,
    TR_KEY_BACK = 16,
    TR_KEY_3D = 17,
    TR_KEY_SUBMENU = 18,
    TR_KEY_MAINMENU = 19,
    TR_KEY_POWER = 20,
    TR_KEY_VOL_UP = 21,
    TR_KEY_VOL_DOWN = 22,
    TR_KEY_MUTE = 23,
    TR_KEY_EPG = 24,
    TR_KEY_DISPLAY = 25,
    TR_KEY_PLAYBACK = 26,
    TR_KEY_CH_UP = 27,
    TR_KEY_CH_DOWN = 28,
    TR_KEY_SOURCE = 29,
    TR_KEY_SCALE = 30,
    TR_KEY_PICTURE = 31,
    TR_KEY_FAVORITE = 32,
    TR_KEY_SEARCH = 33,
    TR_KEY_RED = 34,
    TR_KEY_GREEN = 35,
    TR_KEY_YELLOW = 36,
    TR_KEY_BLUE = 37,
    TR_KEY_BACKSPACE = 38,
    TR_KEY_MOUSELEFT = 39,
    TR_KEY_MOUSERIGHT = 40,
    TR_KEY_INFOWINDOW = 41,
    TR_KEY_DOT=75
}EN_KEY;


/*PrivateProtocolPacket*/
class DatagramPrivatePacket
{
	string mversion;
	string mpacketNo; //timestamp
	string msenderName;
	string msenderType;
	string mcommandNo;
	string mexpand;
public:
	void parse(char *data);
	string &getVersion();
	string &getPacketNo();
	string &getName();
	string &getType();
	string &getCommandNo();
	string &getExpand();
};

#include <vector>
using namespace std;
class StreamPrivatePacket
{
	vector<char *> mcstrvec;
	vector<string> mstrvec;
public:
	StreamPrivatePacket();
	virtual ~StreamPrivatePacket();
	vector<char *> &parse(char *data, const char *splitstr);
	vector<string> &parse(string &data, string &splitstr);
};


#endif /* PRIVATEPROTOCOLDATA_H_ */
