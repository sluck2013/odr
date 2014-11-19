#ifndef ROUTETABLE_H
#define ROUTETABLE_H
#include "constants.h"
#include "stdlib.h"

typedef struct RouteTableEntry {
    char destIP[IP_LEN];
    char nextNode[IP_LEN];
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
RTabEnt_t *getRTabEntByDest(const RTab_t* rTable, const char* destIP);

#endif
