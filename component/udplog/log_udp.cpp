#include <pthread.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <stack>
#include <queue>
#include <cstdio>  
#include "log_udp.h"   

// user define
static std::string getLogFileName(std::string app_name);
static bool judgeCurrTimer();
static bool getNewFileOpened();
static int write_local_log(char* ptr, int curr_index);

static char log_dir[128] = "../log/";

#define MAX_LOG_BUFFER_LEN (64*1024) // one log max size

static inline uint64_t hton64(uint64_t h64)
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
	uint64_t n64 = htonl((uint32_t)h64);
	n64 <<= 32;
	n64 += htonl((uint32_t)(h64 >> 32));
	return n64;
#else
	return h64;
#endif
}

#define ntoh64 hton64

#define MAX_CURR_TIME_STRING_LEN  28  // strlen("2015-10-23 09:15:59.737940") + 1
#define MAX_EXT_STRING_LEN   32
#define MAX_EXT_APP_NAME_LEN    (MAX_APP_NAME_LEN + MAX_EXT_STRING_LEN)
#define LOCAL_FLAG 2
#define REMOTE_FLAG 1
#define FILE_SIZE 1*1024*1024 // create logfile size
#define FILE_COUNTER 10

static inline char * get_curr_time_string(char * time_string_buffer, int buffer_len)
{
	struct timeval tv;
	struct tm tm;

	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &tm);

	tm.tm_year += 1900;
	tm.tm_mon += 1;

	snprintf(time_string_buffer, buffer_len, "%4d-%02d-%02d %02d:%02d:%02d %03d.%03d",
		tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, (uint32_t)(tv.tv_usec / 1000), (uint32_t)(tv.tv_usec % 1000));

	return time_string_buffer;
}

typedef struct _log_pkt_header
{
	char app_name_ext[MAX_EXT_APP_NAME_LEN + 1];
	uint64_t u64_ext;
	int command;
	int log_level;
	uint64_t log_seq;
	char log_data[0];
}log_pkt_header_t;

int log_type;	

typedef struct _log_cntt
{
	char app_name_ext[MAX_EXT_APP_NAME_LEN + 1];
	uint64_t u64_ext;
	uint64_t log_seq;
	int log_fd;
	int log_level;
	std::ofstream os;	
	std::string curFileName;	

	struct tm timeinfo; 
	struct sockaddr_in server_address;
	char log_buffer[MAX_LOG_BUFFER_LEN + 1];
}log_cntt_t;

#define LOG_CNTT_CNTS   8

static pthread_key_t log_key;
static pthread_once_t log_key_once = PTHREAD_ONCE_INIT;

static void make_log_key(void)
{
	pthread_key_create(&log_key, free);
}

static inline const char* get_str_log_level(int log_level)
{
	static const char* str_log_level[8] = { "EMERG", "ALERT", "CRIT", "ERROR", "WARNING", "NOTICE", "INFO", "DEBUG" };

	if ((log_level < 0))
	{
		log_level = 0;
	}
	if ((log_level > 7))
	{
		log_level = 7;
	}

	return str_log_level[log_level];
}

static int load_log_info(log_cntt_t * p_log_cntt, uint32_t svr_id)
{
	uint32_t server_ip = 0;
	int server_cnt = 0;
	int port_start = 0;
	int server_port = 0;
	char log_conf_data[1024];
	FILE * pf = NULL;

	pf = fopen("../conf/log_info.conf", "r");
	if (pf == NULL)
	{
		printf("open log_info.conf fail : %m \r\n");
		return -1;
	}

	while (fgets(log_conf_data, sizeof(log_conf_data) - 1, pf) != NULL)
	{
		if (strncmp(log_conf_data, "server_ip:", strlen("server_ip:")) == 0)
		{
			server_ip = inet_addr(&log_conf_data[strlen("server_ip:")]);
		}
		else if (strncmp(log_conf_data, "server_cnt:", strlen("server_cnt:")) == 0)
		{
			server_cnt = atoi(&log_conf_data[strlen("server_cnt:")]);
		}
		else if (strncmp(log_conf_data, "port_start:", strlen("port_start:")) == 0)
		{
			port_start = atoi(&log_conf_data[strlen("port_start:")]);
		}
	}

	fclose(pf);

	if (server_cnt < 1 || server_cnt > 16)
	{
		printf("server_cnt:%d is illegal \r\n", server_cnt);
		return -1;
	}

	if (port_start < 1025 || port_start > 65535)
	{
		printf("port_start:%d is illegal \r\n", port_start);
		return -1;
	}

	server_port = port_start + (svr_id % server_cnt);

	memset(&(p_log_cntt->server_address), 0, sizeof(struct sockaddr_in));
	p_log_cntt->server_address.sin_family = AF_INET;
	p_log_cntt->server_address.sin_port = htons(server_port);
	p_log_cntt->server_address.sin_addr.s_addr = server_ip;

	return 1;
}

void init_log(char * app_name, uint32_t svr_type, uint32_t svr_id, int t_sn, int h_sn)
{
	log_cntt_t * p_log_cntt = NULL;
	int kernel_buffer_size = 0;
	int i = 0;

	if (app_name == NULL || svr_type == 0 || svr_id == 0 || t_sn < 0 || h_sn < 0 || h_sn >= LOG_CNTT_CNTS)
	{
		printf("app_name or svr_type:%u or svr_id:%u or t_sn:%d or h_sn:%d is illegal \r\n", svr_type, svr_id, t_sn, h_sn);
		exit(-1);
	}
	mkdir(log_dir, 0777);

	pthread_once(&log_key_once, make_log_key);
	p_log_cntt = (log_cntt_t *)pthread_getspecific(log_key);
	if (p_log_cntt == NULL)
	{
		p_log_cntt = new log_cntt_t[LOG_CNTT_CNTS];//(log_cntt_t *)calloc(LOG_CNTT_CNTS, sizeof(log_cntt_t));
		if (p_log_cntt == NULL)
		{
			printf("init_log--->app_name:%s svr_type:%u svr_id:%u t_sn:%d h_sn:%d: allocate log_cntt fail \r\n",
				app_name, svr_type, svr_id, t_sn, h_sn);
			exit(-1);
		}
		pthread_setspecific(log_key, p_log_cntt);
		for (i = 0; i < LOG_CNTT_CNTS; i++)
		{
			p_log_cntt[i].log_fd = -1;
		}
	}
	p_log_cntt = &(p_log_cntt[h_sn]);

	snprintf(p_log_cntt->app_name_ext, MAX_EXT_APP_NAME_LEN, "%s_%u_%u_%d_%d", app_name, svr_type, svr_id, t_sn, h_sn);
	p_log_cntt->app_name_ext[MAX_EXT_APP_NAME_LEN] = 0;
	p_log_cntt->log_seq = 1;

	if (log_type == LOCAL_FLAG)
	{
		/*初始化文件并打开*/
		p_log_cntt->curFileName = getLogFileName(p_log_cntt->app_name_ext);

		char* FILE_NAME = (char*)p_log_cntt->curFileName.data();
		p_log_cntt->os.open(FILE_NAME, std::ios_base::out | std::ios_base::app);
		if (p_log_cntt->os)
		{
			std::cout << "I am " << p_log_cntt->curFileName << "!, it is runing..." << std::endl;
		}
		else
		{
			printf("Please check log dir's privilege!\n");
			exit(-1);
		}
	}
	else
	{
		if (load_log_info(p_log_cntt, svr_id) < 0)
		{
			printf("init_log--->load log server info fail svr_id:%u \r\n", svr_id);
			exit(-1);
		}

		if (p_log_cntt->log_fd >= 0)
		{
			close(p_log_cntt->log_fd);
			p_log_cntt->log_fd = -1;
		}

		p_log_cntt->log_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (p_log_cntt->log_fd < 0)
		{
			printf("create log socket fail : %m \n");
			exit(-1);
		}

		kernel_buffer_size = 0x400000;
		if (setsockopt(p_log_cntt->log_fd, SOL_SOCKET, SO_SNDBUF, &kernel_buffer_size, sizeof(kernel_buffer_size)) != 0)
		{
			printf("set SOL_SOCKET:SO_RCVBUF fail : %m \n");
			exit(-1);
		}
	}
}

void set_log_level(int log_level, int h_sn)
{
	log_cntt_t * p_log_cntt = (log_cntt_t *)pthread_getspecific(log_key);
	if (p_log_cntt == NULL || h_sn < 0 || h_sn >= LOG_CNTT_CNTS)
	{
		printf("set_log_level------------>%s --> %s --> L%d : h_sn:%d call init_log(...) first !!! \r\n", __FILE__, __FUNCTION__, __LINE__, h_sn);
		exit(-1);
	}

	p_log_cntt = &(p_log_cntt[h_sn]);

	if (log_level < LOG_EMERG)
	{
		log_level = LOG_EMERG;
	}
	if (log_level > LOG_DEBUG)
	{
		log_level = LOG_DEBUG;
	}

	p_log_cntt->log_level = log_level;
}

void set_log_type(int LogType)
{
	log_type = LogType;
}

void close_log(int h_sn)
{
	log_cntt_t * p_log_cntt = (log_cntt_t *)pthread_getspecific(log_key);

	if (p_log_cntt == NULL || h_sn < 0 || h_sn >= LOG_CNTT_CNTS)
	{
		return;
	}
	p_log_cntt = &(p_log_cntt[h_sn]);

	if (p_log_cntt->log_fd >= 0)
	{
		close(p_log_cntt->log_fd);
	}

	memset(p_log_cntt, 0, sizeof(log_cntt_t));
	p_log_cntt->log_fd = -1;
	p_log_cntt->log_seq = 1;
}

int write_udp_log(int h_sn, int log_level, const char *file_name, const char *func_name, const int line_num, const char *format, ...)
{
	log_cntt_t * p_log_cntt = NULL;
	log_pkt_header_t * p_pkt_header = NULL;
	int curr_index = 0;
	char * base_file_name = NULL;
	uint64_t log_seq = 0;
	char curr_time_string[MAX_CURR_TIME_STRING_LEN];
	va_list ap;

	p_log_cntt = (log_cntt_t *)pthread_getspecific(log_key);
	if (p_log_cntt == NULL)
	{
		printf("write_udp_log---p_log_cntt == NULL---> %s --> %s --> L%d : call init_log() first !!! \r\n", __FILE__, __FUNCTION__, __LINE__);
		exit(-1);
	}

	if (h_sn < 0 || h_sn >= LOG_CNTT_CNTS)
	{
		printf("write_udp_log---h_sn < 0 || h_sn >= LOG_CNTT_CNTS---> %s --> %s --> L%d : h_sn:%d illegal \r\n", __FILE__, __FUNCTION__, __LINE__, h_sn);
		return 0;
	}

	p_log_cntt = &(p_log_cntt[h_sn]);
	/*****************调整过的**********************/
	if (log_level > p_log_cntt->log_level)
	{
		return 0;
	}
	else if (log_level < LOG_EMERG)
	{
		log_level = LOG_EMERG;
	}
	p_pkt_header = (log_pkt_header_t *)(p_log_cntt->log_buffer);
	p_log_cntt->log_buffer[MAX_LOG_BUFFER_LEN] = 0;
	log_seq = p_log_cntt->log_seq++;

	memcpy(p_pkt_header->app_name_ext, p_log_cntt->app_name_ext, sizeof(p_pkt_header->app_name_ext));
	p_pkt_header->u64_ext = 0UL;
	p_pkt_header->log_seq = hton64(log_seq);
	p_pkt_header->command = 0;
	p_pkt_header->log_level = log_level;

	get_curr_time_string(curr_time_string, sizeof(curr_time_string));

	base_file_name = basename((char *)file_name);
	curr_index = sizeof(log_pkt_header_t);
	curr_index += snprintf(&(p_log_cntt->log_buffer[curr_index]), MAX_LOG_BUFFER_LEN - curr_index,
		"<%s>[%s][%ld][%s:%s():%d]:", get_str_log_level(log_level),
		curr_time_string, log_seq, base_file_name, func_name, line_num);
	va_start(ap, format);
	curr_index += vsnprintf(&(p_log_cntt->log_buffer[curr_index]), MAX_LOG_BUFFER_LEN - curr_index - 2, format, ap);
	va_end(ap);

	p_log_cntt->log_buffer[curr_index++] = '\r';
	p_log_cntt->log_buffer[curr_index++] = '\n';
	p_log_cntt->log_buffer[curr_index] = 0;
	/*****************end**********************/
	/*自己添加的-->处理远端和本地log*/
	if (log_type == LOCAL_FLAG)
	{
		return write_local_log(p_log_cntt->log_buffer + sizeof(log_pkt_header_t), curr_index - sizeof(log_pkt_header_t));
	}
	/*********到这结束(结束自己添加的呢*_*)******************************/

	if (p_log_cntt->log_fd < 0)
	{
		printf("write_udp_log--p_log_cntt->log_fd < 0----> %s --> %s --> L%d : p_log_cntt[%d] not init \r\n", __FILE__, __FUNCTION__, __LINE__, h_sn);
		return 0;
	}

	return sendto(p_log_cntt->log_fd, p_log_cntt->log_buffer, curr_index, 0, (struct sockaddr *)&(p_log_cntt->server_address), sizeof(struct sockaddr_in));
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// user define 
// 封装的写函数，保证数据的完整性/又写了根据时间获取名字的函数
std::string getLogFileName(std::string app_name)
{
	log_cntt_t * p_log_cntt = (log_cntt_t *)pthread_getspecific(log_key);
	if (p_log_cntt == NULL)
	{
		exit(-1);
	}

	std::string gameName = app_name;
	std::stringstream ss;
	time_t now = time(NULL);
	p_log_cntt->timeinfo = *localtime(&now);
	char date[40];
	strftime(date, sizeof(date), "%Y%m%d", &(p_log_cntt->timeinfo));
    char path[128];
    snprintf(path, sizeof(path), "%s%s/", log_dir, date);

    if (access(path, F_OK) != 0)
    {
        mkdir(path, 0777);
    }

	ss << path;
	ss << gameName;
    ss << ".";
	ss << date;
	ss << ".log";
	std::string logFileName;
	logFileName = ss.str();

	return logFileName;
}

/* 获取当前时间判断是否过了零点 */
bool judgeCurrTimer()
{
	log_cntt_t * p_log_cntt = (log_cntt_t *)pthread_getspecific(log_key);
	if (p_log_cntt == NULL)
	{
		printf("judgeCurrTimer---> call init_log(...) first !!! \r\n");
		exit(-1);
	}

	time_t curtime = time(0);
	tm tim = *localtime(&curtime);
	int day, mon, year;
	day = tim.tm_mday;
	mon = tim.tm_mon;
	year = tim.tm_year;

	if ((year != p_log_cntt->timeinfo.tm_year)
		|| (mon != p_log_cntt->timeinfo.tm_mon)
		|| (day != p_log_cntt->timeinfo.tm_mday))
	{
		return true;
	}
	return false;
}

/*获取文件并打开，自动判别文件的回滚*/
bool getNewFileOpened()
{
	log_cntt_t * p_log_cntt = (log_cntt_t *)pthread_getspecific(log_key);
	if (p_log_cntt == NULL)
	{
		printf("getNewFileOpened--> call init_log(...) first !!! \r\n");
		exit(-1);
	}

	p_log_cntt->os.close();
	std::string strNewFileName = getLogFileName(p_log_cntt->app_name_ext);
	if (p_log_cntt->curFileName == strNewFileName)
	{
		char szIndex[10] = { 0 };
		//相同文件名的滚动
		for (int i = FILE_COUNTER-2; i >= 0; --i)
		{
			if (i==0)
			{
				sprintf(szIndex, "%s", "");
			}
			else
			{
				sprintf(szIndex, ".%02d", i);
			}
			std::string strOldFileName = strNewFileName + szIndex;
			sprintf(szIndex, ".%02d", i + 1);
			std::string strRenameFileName = strNewFileName + szIndex;
			rename(strOldFileName.c_str(), strRenameFileName.c_str());
		}
	}
	p_log_cntt->curFileName = strNewFileName;

	char* newFILE_NAME = (char*)p_log_cntt->curFileName.data();
	//std::cout << "open new file OK " << newFILE_NAME << std::endl;
	p_log_cntt->os.open(newFILE_NAME, std::ios_base::out | std::ios_base::app);
	if (p_log_cntt->os)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int write_local_log(char* ptr, int curr_index)
{
	log_cntt_t * p_log_cntt = (log_cntt_t *)pthread_getspecific(log_key);
	if (p_log_cntt == NULL)
	{
		printf("----write_local_log---- call init_log(...) first !!! \r\n");
		exit(-1);
	}

	std::string content = ptr;

	if (judgeCurrTimer())
	{
		getNewFileOpened();
		p_log_cntt->os << content;
	}
	else
	{
		struct stat stat_buf;
		char* FILE_NAME = (char*)p_log_cntt->curFileName.data();
		stat(FILE_NAME, &stat_buf);
		if ((stat_buf.st_size<FILE_SIZE) && ((FILE_SIZE - stat_buf.st_size) >(curr_index)))//判断文件大小
		{
			p_log_cntt->os << content;
		}
		else
		{
			if (getNewFileOpened())
			{
				p_log_cntt->os << content;
			}
			else
			{
				printf("Please check your log dir!\n");
				exit(-1);
			}
		}
	}

	if (p_log_cntt->os.is_open())
	{
		p_log_cntt->os.flush();
	}

	return 0;
}

// test example
int TestUdplog(void)
{
    std::string app = "test_app_name";
    char* FILE_name = (char*)app.data();
    set_log_type(LOCAL_FLAG);
    init_log(FILE_name, 1, 1, 1, 0);
    set_log_level(LOG_DEBUG);

    for (int i = 0; i < 900; i++)
    {
        log_debug("debug log: show debug");
        log_info("info log: show info log");
        log_notice("notice log 012345678901234567890123456789012345678901234567890123456789");
        log_warning("warning log 012345678901234567890123456789012345678901234567890123456789");
        log_error("err log 012345678901234567890123456789012345678901234567890123456789");
        log_crit("crit log 012345678901234567890123456789012345678901234567890123456789");
        log_alert("alert log 012345678901234567890123456789012345678901234567890123456789");
        log_emerg("emerg log 012345678901234567890123456789012345678901234567890123456789");
    }

    close_log();

    return 0;
}
