#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_STR         64
#define MAX_RUNS        50
#define BOREDOM_MAX     100
#define C_TRUE          1
#define C_FALSE         0
#define HUNTER_WAIT     5000
#define GHOST_WAIT      600
#define NUM_HUNTERS     4
#define FEAR_MAX        10
#define LOGGING         C_TRUE

//made constant
#define MAX_EVIDENCE    3

typedef enum EvidenceType EvidenceType;
typedef enum GhostClass GhostClass;

typedef     struct  Ghost   GhostType;
typedef     struct  Room    RoomType;
typedef     struct  House   HouseType;
typedef     struct  RoomList    RoomListType;
typedef     struct  RoomNode    RoomNodeType;
typedef     struct  EvidenceList    EvidenceListType;
typedef     struct  EvidenceNode    EvidenceNodeType;
typedef     struct  Hunter   HunterType;
typedef    struct  EvidenceArray EvidenceArrayType;
typedef    struct  HunterArray HunterArrayType;
typedef    struct  sharedState SharedGameState;



enum EvidenceType { EMF, TEMPERATURE, FINGERPRINTS, SOUND, EV_COUNT, EV_UNKNOWN };
enum GhostClass { POLTERGEIST, BANSHEE, BULLIES, PHANTOM, GHOST_COUNT, GH_UNKNOWN };
enum LoggerDetails { LOG_FEAR, LOG_BORED, LOG_EVIDENCE, LOG_SUFFICIENT, LOG_INSUFFICIENT, LOG_UNKNOWN };

// room structs
struct Room {
    char name[MAX_STR];
    //linked list initialization
    RoomListType *roomlist; 
    EvidenceListType *evidencelist;
    //collection of hunters
    HunterArrayType *hunterArray;
    //pointer to ghost 
    GhostType *ghost;


};

struct Ghost {
  GhostClass ghostType;
  RoomType *room;
  int boredomTime;
  
};

struct RoomNode {
    RoomType *room;
    struct RoomNode *next;
};

struct RoomList {
  //linked list initialization
  RoomNodeType *rhead;
  RoomNodeType *rtail;
  int size; 
  //semaphore 
    sem_t sem;
} ;

 struct House{
    RoomListType* rooms;
    HunterArrayType* hunterArray;
    EvidenceArrayType* evidenceArray;
    int hunterCount;

};
//build linked list evidence list
struct EvidenceList {
  //linked list initialization
  EvidenceNodeType *ehead;
  EvidenceNodeType *etail;
    //semaphore 
    sem_t sem;
} ;

struct EvidenceNode {
    enum EvidenceType evidence;
    struct EvidenceNode *next;
} ;

//hunter struct
struct Hunter {
    char name[MAX_STR];
    EvidenceType equipment;
    //evidence list
    EvidenceArrayType *evidenceArray; 
    int fear;
    int boredom;
    RoomType *room;
    pthread_t thread;
} ;

struct EvidenceArray {
    EvidenceType *evidence;
    //size and capacity (don't know if its needed)
    int size;
    int capacity;
    //semaphore
    sem_t sem;
} ;

struct HunterArray {
    HunterType *hunter;
    //size and capacity (don't know if its neeedd)
    int size;
    int capacity;
    //semaphore (don't know if its needed)
    sem_t sem;
};

typedef struct GhostBehaviorContext {
    GhostType* ghost;
    HunterArrayType* hunters; // Assuming this is an array or similar
    int numHunters;
    HouseType* house;
    SharedGameState *sharedState;  // Pointer to the shared game state

} GhostBehaviorContext;

typedef struct HunterBehaviorContext {
    HunterType *hunter;            // Pointer to this hunter's data
    GhostType *ghosts;             // Pointer to the ghosts, if hunters need to interact with or detect ghosts
    HouseType *house;              // Pointer to the house layout
    EvidenceArrayType *sharedEvidence; // Pointer to a shared evidence array          
    HunterArrayType *allHunters;        // Pointer to an array of all hunters, if interaction between hunters is required
    SharedGameState *sharedState;  // Pointer to the shared game state

} HunterBehaviorContext;

struct sharedState{
    int gameOver;
};


// Helper Utilies
int randInt(int,int);        // Pseudo-random number generator function
float randFloat(float, float);  // Pseudo-random float generator function
enum GhostClass randomGhost();  // Return a randomly selected a ghost type
void ghostToString(enum GhostClass, char*); // Convert a ghost type to a string, stored in output paremeter
void evidenceToString(enum EvidenceType, char*); // Convert an evidence type to a string, stored in output parameter

// Logging Utilities
void l_hunterInit(char* name, enum EvidenceType equipment);
void l_hunterMove(char* name, char* room);
void l_hunterReview(char* name, enum LoggerDetails reviewResult);
void l_hunterCollect(char* name, enum EvidenceType evidence, char* room);
void l_hunterExit(char* name, enum LoggerDetails reason);
void l_ghostInit(enum GhostClass type, char* room);
void l_ghostMove(char* room);
void l_ghostEvidence(enum EvidenceType evidence, char* room);
void l_ghostExit(enum LoggerDetails reason);

//house functions
void initHouse(HouseType *house);
void populateRooms(HouseType* house);
void freeHouse(HouseType *house);


//evidence list functions
void initEvidenceList(EvidenceListType *list);
void initEvidence(EvidenceType *evidence, enum EvidenceType type);
void initEvidenceArray(EvidenceArrayType *evidenceArray, int size);
EvidenceType addEvidence(GhostType* ghost);
int reviewEvidence(EvidenceArrayType *evidenceArray, GhostType *ghost);
EvidenceType isEvidencePresent(RoomType *room, EvidenceType hunterEquipment);
int collectEvidence(EvidenceArrayType *evidenceArray, EvidenceType evidence);
void freeEvidenceList(EvidenceListType *evidenceList);
void freeEvidenceArray(EvidenceArrayType *evidenceArray);

//hunter functions
void initHunterArray(HunterArrayType *hunterArray, int size);
void initHunter(HunterType *hunter, const char *name, EvidenceType equipment, RoomType *room); 
int isHunterPresent(GhostType* ghost, HunterArrayType* list);
void *hunterBehav(void *param);
int addHunter(HunterArrayType *hunterArray, const HunterType *newHunter);
void moveToRandomRoomHunter(HunterType *hunter, HouseType *house);
void updateHunterState(HunterType *hunter, GhostType *ghosts, HouseType *house, EvidenceArrayType *sharedEvidence, SharedGameState *sharedState); 
void assignRandomEquipment(HunterArrayType* hunters, int numHunters);
void removeHunter(HunterArrayType *hunters_list, HunterType* hunter);
void clearHunterArray(HunterArrayType *hunterArray);
void initHunterBehaviorContext(HunterBehaviorContext *context, HunterType *hunter, GhostType *ghosts, HouseType *house, EvidenceArrayType *sharedEvidence, SharedGameState *sharedState); 
void freeHunter(HunterType* hunter);
void freeHunterArray(HunterArrayType *hunterArray);

//ghost functions 
void initGhost(GhostType *ghost, enum GhostClass type, RoomType *room);
void *ghostBehav(void *param);
void updateGhostState(GhostType *ghost, HunterArrayType *hunters, SharedGameState *sharedState); 
int isGhostPresent(GhostType* ghost, HunterType *hunter);
void moveToRandomRoomGhost(GhostType *ghost);
void freeGhost(GhostType *ghost);
void initGhostBehaviorContext(GhostBehaviorContext *context, GhostType *ghost, HouseType *house, HunterArrayType *hunters, SharedGameState *sharedState); 

//initialization functions 
RoomType* createRoom(const char *name);
void initRoom(RoomType *room, const char *name); 
void initRoomList(RoomListType *list);
void addRoom(RoomListType* list, RoomType* room);
void connectRooms(RoomType* room1, RoomType* room2);
RoomType* getRandomRoom(HouseType *house);
void freeRoom(RoomType *room); 
void freeRoomListHouse(RoomListType *roomList);
void freeRoomList(RoomListType *roomList);
RoomType* getRandomRoomExcludeVan(HouseType *house); 