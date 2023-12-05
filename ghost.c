#include "defs.h"

/*
Function: void initGhost(GhostType *ghost, enum GhostClass type, RoomType *room)
Purpose: Initializes a GhostType structure with the provided values.
in/out: ghost - Pointer to the GhostType structure to be initialized
in: type - The type of the ghost (enum GhostClass)
in: room - Pointer to the RoomType structure representing the room where the ghost is located
return: none
*/
void initGhost(GhostType *ghost, enum GhostClass type, RoomType *room)
{

    if (ghost == NULL)
    {
        return; // Ensure the passed pointer is valid
    }

    //GitHub, Inc. "GitHub Copilot." GitHub Copilot, 2021, https://copilot.github.com/.
    // co pilot was used to write this code
    ghost->ghostType = type;
    ghost->room = room;     // You can pass NULL if the ghost isn't in a room initially
    ghost->boredomTime = 0; // Initialize with some default value or a parameter

    // Call l_ghostInit with the ghost type and room name
    l_ghostInit(ghost->ghostType, room != NULL ? room->name : "No Room");
}

/*
Function: void updateGhostState(GhostType *ghost, HunterArrayType *hunters, SharedGameState *sharedState)
Purpose: Updates the state of a ghost based on the presence of hunters and random actions.
in/out: ghost - Pointer to the GhostType structure to be updated
in: hunters - Pointer to the HunterArrayType structure representing the array of hunters
in/out: sharedState - Pointer to the SharedGameState structure representing the shared game state
return: none
*/
void updateGhostState(GhostType *ghost, HunterArrayType *hunters, SharedGameState *sharedState)
{
    // Check if a hunter is in the same room as the ghost
    int isHunterInRoom = isHunterPresent(ghost, hunters);

    if (isHunterInRoom)
    {
        ghost->boredomTime = 0;
    }
    else
    {
        ghost->boredomTime++;
        if (ghost->boredomTime >= BOREDOM_MAX)
        {

            l_ghostExit(LOG_BORED);
            sharedState->gameOver = 1; // Set game over condition
            // terminate all threads
            pthread_exit(NULL);
        }
    }

    switch (randInt(0, 3))
    {
    case 0:
        // Do nothing
        break;
    case 1:
        if (ghost == NULL)
        {
            printf("ghost is null\n");
        }
        else if (ghost->room == NULL)
        {
            printf("room is null\n");
        }
        EvidenceType ev = addEvidence(ghost);
        l_ghostEvidence(ev, ghost->room->name);
        break;
    case 2:
        // printf("moving\n");
        if (!isHunterInRoom)
        {
            moveToRandomRoomGhost(ghost);
            l_ghostMove(ghost->room->name);
        }
        break;
    }
}

/*
Function: void initGhostBehaviorContext(GhostBehaviorContext *context, GhostType *ghost, HouseType *house, HunterArrayType *hunters, SharedGameState *sharedState)
Purpose: Initializes the GhostBehaviorContext structure with the provided ghost, house, hunters, and shared state.
in/out: context - Pointer to the GhostBehaviorContext structure to be initialized
in: ghost - Pointer to the GhostType structure representing the ghost
in: house - Pointer to the HouseType structure representing the house
in: hunters - Pointer to the HunterArrayType structure representing the array of hunters
in: sharedState - Pointer to the SharedGameState structure representing the shared game state
return: none
*/
void initGhostBehaviorContext(GhostBehaviorContext *context, GhostType *ghost, HouseType *house, HunterArrayType *hunters, SharedGameState *sharedState)
{
    //GitHub, Inc. "GitHub Copilot." GitHub Copilot, 2021, https://copilot.github.com/.
    // co pilot was used to write this code
    context->ghost = ghost;
    context->hunters = hunters;
    context->house = house;
    context->sharedState = sharedState;
}

/*
Function: void *ghostBehav(void *param)
Purpose: Represents the behavior of a ghost in a separate thread.
in/out: param - Pointer to a GhostBehaviorContext structure containing information about the ghost's behavior context - modified shared state
return: none
*/
void *ghostBehav(void *param)
{
    GhostBehaviorContext *context = (GhostBehaviorContext *)param;
    if (context == NULL)
    {
        printf("Error: Null context or shared state in ghostBehav\n");
        pthread_exit(NULL);
    }

    
    // While loop should run until the ghost is bored or the game is over
    while (context->ghost->boredomTime < BOREDOM_MAX && context->sharedState->gameOver == 0 && context->house->hunterCount > 0)
    {
        updateGhostState(context->ghost, context->hunters, context->sharedState);

        if (context->ghost->boredomTime >= BOREDOM_MAX || context->house->hunterCount == 0)
        {
            context->sharedState->gameOver = 1; // Set game over condition
            break;                              // Exit the loop
        }

        usleep(GHOST_WAIT);
    }

    pthread_exit(NULL);
}

/*
Function: void freeGhost(GhostType *ghost)
Purpose: Frees the memory allocated for a GhostType structure.
in/out: ghost - Pointer to the GhostType structure to be freed 
return: none
*/
void freeGhost(GhostType *ghost)
{
    if (ghost != NULL)
    {

        // If the GhostType itself is dynamically allocated:
        free(ghost);
    }
}