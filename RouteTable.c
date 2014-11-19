#include "RouteTable.h"
#include "string.h"

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

