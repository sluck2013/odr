#ifndef PATHTABLE_H
#define PATHTABLE_H

typedef struct PathTableEntry {
    unsigned long int createTime;
    unsigned int lifetime;
    int port;
    char sunPath[108];
    struct PathTableEntry *next;
    struct PathTableEntry *prev;
} PTabEnt_t;

typedef struct AvailablePort {
    int port;
    struct AvailablePort *next;
    struct AvailablePort *prev;
} AvailPort_t;

typedef struct AvailablePortList {
    AvailPort_t *head;
    AvailPort_t *tail;
} PortList_t;

typedef struct PathTable {
    PTabEnt_t *head;
    PTabEnt_t *tail;
    PortList_t portList;
} PTab_t;

PTab_t *createPathTable(); 
PTabEnt_t *addToPathTable(PTab_t* pathTable, const int port, const char* path, const unsigned int lifetime);
PTabEnt_t *findPTabEntByPath(const PTab_t* pTable, const char* sunPath);
PTabEnt_t *findPTabEntByPort(const PTab_t* pTable, const int port);
AvailPort_t *portList_pushBack(PortList_t *plist, const int port);
int getNewPTabPort(PTab_t* pathTable);
void deletePTabEnt(PTab_t* t, PTabEnt_t* e);
void deleteExpiredPTabEnts(PTab_t* t);

int portList_popFront(PortList_t* plist);
void portList_remove(PortList_t* plist, AvailPort_t *target);
int portList_isEmpty(PortList_t* plist);
void confirmPTabEnt(PTabEnt_t* e);

#ifdef DEBUG
void prtPortList(const PTab_t* pathTable);
#endif

#endif
