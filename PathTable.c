#include "PathTable.h"
#include "constants.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"
#include "stdio.h"
#include "utility.h"

PTab_t *createPathTable() {
    PTab_t* p = malloc(sizeof(*p));
    p->head = NULL;
    p->tail = NULL;
    p->portList.head = NULL;
    p->portList.tail = NULL;
    for (int i = MAX_EPORT; i >= MIN_EPORT; --i) {
        portList_pushBack(&p->portList, i);
    }
    return p;
}

PTabEnt_t *addToPathTable(PTab_t* pathTable, const int port, const char* path, const unsigned int lifetime) {
    PTabEnt_t *p = malloc(sizeof(*p));
    p->createTime = time(NULL);
    p->lifetime = lifetime;
    p->port = port;
    p->next = NULL;
    p->prev = pathTable->tail;
    strcpy(p->sunPath, path);
    if (pathTable->tail == NULL) {
        pathTable->head = p;
        pathTable->tail = p;
    } else {
        pathTable->tail->next = p;
        pathTable->tail = p;
    }
#ifdef DEBUG
    printf("New path table entry: <port %d, path %s>\n", port, path);
#endif
    return p;
}

PTabEnt_t *findPTabEntByPath(const PTab_t* pathTable, const char* sunPath) {
    PTabEnt_t *p = pathTable->head;
    while (p != NULL) {
        if (strcmp(sunPath, p->sunPath) == 0) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

PTabEnt_t *findPTabEntByPort(const PTab_t* pathTable, const int port) {
    PTabEnt_t *p = pathTable->head;
    while (p != NULL) {
        if (p->port == port) {
            return p;
        }
        p = p->next;
    }
}

void deletePTabEnt(PTab_t* t, PTabEnt_t* e) {
#ifdef DEBUG
    printf("Deleted <port-path> entry <%d, %s>\n", e->port, e->sunPath);
#endif
    int port = e->port;
    if (e->prev != NULL) {
        e->prev->next = e->next;
    } else {
        t->head = e->next;
    }

    if (e->next != NULL) {
        e->next->prev = e->prev;
    } else {
        t->tail = e->prev;
    }
    free(e);

    portList_pushBack(&t->portList, port);
}

void confirmPTabEnt(PTabEnt_t* e) {
    e->createTime = time(NULL);
#ifdef DEBUG
    printf("<port-path> entry <%d, %s> confirmed at time %lu\n",
            e->port, e->sunPath, e->createTime);
#endif
}

int isPTabEntExpired(const PTabEnt_t* e) {
    if (e->lifetime == 0) {
        return 0;
    } else {
        return time(NULL) - e->createTime > e->lifetime;
    }
}

void deleteExpiredPTabEnts(PTab_t* t) {
#ifdef DEBUG
    prtMsg("Deleting expired <port-path> entries");
#endif
    PTabEnt_t *p = t->head;
    while (p != NULL) {
        PTabEnt_t *next = p->next;
        if (isPTabEntExpired(p)) {
            deletePTabEnt(t, p);
        }
        p = next;
    }
}

AvailPort_t *portList_pushBack(PortList_t* plist, const int port) {
    AvailPort_t *newElem = malloc(sizeof(*newElem));
    newElem->next = NULL;
    newElem->prev = plist->tail;
    newElem->port = port;

    if (plist->tail == NULL) {
        plist->tail = newElem;
        plist->head = newElem;
    } else {
        plist->tail->next = newElem;
        plist->tail = newElem;
    }
    return newElem;
}

int portList_popFront(PortList_t* plist) {
    if (plist->head == NULL) {
        return 0;
    }
    int r = plist->head->port;
    portList_remove(plist, plist->head);
    return r;
}


void portList_remove(PortList_t* plist, AvailPort_t *target) {
    if (target != plist->head) {
        target->prev->next = target->next;
    } else {
        plist->head = target->next;
    }
    
    if (target != plist->tail) {
        target->next->prev = target->prev;
    } else {
        plist->tail = target->prev;
    }
    free(target);
}

int portList_isEmpty(PortList_t* plist) {
    return (plist->head == NULL);
}

int getNewPTabPort(PTab_t* pathTable) {
    PortList_t *pl = &pathTable->portList;
    if (portList_isEmpty(pl)) {
        prtErr(ERR_PTAB_OVERFLOW);
        return 0;
    } else {
        return portList_popFront(pl);
    }
}

#ifdef DEBUG
void prtPortList(const PTab_t* pathTable) {
    printf("port list:\n");
    AvailPort_t *p = pathTable->portList.head;
    while (p != NULL) {
        printf("%d ", p->port);
        p = p->next;
    }
}
#endif
