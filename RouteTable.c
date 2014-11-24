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

void deleteRTabEnt(RTab_t* tbl, RTabEnt_t *ent) {
#ifdef DEBUG
    printf("Deleted routing table entry <%s>\n", ent->destIP);
#endif
    if (ent->prev != NULL) {
        ent->prev->next = ent->next;
    } else {
        tbl->head = ent->next;
    }

    if (ent->next != NULL) {
        ent->next->prev = ent->prev;
    } else {
        tbl->tail = ent->prev;
    }
    free(ent);
}

void deleteRTabEntsToDest(RTab_t *tbl, const char* destIP) {
    RTabEnt_t *e = getRTabEntByDest(tbl, destIP);
    if (e == NULL ) {
        return;
    }
    deleteRTabEnt(tbl, e);
}

void clearRTab(RTab_t* tbl) {
#ifdef DEBUG
    prtMsg("Clearing routing table...");
#endif
    RTabEnt_t *p;
    while ((p = tbl->head) != NULL) {
        deleteRTabEnt(tbl, p);
    }
#ifdef DEBUG
    prtMsg("Finished!");
#endif
}

void deleteStaleRTabEnts(RTab_t *t, const unsigned long int staleness) {
#ifdef DEBUG
    prtMsg("Deleting stale routing table entries...");
#endif
    int ctr = 0;
    RTabEnt_t *p = t->head;
    while (p != NULL) {
        RTabEnt_t *next = p->next;
        if (isRTabEntStale(p, staleness)) {
            deleteRTabEnt(t, p);
            ++ctr;
        }
        p = next;
    }
#ifdef DEBUG
    printf("%d stale entries deleted!\n", ctr);
#endif
}

int isRTabEntStale(const RTabEnt_t* e, const unsigned long stale) {
    return time(NULL) - e->lastUpdated > stale;
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

int isRTabEntEqual(const RTabEnt_t *ent, const unsigned char* nextNode, 
        const int outIfIndex, const int distToDest) {
    //destIP should already be same
    return (memcmp(nextNode, ent->nextNode, MAC_LEN) == 0
            && ent->outIfIndex == outIfIndex
            && ent->distToDest == distToDest
           );
}

RTabEnt_t *confirmRTabEnt(RTabEnt_t *ent) {
    ent->lastUpdated = time(NULL);
#ifdef DEBUG
    printf("Routing table entry <%s> confirmed with new time %lu\n", 
            ent->destIP, ent->lastUpdated);
#endif
    return ent;
}
