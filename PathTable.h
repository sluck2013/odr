#ifndef PATHTABLE_H
#define PATHTABLE_H

typedef struct PathTableEntry {
    unsigned long int createTime;
    unsigned int lifetime;
    unsigned int port;
    char sunPath[108];
    struct PathTableEntry *next;
    struct PathTableEntry *prev;
} PTabEnt_t;

typedef struct AvailablePort {
    unsigned int port;
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
PTabEnt_t *addToPathTable(PTab_t* pathTable, const unsigned int port, const char* path, const unsigned int lifetime);
PTabEnt_t *findPTabEntByPath(const PTab_t* pTable, const char* sunPath);
AvailPort_t *portList_pushBack(PortList_t *plist, const unsigned int port);
unsigned int getNewPTabPort(PTab_t* pathTable);

AvailPort_t *portList_pushBack(PortList_t* plist, const unsigned int port);
unsigned int portList_popFront(PortList_t* plist);
void portList_remove(PortList_t* plist, AvailPort_t *target);
int portList_isEmpty(PortList_t* plist);

#ifdef DEBUG
void prtPortList(const PTab_t* pathTable);
#endif

#endif
