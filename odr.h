#ifndef ODR_H
#define ODR_H

#include "constants.h"

typedef struct PathTableElement {
    unsigned long int createTime;
    unsigned int lifetime;
    unsigned int port;
    char sunPath[108];
    struct PathTableElement *next;
    struct PathTableElement *prev;
} PTabElem_t;

typedef struct PathTable {
    PTabElem_t *head;
    PTabElem_t *tail;
} PTab_t;

typedef struct RouteTableSubElement {
    char nextNode[IP_LEN];
    int outIfIndex;
    int distToDest;
    unsigned long int lastUpdated;
    struct RouteTableSubElement *next;
    struct RouteTableSubElement *prev;
} RTabSubElem_t;

typedef struct RouteTableElement {
    char destIP[IP_LEN];
    RTabSubElem_t *head;
    RTabSubElem_t *tail;
} RTabElem_t;

typedef struct RouteTable {
    RTabElem_t *head;
    RTabElem_t *tail;
} RTab_t;

PTab_t *createPathTable(); 
PTabElem_t *addToPathTable();

#endif
