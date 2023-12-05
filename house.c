#include "defs.h"

/*
Function: void populateRooms(HouseType* house)
Purpose: Creates and connects rooms, forming the structure of the house, and adds them to the house's room list.
in/out: house - Pointer to a HouseType structure to be populated with rooms
return: none
*/
void populateRooms(HouseType* house) {
    // First, create each room

    // createRoom assumes that we dynamically allocate a room, initializes the values, and returns a RoomType*
    // create functions are pretty typical, but it means errors are harder to return aside from NULL
    struct Room* van                = createRoom("Van");
    struct Room* hallway            = createRoom("Hallway");
    struct Room* master_bedroom     = createRoom("Master Bedroom");
    struct Room* boys_bedroom       = createRoom("Boy's Bedroom");
    struct Room* bathroom           = createRoom("Bathroom");
    struct Room* basement           = createRoom("Basement");
    struct Room* basement_hallway   = createRoom("Basement Hallway");
    struct Room* right_storage_room = createRoom("Right Storage Room");
    struct Room* left_storage_room  = createRoom("Left Storage Room");
    struct Room* kitchen            = createRoom("Kitchen");
    struct Room* living_room        = createRoom("Living Room");
    struct Room* garage             = createRoom("Garage");
    struct Room* utility_room       = createRoom("Utility Room");

    // This adds each room to each other's room lists
    // All rooms are two-way connections
    // THIS IS INDIVIDUAL ROOM LISTS THAT CONTAINS ALL THE CONNECTIONS OF THE ROOMS IN THE HOUSE
    connectRooms(van, hallway);
    connectRooms(hallway, master_bedroom);
    connectRooms(hallway, boys_bedroom);
    connectRooms(hallway, bathroom);
    connectRooms(hallway, kitchen);
    connectRooms(hallway, basement);
    connectRooms(basement, basement_hallway);
    connectRooms(basement_hallway, right_storage_room);
    connectRooms(basement_hallway, left_storage_room);
    connectRooms(kitchen, living_room);
    connectRooms(kitchen, garage);
    connectRooms(garage, utility_room);

    // Add each room to the house's room list
    // THIS IS THE HOUSEROOM LIST OF ALL THE ROOMS IN THE HOUSE
    addRoom(house->rooms, van);
    addRoom(house->rooms, hallway);
    addRoom(house->rooms, master_bedroom);
    addRoom(house->rooms, boys_bedroom);
    addRoom(house->rooms, bathroom);
    addRoom(house->rooms, basement);
    addRoom(house->rooms, basement_hallway);
    addRoom(house->rooms, right_storage_room);
    addRoom(house->rooms, left_storage_room);
    addRoom(house->rooms, kitchen);
    addRoom(house->rooms, living_room);
    addRoom(house->rooms, garage);
    addRoom(house->rooms, utility_room);
}

/*
Function: void initHouse(HouseType *house)
Purpose: Initializes a HouseType structure.
in/out: house - Pointer to the HouseType structure to be initialized
return: none
*/
void initHouse(HouseType *house) {
    if (house == NULL) {
        printf("Error: House pointer is NULL\n");
        exit(1);
    }

    // Initialize the room list
    house->rooms = malloc(sizeof(RoomListType));
    if (house->rooms == NULL) {
        printf("Error allocating memory for room list\n");
        exit(1);
    }
    initRoomList(house->rooms);

    // Initialize the hunter array
    house->hunterArray = malloc(sizeof(HunterArrayType));
    if (house->hunterArray == NULL) {
        printf("Error allocating memory for hunter array\n");
        exit(1);
    }
    initHunterArray(house->hunterArray, NUM_HUNTERS); // Set an appropriate initial capacity

    // Initialize the evidence array
    house->evidenceArray = malloc(sizeof(EvidenceArrayType));
    if (house->evidenceArray == NULL) {
        printf("Error allocating memory for evidence array\n");
        exit(1);
    }
    
    initEvidenceArray(house->evidenceArray, MAX_EVIDENCE); // Set an appropriate initial capacity

    house->hunterCount = NUM_HUNTERS;
}

/*
Function: void freeHouse(HouseType *house)
Purpose: Frees the memory allocated for a HouseType structure.
in/out: house - Pointer to the HouseType structure to be freed
return: none
*/
void freeHouse(HouseType *house) {
    if (house == NULL) {
        return;
    }

    //GitHub, Inc. "GitHub Copilot." GitHub Copilot, 2021, https://copilot.github.com/.
    // co pilot was used to write this code
    RoomNodeType *current = house->rooms->rhead;
    if(current == NULL) {
        return;
    }
    while (current != NULL) {
        RoomNodeType *temp = current;
        current = current->next;
        freeRoom(temp->room); // Free the room
        free(temp); // Free the node
    }
    free(house->rooms); 

    // Free the hunter array
    clearHunterArray(house->hunterArray); // Assuming this function exists and frees the array correctly
    free(house->hunterArray);
    
    // Free the evidence array
    if (house->evidenceArray != NULL) {
        freeEvidenceArray(house->evidenceArray); // Assuming this function exists and frees the array correctly
        free(house->evidenceArray);
    }
}