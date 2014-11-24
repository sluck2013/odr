#ifndef ROUTETABLE_H
#define ROUTETABLE_H
#include "constants.h"
#include "stdlib.h"

typedef struct RouteTableEntry {
    char destIP[IP_LEN];
    unsigned char nextNode[MAC_LEN];
    int outIfIndex;
    int distToDest;
    unsigned long int lastUpdated;
    struct RouteTableEntry *next;
    struct RouteTableEntry *prev;
} RTabEnt_t;

typedef struct RouteTable {
    RTabEnt_t *head;
    RTabEnt_t *tail;
} RTab_t;


RTab_t *createRouteTable();
void deleteRouteTable(RTab_t* t);
int isRTabEmpty(const RTab_t *t);
int isRTabEntEqual(const RTabEnt_t* ent, const unsigned char* nextNode,
        const int outIfIndex, const int distToDest);
RTabEnt_t *getRTabEntByDest(const RTab_t* rTable, const char* destIP);
RTabEnt_t *addToRTab(RTab_t *table, const char* destIP, const unsigned char *nextNodeMac, const int outIfIndex, const int distToDest);
RTabEnt_t *updateRTabEnt(RTabEnt_t *ent, const unsigned char *nextNodeMac, const int outIfIndex, const int distToDest);
RTabEnt_t *confirmRTabEnt(RTabEnt_t *ent);

#endif
