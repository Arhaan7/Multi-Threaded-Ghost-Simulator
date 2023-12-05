#include "defs.h"

/*
Function: RoomType* createRoom(const char* name)
Purpose: Creates and initializes a new RoomType structure with the specified name.
in: name - Pointer to a null-terminated string representing the name of the room
out: Returns a pointer to the newly created RoomType structure
*/
RoomType *createRoom(const char *name)
{
    RoomType *newRoom = malloc(sizeof(RoomType));
    if (newRoom == NULL)
    {
        printf("Error allocating memory for room\n");
        exit(1);
    }

    // Assuming name is a null-terminated string
    // +1 for the null terminator
    initRoom(newRoom, name);

    // Initialize other fields of RoomType here, if any

    return newRoom;
}

/*
Function: void initRoom(RoomType *room, const char *name)
Purpose: Initializes a RoomType structure with the specified name and allocates memory for associated lists.
in/out: room - Pointer to a RoomType structure to be initialized
in: name - Pointer to a null-terminated string representing the name of the room
*/
void initRoom(RoomType *room, const char *name)
{
    //GitHub, Inc. "GitHub Copilot." GitHub Copilot, 2021, https://copilot.github.com/.
    // co pilot was used to write this code
    strcpy(room->name, name);
    room->roomlist = NULL;
    room->evidencelist = malloc(sizeof(EvidenceListType));
    if (room->evidencelist != NULL)
    {
        initEvidenceList(room->evidencelist);
    }
    else
    {
        // Handle allocation error
        printf("Error: Failed to allocate memory for evidence list\n");
    }

    // Initialize hunter array with allocating memory
    room->hunterArray = malloc(sizeof(HunterArrayType));
    // call initHunterArray to initialize the hunter array
    if (room->hunterArray != NULL)
    {
        initHunterArray(room->hunterArray, NUM_HUNTERS);
    }
    else
    {
        // Handle allocation error
        printf("Error: Failed to allocate memory for hunter array\n");
    }

    room->ghost = NULL;
}

/*
Function: void initRoomList(RoomListType *roomList)
Purpose: Initializes a RoomListType structure with default values and initializes the semaphore.
in/out: roomList - Pointer to a RoomListType structure to be initialized
*/
void initRoomList(RoomListType *roomList)
{
    if (roomList == NULL)
    {
        return; // Ensure the passed pointer is valid
    }
    roomList->rhead = NULL;
    roomList->rtail = NULL;
    roomList->size = 0;

    // Initialize the semaphore
    sem_init(&roomList->sem, 0, 1);
}

/*
Function: void addRoom(RoomListType* list, RoomType* room)
Purpose: Adds a new room to the RoomListType structure.
in/out: list - Pointer to a RoomListType structure representing the list of rooms - adding a room will modify this list
in: room - Pointer to a RoomType structure representing the room to be added
*/
void addRoom(RoomListType *list, RoomType *room)
{
    if (list == NULL || room == NULL)
    {
        printf("Error: NULL parameter passed to addRoom\n");
        return;
    }

    // Create a new room node
    RoomNodeType *newNode = malloc(sizeof(RoomNodeType));
    if (newNode == NULL)
    {
        printf("Error allocating memory for room node\n");
        exit(1);
    }

    // lock the list
    sem_wait(&list->sem);

    newNode->room = room;
    newNode->next = NULL;

    // If the list is empty, set both head and tail to the new node
    if (list->rhead == NULL)
    {
        list->rhead = newNode;
        list->rtail = newNode;
    }
    else
    {
        // Otherwise, add the new node at the end and update the tail
        list->rtail->next = newNode;
        list->rtail = newNode;
    }

    // Increment the list size
    list->size++;
    // unlock the list
    sem_post(&list->sem);
}

/*
Function: void connectRooms(RoomType *room1, RoomType *room2)
Purpose: Connects two rooms by adding each to the other's room list.
in/out: room1 - Pointer to a RoomType structure representing the first room to be connected - allocated memory for its respective room list
in/out: room2 - Pointer to a RoomType structure representing the second room to be connected - allocated memory for its respective room list
*/
void connectRooms(RoomType *room1, RoomType *room2)
{
    if (room1 == NULL || room2 == NULL)
    {
        printf("Error: NULL room passed to connectRooms\n");
        return;
    }

    //GitHub, Inc. "GitHub Copilot." GitHub Copilot, 2021, https://copilot.github.com/.
    // co pilot was used to write this code
    if (room1->roomlist == NULL)
    {
        room1->roomlist = malloc(sizeof(RoomListType));
        initRoomList(room1->roomlist);
    }
    if (room2->roomlist == NULL)
    {
        room2->roomlist = malloc(sizeof(RoomListType));
        initRoomList(room2->roomlist);
    }
    // Append room2 to room1's list and vice versa
    addRoom(room1->roomlist, room2);
    addRoom(room2->roomlist, room1);
}

/*
Function: void freeRoomList(RoomListType *roomList)
Purpose: Frees the memory allocated for the nodes in a RoomListType structure.
in/out: roomList - Pointer to a RoomListType structure representing the list of rooms - to be freed
*/
void freeRoomList(RoomListType *roomList)
{
    if (roomList == NULL)
    {
        return;
    }


    RoomNodeType *current = roomList->rhead;
    while (current != NULL)
    {
        RoomNodeType *temp = current;
        current = current->next;
        temp->room = NULL; // Set the room pointer to NULL
        free(temp);        // Free the node only, not the room
    }
}

/*
Function: void freeRoom(RoomType *room)
Purpose: Frees the memory allocated for a RoomType structure, including its associated lists and arrays.
in/out: room - Pointer to a RoomType structure representing the room to be freed
*/
void freeRoom(RoomType *room)
{
    if (room == NULL)
    {
        return;
    }

    // Free the evidence list
    if (room->evidencelist != NULL)
    {
        freeEvidenceList(room->evidencelist); // This function should free the entire list and its contents
    }

    // Free the hunter array
    if (room->hunterArray != NULL)
    {
        clearHunterArray(room->hunterArray); // This function should free the contents of the hunter array
        free(room->hunterArray);             // Free the hunter array structure itself
    }

    // free room list
    if (room->roomlist != NULL)
    {
        freeRoomList(room->roomlist);
        free(room->roomlist);
    }

    // free the room
    free(room);
}