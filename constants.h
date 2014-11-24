#ifndef CONSTANTS_H
#define CONSTANTS_H

#define DEBUG
//#define LDEBUG
#define IP_LEN (18)
#define MAC_LEN (6)
#define MSG_LEN (2)
#define VM_NUM (10)
#define MAX_EPORT (65535)
#define MIN_EPORT (64536)
#define IF_NAME_LEN (10)
#define APP_DATA_LEN (50)

#define ERR_CREATE_TEMP_SOCK_FILE "Create temp file failed!"
#define ERR_PTAB_OVERFLOW "Too many entries in path table!"
#define ERR_SELECT "select error"
#define ERR_RECVFROM "recvfrom error"
#define ERR_SEND_RAW_DATA "Sending data via raw socket"
#define ERR_CREATE_RAW_SOCK "Creating raw socket failed!"
#define ERR_RAW_RECV "Receiving data via raw socket."
#define ERR_GET_PATH_BY_PORT "No path corresponds to port"
#define ERR_ROUTE_NOT_EXIST "route does not exist"

#define SERV_WK_PATH "odr399621"
#define SERV_WK_PORT (39962)
#define ODR_WK_PATH "odr079356"
#define ODR_WK_PORT (7935)
//#define ODR_BROADCAST_MAC "ff:ff:ff:ff:ff:ff"
#define ODR_BROADCAST_MAC {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}
#define PTAB_ENT_LIFETIME (3)

#define ETH_PROT_VALUE (10939)
#define ETH_DATA_LEN (1496)
#define ETH_DATA_OFFSET (14)

#define RREQ_SIZE (12+2*IP_LEN)
#define RREP_SIZE (3+2*IP_LEN)
#define APPMSG_SIZE (11+2*IP_LEN+MSG_LEN)

#endif
