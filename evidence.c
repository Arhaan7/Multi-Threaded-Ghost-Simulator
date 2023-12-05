#include "defs.h"

/*
Function: void initEvidenceArray(EvidenceArrayType *evidenceArray, int capacity)
Purpose: Initializes an EvidenceArrayType structure with an array of EvidenceType and sets its capacity.
in/out: evidenceArray - Pointer to an EvidenceArrayType structure to be initialized
in: capacity - Capacity of the evidence array
return: nothing is being returned
*/
void initEvidenceArray(EvidenceArrayType *evidenceArray, int capacity)
{
    evidenceArray->evidence = malloc(capacity * sizeof(EvidenceType));
    evidenceArray->size = 0;
    evidenceArray->capacity = capacity;
    sem_init(&evidenceArray->sem, 0, 1); // Pass address of sem field
}

/*
Function: void initEvidenceList(EvidenceListType *evidenceList)
Purpose: Initializes an EvidenceListType structure by setting its head, tail, and semaphore.
in/out: evidenceList - Pointer to an EvidenceListType structure to be initialized
return: nothing is being returned
*/
void initEvidenceList(EvidenceListType *evidenceList)
{
    if (evidenceList == NULL)
    {
        printf("Error: evidenceList is NULL\n");
        return;
    }
    //GitHub, Inc. "GitHub Copilot." GitHub Copilot, 2021, https://copilot.github.com/.
    // co pilot was used to write this code
    evidenceList->ehead = NULL;
    evidenceList->etail = NULL;
    sem_init(&evidenceList->sem, 0, 1);
}

/*
Function: EvidenceType addEvidence(GhostType *ghost)
Purpose: Generates and adds evidence to the evidence list of a room based on the ghost's type.
in: ghost - Pointer to the GhostType structure representing the ghost
out: Returns the type of evidence added to the room
*/
EvidenceType addEvidence(GhostType *ghost)
{
    if (ghost == NULL || ghost->room == NULL || ghost->room->evidencelist == NULL)
    {
        printf("Error: Invalid ghost or room\n");
        return EV_UNKNOWN;
    }
    // test
    // printf("test adding evidence\n");
    // Create a new evidence, starting it off as unknown
    EvidenceType evidenceToAdd = EV_UNKNOWN;
    // if (evidenceToAdd == NULL) {
    //     // Handle memory allocation failure
    //     return NULL;
    // }
    // Determine evidence type based on ghost class
    switch (ghost->ghostType)
    {
    case POLTERGEIST:
        // Randomly choose one of the three evidence types for POLTERGEIST
        switch (randInt(0, 3))
        {
        case 0:
            evidenceToAdd = EMF;
            break;
        case 1:
            evidenceToAdd = TEMPERATURE;
            break;
        case 2:
            evidenceToAdd = FINGERPRINTS;
            break;
        }
        break;
    case BANSHEE:
        switch (randInt(0, 3))
        {
        case 0:
            evidenceToAdd = EMF;
            break;
        case 1:
            evidenceToAdd = TEMPERATURE;
            break;
        case 2:
            evidenceToAdd = SOUND;
            break;
        }
        break;
    case BULLIES:
        switch (randInt(0, 3))
        {
        case 0:
            evidenceToAdd = EMF;
            break;
        case 1:
            evidenceToAdd = FINGERPRINTS;
            break;
        case 2:
            evidenceToAdd = SOUND;
            break;
        }
        break;
    case PHANTOM:
        switch (randInt(0, 3))
        {
        case 0:
            evidenceToAdd = TEMPERATURE;
            break;
        case 1:
            evidenceToAdd = FINGERPRINTS;
            break;
        case 2:
            evidenceToAdd = SOUND;
            break;
        }
        break;
    default:
        // Handle unknown ghost types, if necessary
        return evidenceToAdd;
    }

    // Create a new evidence node
    EvidenceNodeType *newNode = malloc(sizeof(EvidenceNodeType));
    if (newNode == NULL)
    {
        printf("Error allocating memory for evidence node\n");
        exit(1);
    }
    // lock the evidence list
    sem_wait(&ghost->room->evidencelist->sem);
    newNode->evidence = evidenceToAdd;
    newNode->next = NULL;

    //GitHub, Inc. "GitHub Copilot." GitHub Copilot, 2021, https://copilot.github.com/.
    // co pilot was used to write this code
    if (ghost->room->evidencelist->ehead == NULL)
    {
        ghost->room->evidencelist->ehead = newNode;
        ghost->room->evidencelist->etail = newNode;
    }
    else
    {
        ghost->room->evidencelist->etail->next = newNode;
        ghost->room->evidencelist->etail = newNode;
    }
    // unlock the evidence list
    sem_post(&ghost->room->evidencelist->sem);
    return evidenceToAdd;
}

/*
Function: int collectEvidence(EvidenceArrayType *evidenceArray, EvidenceType evidence)
Purpose: Collects evidence and adds it to the shared evidence array.
in/out: evidenceArray - Pointer to an EvidenceArrayType structure representing the shared evidence array
in: evidence - The type of evidence to be collected
out: Returns 1 if evidence is successfully collected, 0 otherwise
*/
int collectEvidence(EvidenceArrayType *evidenceArray, EvidenceType evidence)
{
    if (evidenceArray == NULL)
    {
        printf("Error: NULL evidence array\n");
        return 0;
    }

    if (evidence < EMF || evidence >= EV_UNKNOWN)
    {
        return 0;
    }

    // Check if the array has reached its capacity
    if (evidenceArray->size >= MAX_EVIDENCE)
    {
        printf("Error: Evidence array is full\n");
        return 0;
    }

    sem_wait(&evidenceArray->sem); // Ensure semaphore is properly initialized

    // Check if the evidence is already in the shared array
    for (int i = 0; i < evidenceArray->size; i++)
    {
        if (evidenceArray->evidence[i] == evidence)
        {
            sem_post(&evidenceArray->sem);
            return 0; // Evidence already collected
        }
    }

    // Add the evidence to the array
    evidenceArray->evidence[evidenceArray->size] = evidence;
    evidenceArray->size++;

    sem_post(&evidenceArray->sem);

    return 1; // Evidence collected
}

/*
Function: EvidenceType isEvidencePresent(RoomType *room, EvidenceType hunterEquipment)
Purpose: Checks if a specific type of evidence is present in the evidence list of a room.
in: room - Pointer to the RoomType structure representing the room
in: hunterEquipment - The type of evidence to check for
out: Returns the type of evidence if present, otherwise returns EV_UNKNOWN
*/
EvidenceType isEvidencePresent(RoomType *room, EvidenceType hunterEquipment)
{
    if (room == NULL || room->evidencelist == NULL)
    {
        return 0; // No evidence present
    }

    EvidenceNodeType *current = room->evidencelist->ehead;
    while (current != NULL)
    {
        if (current->evidence == hunterEquipment)
        {
            return current->evidence; // Evidence matching the hunter's equipment is present
        }
        current = current->next;
    }

    return EV_UNKNOWN; // No matching evidence found
}

/*
Function: int reviewEvidence(EvidenceArrayType *evidenceArray, GhostType *ghost)
Purpose: Reviews the collected evidence and attempts to identify the ghost type.
in: evidenceArray - Pointer to an EvidenceArrayType structure representing the collected evidence array
in: ghost - Pointer to a GhostType structure to be identified
return: Returns 1 if the ghost type is correctly identified, 0 otherwise
*/
int reviewEvidence(EvidenceArrayType *evidenceArray, GhostType *ghost)
{
    if (evidenceArray == NULL || evidenceArray->size != 3)
    {
        return 0;
    }

    // Acquire the semaphore to ensure thread safety
    sem_wait(&evidenceArray->sem);

    // Assuming evidenceArray->evidence has exactly 3 elements
    EvidenceType ev1 = evidenceArray->evidence[0];
    EvidenceType ev2 = evidenceArray->evidence[1];
    EvidenceType ev3 = evidenceArray->evidence[2];

    GhostClass identifiedGhostType = GH_UNKNOWN; // Assuming UNKNOWN_GHOST is a valid GhostType

    //GitHub, Inc. "GitHub Copilot." GitHub Copilot, 2021, https://copilot.github.com/.
    // co pilot was used to write this code
    // Match the collected evidence to the known evidence for each ghost type
    if ((ev1 == EMF || ev2 == EMF || ev3 == EMF) &&
        (ev1 == TEMPERATURE || ev2 == TEMPERATURE || ev3 == TEMPERATURE) &&
        (ev1 == FINGERPRINTS || ev2 == FINGERPRINTS || ev3 == FINGERPRINTS))
    {
        identifiedGhostType = POLTERGEIST;
    }
    else if ((ev1 == EMF || ev2 == EMF || ev3 == EMF) &&
             (ev1 == TEMPERATURE || ev2 == TEMPERATURE || ev3 == TEMPERATURE) &&
             (ev1 == SOUND || ev2 == SOUND || ev3 == SOUND))
    {
        identifiedGhostType = BANSHEE;
    }
    else if ((ev1 == EMF || ev2 == EMF || ev3 == EMF) &&
             (ev1 == FINGERPRINTS || ev2 == FINGERPRINTS || ev3 == FINGERPRINTS) &&
             (ev1 == SOUND || ev2 == SOUND || ev3 == SOUND))
    {
        identifiedGhostType = BULLIES;
    }
    else if ((ev1 == TEMPERATURE || ev2 == TEMPERATURE || ev3 == TEMPERATURE) &&
             (ev1 == FINGERPRINTS || ev2 == FINGERPRINTS || ev3 == FINGERPRINTS) &&
             (ev1 == SOUND || ev2 == SOUND || ev3 == SOUND))
    {
        identifiedGhostType = PHANTOM;
    }

    // Release the semaphore
    sem_post(&evidenceArray->sem);

    // Check if the identified ghost type matches the actual ghost type
    char ghostName[MAX_STR];                       // Make sure MAX_STR is defined and large enough
    ghostToString(identifiedGhostType, ghostName); // Convert identified ghost type to string

    if (identifiedGhostType == ghost->ghostType)
    {
        printf("Correctly identified the ghost type as %s!\n", ghostName);
        return 1;
    }
    else
    {
        printf("Incorrect ghost type. Further investigation needed.\n");
        return 0;
    }
    return 0;
}

/*
Function: void freeEvidenceArray(EvidenceArrayType *evidenceArray)
Purpose: Frees the memory associated with the evidence array.
in/out: evidenceArray - Pointer to an EvidenceArrayType structure representing the evidence array and freeing its memory
return: none
*/
void freeEvidenceArray(EvidenceArrayType *evidenceArray)
{
    if (evidenceArray != NULL)
    {
        // Free the array of evidence
        free(evidenceArray->evidence);

        // Reset the pointer to avoid dangling references
        evidenceArray->evidence = NULL;
        evidenceArray->size = 0;
        evidenceArray->capacity = 0;
    }
}

/*
Function: void freeEvidenceList(EvidenceListType *evidenceList)
Purpose: Frees the memory associated with the evidence list.
in/out: evidenceList - Pointer to an EvidenceListType structure representing the evidence list and freeing its memory
return: none
*/
void freeEvidenceList(EvidenceListType *evidenceList)
{
    if (evidenceList == NULL)
    {
        return;
    }

    EvidenceNodeType *current = evidenceList->ehead;
    while (current != NULL)
    {
        EvidenceNodeType *temp = current;
        current = current->next;
        free(temp); // Free each node
    }

    free(evidenceList); // Free the list
}