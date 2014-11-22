#ifndef CONSTANTS_H
#define CONSTANTS_H

#define DEBUG
//#define LDEBUG
#define IP_LEN (16)
#define MAC_LEN (18)
#define VM_NUM (10)
#define MAX_EPORT (65535)
#define MIN_EPORT (64536)

#define ERR_CREATE_TEMP_SOCK_FILE "Create temp file failed!"
#define ERR_PTAB_OVERFLOW "Too many entries in path table!"
#define ERR_SELECT "select error"
#define ERR_RECVFROM "recvfrom error"
#define ERR_SEND_RAW_DATA "Sending data via raw socket"
#define ERR_CREATE_RAW_SOCK "Creating raw socket failed!"

#define SERV_WK_PATH "109399621"
#define SERV_WK_PORT (39962)
#define ODR_WK_PATH "109079356"
#define ODR_WK_PORT (7935)
#define ODR_BROADCAST_MAC "ff:ff:ff:ff:ff:ff"
#define PTAB_ENT_LIFETIME (3)

#ifdef LDEBUG
#define ODR_IF_NAME "wlan0"
#define ETH_IF_INDEX (3)
#else
#define ODR_IF_NAME "eth0"
#define ETH_IF_INDEX (2)
#endif

#define ETH_PROT_VALUE (10939)
#define ETH_DATA_LEN (1496)

#endif
