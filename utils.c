#include "defs.h"

/*
    Returns a pseudo randomly generated number, in the range min to (max - 1), inclusively
        in:   lower end of the range of the generated number
        in:   upper end of the range of the generated number
    return:   randomly generated integer in the range [0, max-1) 
*/
int randInt(int min, int max)
{
    return (int) randFloat(min, max);
}

/*
    Returns a pseudo randomly generated floating point number.
    A few tricks to make this thread safe, just to reduce any chance of issues using random
        in:   lower end of the range of the generated number
        in:   upper end of the range of the generated number
    return:   randomly generated floating point number in the range [min, max)
*/
float randFloat(float min, float max) {
    static __thread unsigned int seed = 0;
    if (seed == 0) {
        seed = (unsigned int)time(NULL) ^ (unsigned int)pthread_self();
    }

    float random = ((float) rand_r(&seed)) / (float) RAND_MAX;
    float diff = max - min;
    float r = random * diff;
    return min + r;
}

/* 
    Returns a random enum GhostClass.
*/
enum GhostClass randomGhost() {
    return (enum GhostClass) randInt(0, GHOST_COUNT);
}
// used when intialziign gohst

/*
    Returns the string representation of the given enum EvidenceType.
        in: type - the enum EvidenceType to convert
        out: str - the string representation of the given enum EvidenceType, minimum 16 characters
*/
void evidenceToString(enum EvidenceType type, char* str) {
    switch (type) {
        case EMF:
            strcpy(str, "EMF");
            break;
        case TEMPERATURE:
            strcpy(str, "TEMPERATURE");
            break;
        case FINGERPRINTS:
            strcpy(str, "FINGERPRINTS");
            break;
        case SOUND:
            strcpy(str, "SOUND");
            break;
        default:
            strcpy(str, "UNKNOWN");
            break;
    }
}

/* 
    Returns the string representation of the given enum GhostClass.
        in: ghost - the enum GhostClass to convert
        out: buffer - the string representation of the given enum GhostClass, minimum 16 characters
*/
void ghostToString(enum GhostClass ghost, char* buffer) {
    switch(ghost) {
        case BANSHEE:
            strcpy(buffer, "Banshee");
            break;
        case BULLIES:
            strcpy(buffer, "Bullies");
            break;
        case PHANTOM:
            strcpy(buffer, "Phantom");
            break;
        case POLTERGEIST:
            strcpy(buffer, "Poltergeist");
            break;
        default:
            strcpy(buffer, "Unknown");
            break;
        
    }
}


/*
Function: int isHunterPresent(GhostType* ghost, HunterArrayType* list)
Purpose: Checks if any hunter in a list is present in the same room as the specified ghost.
in: ghost - Pointer to a GhostType structure representing the ghost
in: list - Pointer to a HunterArrayType structure representing the list of hunters
return: Returns 1 if a hunter is in the same room as the ghost, 0 otherwise
*/
int isHunterPresent(GhostType* ghost, HunterArrayType* list) {
    if (ghost == NULL || list == NULL || list->size == 0) {
        return 0; // Equivalent to false
    }
    //printf("isHunterPresent: %d\n", list->size);

    for (int i = 0; i <= list->size; i++) {
    
        if (list->hunter->room == ghost->room) {
            return 1; // Equivalent to true, a hunter is in the same room
        }
    }

    return 0; // Equivalent to false, no hunter is in the same room
}

/*
Function: int isGhostPresent(GhostType* ghost, HunterType *hunter)
Purpose: Checks if a ghost is present in the same room as the specified hunter.
in: ghost - Pointer to a GhostType structure representing the ghost
in: hunter - Pointer to a HunterType structure representing the hunter
return: Returns 1 if a ghost is in the same room as the hunter, 0 otherwise
*/
int isGhostPresent(GhostType* ghost, HunterType *hunter) {
    if (ghost == NULL || hunter == NULL) {
        return 0; // Equivalent to false
    }

    if(hunter->room == ghost->room) {
        return 1; // Equivalent to true, a ghost is in the same room
    }

    return 0; // Equivalent to false, no ghost is in the same room
}

/*
Function: void moveToRandomRoomGhost(GhostType *ghost)
Purpose: Moves a ghost to a random connected room.
in/out: ghost - Pointer to a GhostType structure representing the ghost to be moved
*/
void moveToRandomRoomGhost(GhostType *ghost) {
    if (ghost == NULL || ghost->room == NULL) {
        printf("Error: Null ghost or ghost room reference\n");
        return;
    }

    // Assuming ghost->room->connectedRooms is an array of RoomType pointers
    // and there is a way to indicate the end of this array (like a NULL pointer)
    RoomNodeType *connectedRooms = ghost->room->roomlist->rhead;

    int room_num = randInt(0, ghost->room->roomlist->size); 

    //pick a random room using loop

    connectedRooms = ghost->room->roomlist->rhead;
    for (int i = 0; i < room_num; i++) {
        connectedRooms = connectedRooms->next;
    }
    ghost->room = connectedRooms->room; 

}


/*
Function: void moveToRandomRoomHunter(HunterType *hunter, HouseType *house)
Purpose: Moves a hunter to a random connected room within the specified house.
in/out: hunter - Pointer to a HunterType structure representing the hunter to be moved
in: house - Pointer to a HouseType structure representing the house environment
*/
void moveToRandomRoomHunter(HunterType *hunter, HouseType *house) {
    if (hunter == NULL || hunter->room == NULL || house == NULL) {
        printf("Error: Null hunter or hunter room reference\n");
        return;
    }

    // Assuming hunter->room->connectedRooms is an array of RoomType pointers
    // and there is a way to indicate the end of this array (like a NULL pointer)

    //remove hunter array from the old room
    removeHunter(hunter->room->hunterArray, hunter);

    RoomNodeType *connectedRooms = hunter->room->roomlist->rhead;
    int numConnectedRooms = hunter->room->roomlist->size;

    int room_num = randInt(0, numConnectedRooms); 

    //pick a random room using loop

    for (int i = 0; i < room_num; i++) {
        connectedRooms = connectedRooms->next;
    }
    hunter->room = connectedRooms->room; 

    //edit hunter array in room to add hunter
    addHunter(connectedRooms->room->hunterArray, hunter);

}



/*
Function: RoomType* getRandomRoomExcludeVan(HouseType *house)
Purpose: Returns a pointer to a randomly chosen room in the house, excluding the room named 'Van'.
in: house - Pointer to a HouseType structure representing the house environment
return: Returns a pointer to the randomly chosen RoomType structure, or NULL if no valid room is found
*/
RoomType* getRandomRoomExcludeVan(HouseType *house) {
    if (house == NULL || house->rooms == NULL || house->rooms->rhead == NULL) {
        printf("Error: Invalid house or room list\n");
        return NULL;
    }

    //GitHub, Inc. "GitHub Copilot." GitHub Copilot, 2021, https://copilot.github.com/.
    // co pilot was used to write this code
    // Count the number of rooms in the house, excluding 'Van'
    int numRooms = 0;
    RoomNodeType *current = house->rooms->rhead;
    while (current != NULL) {
        if (strcmp(current->room->name, "Van") != 0) {
            numRooms++;
        }
        current = current->next;
    }

    if (numRooms <= 1) { // If only 'Van' is present or no rooms are present
        printf("Error: No rooms available other than Van\n");
        return NULL;
    }

    // Generate a random room index
    int roomIndex = randInt(0, numRooms - 1);

    // Reset the current pointer to the head of the list
    current = house->rooms->rhead;
    while (current != NULL) {
        if (strcmp(current->room->name, "Van") != 0) {
            if (roomIndex == 0) {
                return current->room;
            }
            roomIndex--;
        }
        current = current->next;
    }

    return NULL; // In case something goes wrong
}