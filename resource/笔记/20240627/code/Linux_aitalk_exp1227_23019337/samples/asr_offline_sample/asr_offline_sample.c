#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../../include/qisr.h"
#include "../../include/msp_cmn.h"
#include "../../include/msp_errors.h"

#define SAMPLE_RATE_16K     (16000)
#define SAMPLE_RATE_8K      (8000)
#define MAX_GRAMMARID_LEN   (32)
#define MAX_PARAMS_LEN      (1024)

const char * ASR_RES_PATH        = "fo|res/asr/common.jet"; //离线语法识别资源路径
const char * GRM_BUILD_PATH      = "res/asr/GrmBuilld"; //构建离线语法识别网络生成数据保存路径
const char * GRM_FILE            = "call.bnf"; //构建离线识别语法网络所用的语法文件
const char * LEX_NAME            = "contact"; //更新离线识别语法的contact槽（语法文件为此示例中使用的call.bnf）

typedef struct _UserData {
	int     build_fini; //标识语法构建是否完成
	int     update_fini; //标识更新词典是否完成
	int     errcode; //记录语法构建或更新词典回调错误码
	char    grammar_id[MAX_GRAMMARID_LEN]; //保存语法构建返回的语法ID
}UserData;


const char *get_audio_file(void); //选择进行离线语法识别的语音文件
int build_grammar(UserData *udata); //构建离线识别语法网络
int update_lexicon(UserData *udata); //更新离线识别语法词典
int run_asr(UserData *udata); //进行离线语法识别

const char* get_audio_file(void)
{
	char key = 0;
	while(key != 27) //按Esc则退出
	{
		printf("请选择音频文件：\n");
		printf("1.打电话给丁伟\n");
		printf("2.打电话给黄辣椒\n");
		key = getchar();
		getchar();
		switch(key)
		{
		case '1':
			printf("\n1.打电话给丁伟\n");
			return "wav/ddhgdw.pcm";
		case '2':
			printf("\n2.打电话给黄辣椒\n");
			return "wav/ddhghlj.pcm";
		default:
			continue;
		}
	}
	exit(0);
	return NULL;
}

int build_grm_cb(int ecode, const char *info, void *udata)
{
	UserData *grm_data = (UserData *)udata;

	if (NULL != grm_data) {
		grm_data->build_fini = 1;
		grm_data->errcode = ecode;
	}

	if (MSP_SUCCESS == ecode && NULL != info) {
		printf("构建语法成功！ 语法ID:%s\n", info);
		if (NULL != grm_data)
			snprintf(grm_data->grammar_id, MAX_GRAMMARID_LEN - 1, info);
	}
	else
		printf("构建语法失败！%d\n", ecode);

	return 0;
}

int build_grammar(UserData *udata)
{
	FILE *grm_file                           = NULL;
	char *grm_content                        = NULL;
	unsigned int grm_cnt_len                 = 0;
	char grm_build_params[MAX_PARAMS_LEN]    = {0};
	int ret                                  = 0;

	grm_file = fopen(GRM_FILE, "rb");	
	if(NULL == grm_file) {
		printf("打开\"%s\"文件失败！[%s]\n", GRM_FILE, strerror(errno));
		return -1; 
	}

	fseek(grm_file, 0, SEEK_END);
	grm_cnt_len = ftell(grm_file);
	fseek(grm_file, 0, SEEK_SET);

	grm_content = (char *)malloc(grm_cnt_len + 1);
	if (NULL == grm_content)
	{
		printf("内存分配失败!\n");
		fclose(grm_file);
		grm_file = NULL;
		return -1;
	}
	fread((void*)grm_content, 1, grm_cnt_len, grm_file);
	grm_content[grm_cnt_len] = '\0';
	fclose(grm_file);
	grm_file = NULL;

	snprintf(grm_build_params, MAX_PARAMS_LEN - 1, 
		"engine_type = local, \
		asr_res_path = %s, sample_rate = %d, \
		grm_build_path = %s, ",
		ASR_RES_PATH,
		SAMPLE_RATE_16K,
		GRM_BUILD_PATH
		);
	ret = QISRBuildGrammar("bnf", grm_content, grm_cnt_len, grm_build_params, build_grm_cb, udata);

	free(grm_content);
	grm_content = NULL;

	return ret;
}

int update_lex_cb(int ecode, const char *info, void *udata)
{
	UserData *lex_data = (UserData *)udata;

	if (NULL != lex_data) {
		lex_data->update_fini = 1;
		lex_data->errcode = ecode;
	}

	if (MSP_SUCCESS == ecode)
		printf("更新词典成功！\n");
	else
		printf("更新词典失败！%d\n", ecode);

	return 0;
}

int update_lexicon(UserData *udata)
{
	const char *lex_content                   = "丁伟\n黄辣椒";
	unsigned int lex_cnt_len                  = strlen(lex_content);
	char update_lex_params[MAX_PARAMS_LEN]    = {0}; 

	snprintf(update_lex_params, MAX_PARAMS_LEN - 1, 
		"engine_type = local, text_encoding = UTF-8, \
		asr_res_path = %s, sample_rate = %d, \
		grm_build_path = %s, grammar_list = %s, ",
		ASR_RES_PATH,
		SAMPLE_RATE_16K,
		GRM_BUILD_PATH,
		udata->grammar_id);
	return QISRUpdateLexicon(LEX_NAME, lex_content, lex_cnt_len, update_lex_params, update_lex_cb, udata);
}

int sockfd = -1;
struct sockaddr_in my_addr;
socklen_t addrlen = sizeof(my_addr);

//语音识别
int run_asr1(UserData *udata)
{
	char asr_params[MAX_PARAMS_LEN]    = {0};
	const char *rec_rslt               = NULL;
	const char *session_id             = NULL;
	const char *asr_audiof             = NULL;
	FILE *f_pcm                        = NULL;
	char *pcm_data                     = NULL;
	long pcm_count                     = 0;
	long pcm_size                      = 0;
	//int last_audio                     = 0;
	int aud_stat                       = MSP_AUDIO_SAMPLE_CONTINUE;
	int ep_status                      = MSP_EP_LOOKING_FOR_SPEECH;
	int rec_status                     = MSP_REC_STATUS_INCOMPLETE;
	int rss_status                     = MSP_REC_STATUS_INCOMPLETE;
	int errcode                        = -1;

	//asr_audiof = get_audio_file();//你要识别哪个文件   这个文件可以固定
	asr_audiof = "wav/hehe.pcm";//这里就固定到这个文件上面去了
	f_pcm = fopen(asr_audiof, "rb");
	if (NULL == f_pcm) {
		printf("打开\"%s\"失败！[%s]\n", asr_audiof, strerror(errno));
		goto run_error;
	}
	fseek(f_pcm, 0, SEEK_END);
	pcm_size = ftell(f_pcm);
	fseek(f_pcm, 0, SEEK_SET);
	pcm_data = (char *)malloc(pcm_size);
	if (NULL == pcm_data)
		goto run_error;
	fread((void *)pcm_data, pcm_size, 1, f_pcm);
	fclose(f_pcm);
	f_pcm = NULL;

	//离线语法识别参数设置
	snprintf(asr_params, MAX_PARAMS_LEN - 1, 
		"engine_type = local, \
		asr_res_path = %s, sample_rate = %d, \
		grm_build_path = %s, local_grammar = %s, \
		result_type = xml, result_encoding = UTF-8, ",
		ASR_RES_PATH,
		SAMPLE_RATE_16K,
		GRM_BUILD_PATH,
		udata->grammar_id
		);
	session_id = QISRSessionBegin(NULL, asr_params, &errcode);
	if (NULL == session_id)
		goto run_error;
	printf("开始识别...\n");

	while (1) {
		unsigned int len = 6400;

		if (pcm_size < 12800) {
			len = pcm_size;
			//last_audio = 1;
		}

		aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;

		if (0 == pcm_count)
			aud_stat = MSP_AUDIO_SAMPLE_FIRST;

		if (len <= 0)
			break;

		printf(">");
		fflush(stdout);
		errcode = QISRAudioWrite(session_id, (const void *)&pcm_data[pcm_count], len, aud_stat, &ep_status, &rec_status);
		if (MSP_SUCCESS != errcode)
			goto run_error;

		pcm_count += (long)len;
		pcm_size -= (long)len;

		//检测到音频结束
		if (MSP_EP_AFTER_SPEECH == ep_status)
			break;

		usleep(150 * 1000); //模拟人说话时间间隙
	}
	//主动点击音频结束
	QISRAudioWrite(session_id, (const void *)NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_status, &rec_status);

	free(pcm_data);
	pcm_data = NULL;

	//获取识别结果
	while (MSP_REC_STATUS_COMPLETE != rss_status && MSP_SUCCESS == errcode) {
		rec_rslt = QISRGetResult(session_id, &rss_status, 0, &errcode);
		usleep(150 * 1000);
	}
	printf("\n识别结束：\n");
	printf("=============================================================\n");
	int id = -1;
	if (NULL != rec_rslt)
	{
		printf("%s\n", rec_rslt);//输出识别的信息
		//将这个结果处理  将识别的id发回给客户端   处理识别的字符串
		while(*rec_rslt)
		{
			if(strncmp(rec_rslt,"id=",3) == 0)
			{
				break;
			}
			rec_rslt++;
		}
		
		char buf[16] = {0};
		strncpy(buf,rec_rslt + 4,4);
		id = atoi(buf);
		
	}
	else
		printf("没有识别结果！\n");
	printf("=============================================================\n");
	printf("识别的id为 %d\n",id);
	sendto(sockfd,&id, 4,0,(struct sockaddr *)&my_addr,addrlen);
	printf("识别结果发送给了客户端了\n");
	//将id发送给客户端
	goto run_exit;

run_error:
	if (NULL != pcm_data) {
		free(pcm_data);
		pcm_data = NULL;
	}
	if (NULL != f_pcm) {
		fclose(f_pcm);
		f_pcm = NULL;
	}
run_exit:
	QISRSessionEnd(session_id, NULL);
	return errcode;
}

char *pcm_data                     = NULL;
long pcm_size                      = 0;
//语音识别
int run_asr(UserData *udata)
{
	char asr_params[MAX_PARAMS_LEN]    = {0};
	const char *rec_rslt               = NULL;
	const char *session_id             = NULL;
	//const char *asr_audiof             = NULL;
	//FILE *f_pcm                        = NULL;
	
	long pcm_count                     = 0;
	
	//int last_audio                     = 0;
	int aud_stat                       = MSP_AUDIO_SAMPLE_CONTINUE;
	int ep_status                      = MSP_EP_LOOKING_FOR_SPEECH;
	int rec_status                     = MSP_REC_STATUS_INCOMPLETE;
	int rss_status                     = MSP_REC_STATUS_INCOMPLETE;
	int errcode                        = -1;

	//asr_audiof = get_audio_file();//你要识别哪个文件   这个文件可以固定
	//asr_audiof = "wav/hehe.pcm";//这里就固定到这个文件上面去了
	/*
	f_pcm = fopen(asr_audiof, "rb");
	if (NULL == f_pcm) {
		printf("打开\"%s\"失败！[%s]\n", asr_audiof, strerror(errno));
		goto run_error;
	}
	fseek(f_pcm, 0, SEEK_END);
	pcm_size = ftell(f_pcm);
	fseek(f_pcm, 0, SEEK_SET);
	pcm_data = (char *)malloc(pcm_size);
	if (NULL == pcm_data)
		goto run_error;
	fread((void *)pcm_data, pcm_size, 1, f_pcm);
	fclose(f_pcm);
	f_pcm = NULL;*/

	//离线语法识别参数设置
	snprintf(asr_params, MAX_PARAMS_LEN - 1, 
		"engine_type = local, \
		asr_res_path = %s, sample_rate = %d, \
		grm_build_path = %s, local_grammar = %s, \
		result_type = xml, result_encoding = UTF-8, ",
		ASR_RES_PATH,
		SAMPLE_RATE_16K,
		GRM_BUILD_PATH,
		udata->grammar_id
		);
	session_id = QISRSessionBegin(NULL, asr_params, &errcode);
	if (NULL == session_id)
		goto run_error;
	printf("开始识别...\n");

	while (1) {
		unsigned int len = 6400;

		if (pcm_size < 12800) {
			len = pcm_size;
			//last_audio = 1;
		}

		aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;

		if (0 == pcm_count)
			aud_stat = MSP_AUDIO_SAMPLE_FIRST;

		if (len <= 0)
			break;

		printf(">");
		fflush(stdout);
		errcode = QISRAudioWrite(session_id, (const void *)&pcm_data[pcm_count], len, aud_stat, &ep_status, &rec_status);
		if (MSP_SUCCESS != errcode)
			goto run_error;

		pcm_count += (long)len;
		pcm_size -= (long)len;

		//检测到音频结束
		if (MSP_EP_AFTER_SPEECH == ep_status)
			break;

		usleep(150 * 1000); //模拟人说话时间间隙
	}
	//主动点击音频结束
	QISRAudioWrite(session_id, (const void *)NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_status, &rec_status);

	free(pcm_data);
	pcm_data = NULL;

	//获取识别结果
	while (MSP_REC_STATUS_COMPLETE != rss_status && MSP_SUCCESS == errcode) {
		rec_rslt = QISRGetResult(session_id, &rss_status, 0, &errcode);
		usleep(150 * 1000);
	}
	printf("\n识别结束：\n");
	printf("=============================================================\n");
	int id = -1;
	if (NULL != rec_rslt)
	{
		printf("%s\n", rec_rslt);//输出识别的信息
		//将这个结果处理  将识别的id发回给客户端   处理识别的字符串
		while(*rec_rslt)
		{
			if(strncmp(rec_rslt,"id=",3) == 0)
			{
				break;
			}
			rec_rslt++;
		}
		
		char buf[16] = {0};
		strncpy(buf,rec_rslt + 4,4);
		id = atoi(buf);
		
	}
	else
		printf("没有识别结果！\n");
	printf("=============================================================\n");
	printf("识别的id为 %d\n",id);
	sendto(sockfd,&id, 4,0,(struct sockaddr *)&my_addr,addrlen);
	printf("识别结果发送给了客户端了\n");
	//将id发送给客户端
	goto run_exit;

run_error:
	if (NULL != pcm_data) {
		free(pcm_data);
		pcm_data = NULL;
	}
	
run_exit:
	QISRSessionEnd(session_id, NULL);
	return errcode;
}



//初始化UDP服务器  成功返回0
int UDP_Servcer_Init(const char * inetaddr,unsigned int port)
{
	
    //1创建套接字 
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(-1 == sockfd)
    {
        perror("socket error");
        return -2;
    }
    //2绑定地址
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = inet_addr(inetaddr);
    int r = bind(sockfd,(struct sockaddr *)&sa,sizeof(sa));
    if(-1 == r)
    {
        perror("bind error");
        return -3;
    }
	return 0;
}



//写一个从客户端接收文件的代码
//记住：接收文件一定要先接收文件的大小  再接收文件的内容
void getClientFile()  //接受到的文件每次都将 wav/hehe.pcm文件覆盖
{
	//unsigned char RecvBuf[1024] = {0};
	
	//接收的时候首先接收的是文件大小
	int filesize = 0;
	recvfrom(sockfd,&filesize,4,0,(struct sockaddr *)&my_addr,&addrlen);
	printf("文件大小为 %d 个字节\n",filesize);
	pcm_size = filesize;
	pcm_data = (char *)malloc(pcm_size);
	//给客户端一个回应  让它开始往下走
	sendto(sockfd,"next!!!", 8,0,(struct sockaddr *)&my_addr,addrlen);
	
	//打开音频文件
	//int fd = open("wav/hehe.pcm",O_RDWR | O_TRUNC);
	//if(-1 == fd)
	//{
		//perror("open error");
		//return;
	//}
	
	
	int writesize = 0;
	//接收文件内容
	while(1)
	{
		int r = recvfrom(sockfd,pcm_data + writesize,1024,0,(struct sockaddr *)&my_addr,&addrlen);
		if(r == -1)
		{
			perror("recvfrom error");
			return;
		}
		//将文件内容写入到文件里面去
		//write(fd,RecvBuf,r);
		//让他开始下一次的传输
		sendto(sockfd,"next!!!", 8,0,(struct sockaddr *)&my_addr,addrlen);

		writesize += r;
		if(writesize >= filesize)
		{
			printf("文件接收完毕\n");
			break;
		}
		
		
				
		
	}
	//close(fd);
}



int main(int argc, char* argv[])
{
	if(argc < 3)
    {
        printf("参数数量不够，例子：./a.out 192.168.100.249 6789\n");
        return -1;
    }
	if(UDP_Servcer_Init(argv[1],atoi(argv[2])))
	{
		printf("服务器初始化失败了\n");
		return -2;
	}
	
	const char *login_config    = "appid = 23019337"; //登录参数
	UserData asr_data; 
	int ret                     = 0 ;
	//char c;

	ret = MSPLogin(NULL, NULL, login_config); //第一个参数为用户名，第二个参数为密码，传NULL即可，第三个参数是登录参数
	if (MSP_SUCCESS != ret) {
		printf("登录失败：%d\n", ret);
		goto exit;
	}

	memset(&asr_data, 0, sizeof(UserData));
	printf("构建离线识别语法网络...\n");
	ret = build_grammar(&asr_data);  //第一次使用某语法进行识别，需要先构建语法网络，获取语法ID，之后使用此语法进行识别，无需再次构建
	if (MSP_SUCCESS != ret) {
		printf("构建语法调用失败！\n");
		goto exit;
	}
	while (1 != asr_data.build_fini)
		usleep(300 * 1000);
	if (MSP_SUCCESS != asr_data.errcode)
		goto exit;
	printf("离线识别语法网络构建完成，开始识别...\n");	
	while(1)//让它在这里重复的识别
	{
		//printf("按回车键开始识别.......\n");
		//getchar();
		//printf("请录音4s.......\n");
		//system("arecord -d4 -c1 -r16000 -traw -fS16_LE ./wav/hehe.pcm");
		printf("等待客户端发送录音文件过来.......\n");
		getClientFile();
		ret = run_asr(&asr_data);//这个函数就是去识别语音的
		if (MSP_SUCCESS != ret) {
			printf("离线语法识别出错: %d \n", ret);
			goto exit;
		}
	}

	printf("更新离线语法词典...\n");
	ret = update_lexicon(&asr_data);  //当语法词典槽中的词条需要更新时，调用QISRUpdateLexicon接口完成更新
	if (MSP_SUCCESS != ret) {
		printf("更新词典调用失败！\n");
		goto exit;
	}
	while (1 != asr_data.update_fini)
		usleep(300 * 1000);
	if (MSP_SUCCESS != asr_data.errcode)
		goto exit;
	printf("更新离线语法词典完成，开始识别...\n");
	ret = run_asr(&asr_data);
	if (MSP_SUCCESS != ret) {
		printf("离线语法识别出错: %d \n", ret);
		goto exit;
	}

exit:
	MSPLogout();
	printf("请按任意键退出...\n");
	getchar();
	return 0;
}

