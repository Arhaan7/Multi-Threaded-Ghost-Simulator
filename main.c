#include "defs.h"

// main function
int main() {
    // Initialize the random number generator
    // Populate the house with the provided rooms
    HouseType house;
    initHouse(&house);
    populateRooms(&house);
    // Place the ghost in a random room, that is not the van
    //allocate memory for the ghost
    GhostType *ghost = malloc(sizeof(GhostType));
    RoomType* randomRoom = getRandomRoomExcludeVan(&house);  // You need to implement this function
    initGhost(ghost, randomGhost(), randomRoom);

    // Ask the user to input 4 names for our hunters
    char hunterNames[NUM_HUNTERS][MAX_STR];  // Assuming NUM_HUNTERS is 4
    for (int i = 0; i < NUM_HUNTERS; i++) {
        printf("Enter name for hunter %d: ", i + 1);
        fgets(hunterNames[i], MAX_STR, stdin);
        hunterNames[i][strcspn(hunterNames[i], "\n")] = 0;  // Remove newline character
    }

    //creating initial room hunter should be in 
    RoomType* vanRoom = house.rooms->rhead->room;  // Assuming the Van room is the head of the room list

    // Create a hunter for each name and add them to the house and initialize 
    for (int i = 0; i < NUM_HUNTERS; i++) {
        HunterType hunter;
        initHunter(&hunter, hunterNames[i], EV_UNKNOWN, vanRoom);
        addHunter(house.hunterArray, &hunter);
        //add hunters to the vanRoom hunter array list
        addHunter(vanRoom->hunterArray, &hunter);

    }

    // Assign random equipment to each hunter
    assignRandomEquipment(house.hunterArray, house.hunterArray->size);  // You need to implement this function
    
    for (int i = 0; i < NUM_HUNTERS; i++) {
        l_hunterInit(house.hunterArray->hunter[i].name, house.hunterArray->hunter[i].equipment);
    }

    // Initialize the shared game state
    SharedGameState gameState;
    gameState.gameOver = 0; // Game starts with not being over

    // Create threads
    pthread_t hunterThreads[NUM_HUNTERS];
    pthread_t ghostThread;


    // Initialize ghostContext fields...
    GhostBehaviorContext *ghostContext = malloc(sizeof(GhostBehaviorContext));
    if (ghostContext == NULL) {
        printf("Failed to allocate memory for ghost context\n");
        // Handle allocation error
        return 1;
    }
    
    // Initialize ghostContext fields
    initGhostBehaviorContext(ghostContext, ghost, &house, house.hunterArray, &gameState);
    pthread_create(&ghostThread, NULL, ghostBehav, (void *)ghostContext);
    
    // Create one thread for each hunter
    HunterBehaviorContext *hunterContexts[NUM_HUNTERS];  // Array to store contexts

    for (int i = 0; i < NUM_HUNTERS; i++) {
        hunterContexts[i] = malloc(sizeof(HunterBehaviorContext));
        if (hunterContexts[i] == NULL) {
            printf("Failed to allocate memory for hunter context\n");
            // Handle allocation error
            return 1;
        }
        // Initialize hunterContext fields
        initHunterBehaviorContext(hunterContexts[i], &house.hunterArray->hunter[i], ghost, &house, house.evidenceArray, &gameState);
        
        // Create thread
        pthread_create(&hunterThreads[i], NULL, hunterBehav, (void *)hunterContexts[i]);
    }

    // Join threads and free their contexts
    pthread_join(ghostThread, NULL);

    for (int i = 0; i < NUM_HUNTERS; i++) {
        // joining the thread
        pthread_join(hunterThreads[i], NULL);
    }

    //temp variables to count the fear and boredom
    int fear_count = 0;
    int boredom_count_hunter = 0;
    printf("\nAll done! Let's tally the results...\n");
    printf("=================================\n");

    //analyze each hunter fear 
    if(house.hunterArray->size == 0){
        printf("There are no hunters left in the house\n");
        fear_count = NUM_HUNTERS;
    }
    else{
        for(int i = 0; i < house.hunterArray->size; i++){
            if(house.hunterArray->hunter[i].fear >= FEAR_MAX){
                printf("%s has been scared out of the house\n", house.hunterArray->hunter[i].name);
                fear_count++;
            }
        }

    }

    //analyze each hunter boredom
    for(int i = 0; i < house.hunterArray->size; i++){
        if(house.hunterArray->hunter[i].boredom >= BOREDOM_MAX){
            printf("%s has gotten bored and left the house\n", house.hunterArray->hunter[i].name);
            boredom_count_hunter++;
        }
    }

    //print the evidence that has been collected
    char ev[3][MAX_STR];
    printf("The evidence collected is: \n");
    for(int i = 0; i < house.evidenceArray->size; i++){
        evidenceToString(house.evidenceArray->evidence[i], ev[i]);
        printf("%s\n", ev[i]);
    }

    printf("\n"); 
    // Check if the ghost has won
    if (fear_count  == NUM_HUNTERS || boredom_count_hunter == NUM_HUNTERS || house.hunterCount == 0) {
        printf("The ghost has won the game!\n");
    } 
    // Check if the hunters have won
    else if(house.evidenceArray->size == 3) 
    {
        printf("The hunters have won the game!\n");
        reviewEvidence(house.evidenceArray, ghost);

    }
    else{
        printf("The ghost's boredom level is %d\n", ghost->boredomTime);
        printf("The ghost got bored and left\n");
    }

    // Free memory
    freeGhost(ghost);
    freeHouse(&house);
    free(ghostContext);
    for (int i = 0; i < NUM_HUNTERS; i++) {
        free(hunterContexts[i]);
    }
    
    return 0;
}