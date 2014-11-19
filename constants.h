#ifndef CONSTANTS_H
#define CONSTANTS_H

#define DEBUG
#define LDEBUG
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

#define SERV_WK_PATH "109399621"
#define SERV_WK_PORT (39962)
#define ODR_WK_PATH "109079356"
#define ODR_WK_PORT (7935)
#define ODR_BROADCAST_MAC "ff:ff:ff:ff:ff:ff"
#define PTAB_ENT_LIFETIME (3)

#ifdef LDEBUG
#define ODR_IF_NAME "wlan0"
#else
#define ODR_IF_NAME "eth0"
#endif

#define ETH_PROT_VALUE (10939)
#define ETH_IF_INDEX (2)
#define ETH_FRAME_LEN (1518)

#define ASCII_0 48
#define ASCII_9 57
#define ASCII_A 65
#define ASCII_F 70

#endif
