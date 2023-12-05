#include "defs.h"

/*
Function: void initHunter(HunterType *hunter, const char *name, EvidenceType equipment, RoomType *room)
Purpose: Initializes a HunterType structure with the provided values.
in/out: hunter - Pointer to a HunterType structure to be initialized
in: name - Pointer to a string representing the hunter's name
in: equipment - EvidenceType representing the hunter's equipment
in: room - Pointer to a RoomType structure representing the initial room of the hunter
*/
void initHunter(HunterType *hunter, const char *name, EvidenceType equipment, RoomType *room)
{
    if (hunter == NULL)
    {
        return; // Ensure the passed pointer is valid
    }

    //GitHub, Inc. "GitHub Copilot." GitHub Copilot, 2021, https://copilot.github.com/.
    // co pilot was used to write this code
    strcpy(hunter->name, name);    // Copy the name into the hunter's name field
    hunter->equipment = equipment; // Set the equipment
    hunter->fear = 0;              // Initialize fear to a default value
    hunter->boredom = 0;           // Initialize boredom to a default value
    hunter->room = room;           // Set the current room of the hunter
}

/*
Function: void initHunterArray(HunterArrayType *hunterArray, int initial_capacity)
Purpose: Initializes a HunterArrayType structure with the specified initial capacity.
in/out: hunterArray - Pointer to a HunterArrayType structure to be initialized
in: initial_capacity - Integer representing the initial capacity of the hunter array
*/
void initHunterArray(HunterArrayType *hunterArray, int initial_capacity)
{
    if (hunterArray == NULL)
    {
        return; // Ensure the passed pointer is valid
    }

    // Allocate memory for the array of HunterType
    hunterArray->hunter = malloc(sizeof(HunterType) * initial_capacity);
    if (hunterArray->hunter == NULL)
    {
        // Handle memory allocation failure
        return;
    }

    // Initialize the size and capacity
    hunterArray->size = 0;
    hunterArray->capacity = initial_capacity;

    // Initialize the semaphore
    sem_init(&hunterArray->sem, 0, 1);
}

/*
Function: void initHunterBehaviorContext(HunterBehaviorContext *context, HunterType *hunter, GhostType *ghosts, HouseType *house, EvidenceArrayType *sharedEvidence, SharedGameState *sharedState)
Purpose: Initializes a HunterBehaviorContext structure with the provided references.
in/out: context - Pointer to a HunterBehaviorContext structure to be initialized
in: hunter - Pointer to a HunterType structure representing the hunter
in: ghosts - Pointer to a GhostType structure representing the ghosts in the house
in: house - Pointer to a HouseType structure representing the house environment
in: sharedEvidence - Pointer to an EvidenceArrayType structure for shared evidence collection
in: sharedState - Pointer to a SharedGameState structure for shared game state
*/
void initHunterBehaviorContext(HunterBehaviorContext *context, HunterType *hunter, GhostType *ghosts, HouseType *house, EvidenceArrayType *sharedEvidence, SharedGameState *sharedState)
{
    if (context == NULL)
    {
        return; // Ensure the passed pointer is valid
    }

    context->hunter = hunter;
    context->ghosts = ghosts;
    context->house = house;
    context->sharedEvidence = sharedEvidence;
    context->sharedState = sharedState;
}

/*
Function: int addHunter(HunterArrayType *hunterArray, const HunterType *newHunter)
Purpose: Adds a new hunter to the HunterArrayType structure.
in/out: hunterArray - Pointer to a HunterArrayType structure representing the array of hunters - items will be added to this array
in: newHunter - Pointer to a constant HunterType structure representing the new hunter to be added
out: Returns 0 on success, -1 on invalid input or if the array is full
*/
int addHunter(HunterArrayType *hunterArray, const HunterType *newHunter)
{
    if (hunterArray == NULL || newHunter == NULL)
    {
        return -1; // Invalid input
    }

    // Check if the array has reached its capacity
    if (hunterArray->size >= hunterArray->capacity)
    {
        // Optionally, resize the array (realloc) here
        return -1; // Array is full, cannot add more hunters
    }

    // Acquire the semaphore, lock it for this thread
    sem_wait(&hunterArray->sem);
    // Copy the new hunter to the array
    hunterArray->hunter[hunterArray->size] = *newHunter;
    hunterArray->size++;
    // Release the semaphore, unlock it for other threads
    sem_post(&hunterArray->sem);

    return 0; // Success
}


/*
Function: void *hunterBehav(void *param)
Purpose: Represents the behavior of a hunter in a separate thread, updating its state in the house environment.
in/out: param - Pointer to a HunterBehaviorContext structure representing the context for hunter behavior - modified shared state
*/
void *hunterBehav(void *param)
{
    HunterBehaviorContext *context = (HunterBehaviorContext *)param;

    // Check for NULL context
    if (context == NULL || context->sharedState == NULL)
    {
        printf("Error: Null context or shared state in hunterBehav\n");
        pthread_exit(NULL);
    }

    //GitHub, Inc. "GitHub Copilot." GitHub Copilot, 2021, https://copilot.github.com/.
    // co pilot was used to write this code
    HunterType *hunter = context->hunter;
    GhostType *ghosts = context->ghosts;
    HouseType *house = context->house;
    EvidenceArrayType *sharedEvidence = context->sharedEvidence;
    SharedGameState *sharedState = context->sharedState;

    // Loop until the hunter is too fearful, bored, or enough evidence is collected
    while (hunter->fear < FEAR_MAX && hunter->boredom < BOREDOM_MAX && sharedState->gameOver == 0)
    {
        updateHunterState(hunter, ghosts, house, sharedEvidence, sharedState);

        // Check for game ending conditions
        if (house->hunterCount == 0)
        {
            sharedState->gameOver = 1; // Set game over condition
            break;                     // Exit the loop
        }

        usleep(HUNTER_WAIT); // Wait for a predefined period before updating state again
    }

    pthread_exit(NULL);
}

/*
Function: void updateHunterState(HunterType *hunter, GhostType *ghost, HouseType *house, EvidenceArrayType *sharedEvidence, SharedGameState *sharedState)
Purpose: Updates the state of a hunter based on the current conditions in the house.
in/out: hunter - Pointer to a HunterType structure representing the hunter to be updated
in: ghost - Pointer to a GhostType structure representing the ghost in the house
in: house - Pointer to a HouseType structure representing the house environment
in: sharedEvidence - Pointer to an EvidenceArrayType structure for shared evidence collection
in/out: sharedState - Pointer to a SharedGameState structure for shared game state
*/
void updateHunterState(HunterType *hunter, GhostType *ghost, HouseType *house, EvidenceArrayType *sharedEvidence, SharedGameState *sharedState)
{
    if (hunter == NULL || house == NULL || sharedEvidence == NULL)
    {
        return; // Ensure the passed pointer is valid
    }

    // Check if a ghost is in the same room as the hunter
    int isGhostInRoom = isGhostPresent(ghost, hunter);

    if (isGhostInRoom)
    {
        hunter->fear++;
        hunter->boredom = 0;
    }
    else
    {
        hunter->boredom++;
    }
    if (hunter->fear >= FEAR_MAX)
    {
        // remove hunter from the array
        l_hunterExit(hunter->name, LOG_FEAR);
        // removeHunter(house->hunterArray, hunter);
        house->hunterCount = house->hunterCount - 1;
        pthread_exit(NULL);
    }
    if (hunter->boredom >= BOREDOM_MAX)
    {
        // remove hunter from the array
        l_hunterExit(hunter->name, LOG_BORED);
        // removeHunter(house->hunterArray, hunter);
        house->hunterCount = house->hunterCount - 1;
        pthread_exit(NULL);
    }
    if (house->hunterArray->size == 0)
    {
        sharedState->gameOver = 1;
        pthread_exit(NULL);
    }

    switch (randInt(0, 3))
    {
    case 0: // Move to a random, connected room
        moveToRandomRoomHunter(hunter, house);
        l_hunterMove(hunter->name, hunter->room->name);
        break;
    case 1:
        // check if evidence is in the room of the hunter
        EvidenceType collectedEv = isEvidencePresent(hunter->room, hunter->equipment);
        if (collectedEv != EV_UNKNOWN)
        {
            int added = collectEvidence(sharedEvidence, collectedEv);
            if (added == 0)
            {
                printf("Failed to add evidence to shared array\n"); // if evidence array is full or that type of evidence is already collected
            }
            else
            {
                l_hunterCollect(hunter->name, collectedEv, hunter->room->name);
            }
        }
        break;

    case 2: // Review evidence

        if (reviewEvidence(sharedEvidence, ghost) == 1)
        {
            l_hunterReview(hunter->name, LOG_SUFFICIENT);
            // EXIT ALL THE THREADS FOR THE HUNTER
            sharedState->gameOver = 1;
            pthread_exit(NULL);
        }
        else
        {
            l_hunterReview(hunter->name, LOG_INSUFFICIENT);
        }
        break;
    }
}

/*
Function: void assignRandomEquipment(HunterArrayType *hunters, int numHunters)
Purpose: Assigns random equipment to a specified number of hunters.
in/out: hunters - Pointer to a HunterArrayType structure representing the array of hunters - assigning equipment to these hunters
in: numHunters - Integer specifying the number of hunters to assign equipment to
*/
void assignRandomEquipment(HunterArrayType *hunters, int numHunters)
{
    if (hunters == NULL || hunters->hunter == NULL || numHunters <= 0 || hunters->size < numHunters)
    {

        return; // Invalid input
    }

    if (numHunters > EV_COUNT)
    {
        printf("Error: More hunters than equipment types.\n");
        return;
    }

    // loop goes through the number of hunters and assigns them a random equipment
    for (int i = 0; i < numHunters; i++)
    {
        EvidenceType equipmentIndex;
        int isUnique = 0;
        do
        {
            equipmentIndex = randInt(0, EV_COUNT - 1);
            isUnique = 1;

            // Check if this equipment has already been assigned
            for (int j = 0; j < i; j++)
            {
                if (hunters->hunter[j].equipment == equipmentIndex)
                {
                    isUnique = 0;
                    break;
                }
            }
        } while (!isUnique);

        hunters->hunter[i].equipment = equipmentIndex;
    }
}

/*
Function: void removeHunter(HunterArrayType *hunters_list, HunterType *hunter)
Purpose: Removes a specified hunter from the list of hunters.
in/out: hunters_list - Pointer to a HunterArrayType structure representing the array of hunters
in: hunter - Pointer to a HunterType structure representing the hunter to be removed
*/
void removeHunter(HunterArrayType *hunters_list, HunterType *hunter)
{
    int i;
    // lock with semaphore
    sem_wait(&hunters_list->sem);

    for (i = 0; i < hunters_list->size; i++)
    {
        // Find the hunter to remove
        if (strcmp(hunters_list->hunter[i].name, hunter->name) == 0)
        {
            // Replace the found hunter with the last hunter in the list
            if (i != hunters_list->size - 1)
            { // Check if it's not already the last hunter
                hunters_list->hunter[i] = hunters_list->hunter[hunters_list->size - 1];
            }
            hunters_list->size--; // Decrease the size of the list
            break;                // Exit the loop
        }
    }
    // unlock the semaphore
    sem_post(&hunters_list->sem);
}

/*
Function: void freeHunter(HunterType *hunter)
Purpose: Frees the memory allocated for a HunterType structure, including its evidence array.
in/out: hunter - Pointer to a HunterType structure representing the hunter to be freed
*/
void freeHunter(HunterType *hunter)
{
    if (hunter == NULL)
    {
        return;
    }

    // Free the evidence array contents
    if (hunter->evidenceArray != NULL)
    {
        freeEvidenceArray(hunter->evidenceArray); // Free the contents of the evidence array
        free(hunter->evidenceArray);              // Free the evidence array pointer itself
        hunter->evidenceArray = NULL;             // Avoid dangling pointer
    }
}

/*
Function: void clearHunterArray(HunterArrayType *hunterArray)
Purpose: Clears the memory allocated for the array of hunters in a HunterArrayType structure.
in/out: hunterArray - Pointer to a HunterArrayType structure representing the array of hunters - to be freed
*/
void clearHunterArray(HunterArrayType *hunterArray)
{
    if (hunterArray == NULL)
    {
        return; // Ensure the passed pointer is valid
    }

    // Free the memory allocated for the array of hunter array
    free(hunterArray->hunter);
    // making it null
    hunterArray->hunter = NULL;
}