
#ifndef LOG_UDP_H
#define LOG_UDP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <endian.h>
#include <libgen.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <algorithm>

#define MAX_APP_NAME_LEN    47

#define	LOG_EMERG	0	/* system is unusable */
#define	LOG_ALERT	1	/* action must be taken immediately */
#define	LOG_CRIT	2	/* critical conditions */
#define	LOG_ERROR	3	/* error conditions */
#define	LOG_WARNING	4	/* warning conditions */
#define	LOG_NOTICE	5	/* normal but significant condition */
#define	LOG_INFO	6	/* informational */
#define	LOG_DEBUG	7	/* debug-level messages */

#define	LOG_BOOT	3	
#define	LOG_FATAL	3	
#define	LOG_TRACE	7	

void init_log(char * app_name, uint32_t svr_type = 0, uint32_t svr_id = 0, int t_sn = 0, int h_sn = 0);
void set_log_level(int log_level, int h_sn = 0);
void set_log_type(int LogType);
void close_log(int h_sn = 0);

int write_udp_log(int h_sn, int log_level, const char * file_name, const char * func_name, const int line_num,
	const char * format, ...) __attribute__((__format__(__printf__, 6, 7)));

#define log_debug(format, args...)      write_udp_log(0, LOG_DEBUG, __FILE__, __FUNCTION__, __LINE__, format, ##args)
#define log_info(format, args...)       write_udp_log(0, LOG_INFO, __FILE__, __FUNCTION__, __LINE__, format, ##args)
#define log_notice(format, args...)     write_udp_log(0, LOG_NOTICE, __FILE__, __FUNCTION__, __LINE__, format, ##args)
#define log_warning(format, args...)    write_udp_log(0, LOG_WARNING, __FILE__, __FUNCTION__, __LINE__, format, ##args)
#define log_error(format, args...)      write_udp_log(0, LOG_ERROR, __FILE__, __FUNCTION__, __LINE__, format, ##args)
#define log_crit(format, args...)       write_udp_log(0, LOG_CRIT, __FILE__, __FUNCTION__, __LINE__, format, ##args)
#define log_alert(format, args...)      write_udp_log(0, LOG_ALERT, __FILE__, __FUNCTION__, __LINE__, format, ##args)
#define log_emerg(format, args...)      write_udp_log(0, LOG_EMERG, __FILE__, __FUNCTION__, __LINE__, format, ##args)

// add log_boot/logtrace
#define log_boot(format, args...)      write_udp_log(0, LOG_BOOT, __FILE__, __FUNCTION__, __LINE__, format, ##args)
#define log_trace(format, args...)      write_udp_log(0, LOG_TRACE, __FILE__, __FUNCTION__, __LINE__, format, ##args)
#define log_fatal(format, args...)      write_udp_log(0, LOG_FATAL, __FILE__, __FUNCTION__, __LINE__, format, ##args)

#define log_debug_ext(h_sn, format, args...)    write_udp_log(h_sn, LOG_DEBUG, __FILE__, __FUNCTION__, __LINE__, format, ##args)
#define log_info_ext(h_sn, format, args...)     write_udp_log(h_sn, LOG_INFO, __FILE__, __FUNCTION__, __LINE__, format, ##args)
#define log_notice_ext(h_sn, format, args...)   write_udp_log(h_sn, LOG_NOTICE, __FILE__, __FUNCTION__, __LINE__, format, ##args)
#define log_warning_ext(h_sn, format, args...)  write_udp_log(h_sn, LOG_WARNING, __FILE__, __FUNCTION__, __LINE__, format, ##args)
#define log_error_ext(h_sn, format, args...)    write_udp_log(h_sn, LOG_ERROR, __FILE__, __FUNCTION__, __LINE__, format, ##args)
#define log_crit_ext(h_sn, format, args...)     write_udp_log(h_sn, LOG_CRIT, __FILE__, __FUNCTION__, __LINE__, format, ##args)
#define log_alert_ext(h_sn, format, args...)    write_udp_log(h_sn, LOG_ALERT, __FILE__, __FUNCTION__, __LINE__, format, ##args)
#define log_emerg_ext(h_sn, format, args...)    write_udp_log(h_sn, LOG_EMERG, __FILE__, __FUNCTION__, __LINE__, format, ##args)

#define __ASSERT_AND_LOG(X) \
if (!(X)) { log_error("%s is invalid\n", #X); }

#define ASSET_AND_RETURN_INT(X) \
__ASSERT_AND_LOG(X); if (!(X)) return -1

#define ASSET_AND_RETURN_INT_FORMAT(X, format, ...) \
if (!(X)) { log_error("%s is invalid," format, #X, ##__VA_ARGS__); }; if (!(X)) return -1

#define ASSET_AND_RETURN_VOID(X) \
__ASSERT_AND_LOG(X); if (!(X)) return

#define ASSET_AND_RETURN_PTR(X) \
__ASSERT_AND_LOG(X); if (!(X)) return NULL

#define ASSET_AND_RETURN_EX(X, ret) \
__ASSERT_AND_LOG(X); if (!(X)) return ret

#define ASSET_AND_RETURN_BOOL(X) \
__ASSERT_AND_LOG(X); if (!(X)) return false


/*添加了两个宏定义*/
#define ASSET_AND_CONTINUE(X) \
    __ASSERT_AND_LOG(X); if (!(X)) continue

#define ASSET_AND_BREAK(X) \
    __ASSERT_AND_LOG(X); if (!(X)) break;

#define log_player(uid, format, ...) \
if (IsTraceUid(uid)) {log_info("uid:%d " format, #uid, ##__VA_ARGS__); };



#endif

