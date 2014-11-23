#include "RouteTable.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "utility.h"
#include "stdio.h"

RTab_t *createRouteTable() {
    RTab_t* t = malloc(sizeof(*t));
    t->head = NULL;
    t->tail = NULL;
    return t;
}

void deleteRouteTable(RTab_t* t) {
    //for 

}

int isRTabEmpty(const RTab_t* t) {
    return t->head == NULL;
}

RTabEnt_t *getRTabEntByDest(const RTab_t* rTable, const char* destIP) {
    RTabEnt_t *p = rTable->head;
    while (p != NULL) {
        if (strcmp(p->destIP, destIP) == 0) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

RTabEnt_t *addToRTab(RTab_t *table, const char* destIP, 
        const unsigned char* nextNodeMac, 
        const int outIfIndex, const int distToDest) {
    RTabEnt_t *newEnt = malloc(sizeof(RTabEnt_t));
    strcpy(newEnt->destIP, destIP);
    memcpy(newEnt->nextNode, nextNodeMac, MAC_LEN);
    newEnt->outIfIndex = outIfIndex;
    newEnt->distToDest = distToDest;
    newEnt->lastUpdated = time(NULL);
    newEnt->next = NULL;
    newEnt->prev = table->tail;
    if (isRTabEmpty(table)) {
        table->head = newEnt;
        table->tail = newEnt;
    } else {
        table->tail->next = newEnt;
        table->tail = newEnt;
    }
#ifdef DEBUG
    printf("New routing table entry added at %lu:\n", newEnt->lastUpdated);
    prtItemStr("dest", destIP);
    prtMac("next node", nextNodeMac);
    printf("interface: %d, distance to dest: %d\n\n", outIfIndex, distToDest);
#endif
    return newEnt;
}

RTabEnt_t *updateRTabEnt(RTabEnt_t *ent, const unsigned char* nextNodeMac, 
        const int outIfIndex, const int distToDest) {
    if (strcmp(nextNodeMac, "") != 0) {
        memcpy(ent->nextNode, nextNodeMac, MAC_LEN);
        prtMac("next node", nextNodeMac);
    }
    if (outIfIndex >= 0) {
        ent->outIfIndex = outIfIndex;
    }
    if (distToDest >= 0) {
        ent->distToDest = distToDest;
    }
    ent->lastUpdated = time(NULL);
#ifdef DEBUG
    printf("Routing table entry %s updated at %lu:\n", ent->destIP, ent->lastUpdated);
    prtMac("next node", nextNodeMac);
    printf("interface: %d, distance to dest: %d\n\n", outIfIndex, distToDest);
#endif
    return ent;
}

