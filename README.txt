#Names: Arhaan Wazid, Joshua John
#Student ID: 101256222, 101259727

#Description: 
This project involves creating a multi-threaded ghost hunting simulation.
The simulation features a map of connected rooms, ghost hunters, and a ghost.
Each entity (ghosts and hunters) operates in its own thread, simulating real-time interactions in a haunted house environment.
The project focuses on several programming concepts, including dynamic and static memory allocation, linked lists, and the use of semaphores for thread synchronization.
The main objective is to create a realistic simulation where hunters gather evidence to identify the ghost, while managing their fear and boredom levels.

#List of files: 
defs.h
evidence.c
ghost.c
house.c
hunter.c
logger.c
main.c
Makefile
README.txt
room.c
utils.c

# Compiling the Program and Execution (Compiling and Running)
1. Go to the folder containing the source code in a terminal.
2. Use the command 'make' to create the 'fp' executable which can be executed by the computer.
3. './fp' will allow you to just to run the program without knowing where the memory leaks are (can add additional command lines to see memory leaks).


#Instructions for how to use the program after it is running 
1. Program will prompt you to type in a name for each of the 4 hunters. 
2. The program will then run and display a series of ghost and hunter movements.
3. After the simulation finishes, a final result will be displayed based on the simulation. 

#Rules: 
In the finalization of results for the game, once all threads have completed their tasks, the program follows a set of rules to present the outcomes. Firstly, it displays the results on the screen, listing hunters whose fear levels are equal to or exceed MAX_FEAR and those with boredom levels equal to or surpassing MAX_BOREDOM. Subsequently, if all hunters exhibit excessively high levels of fear or boredom, the program announces that the ghost has emerged victorious. Additionally, it prints all the evidence collected by the hunters. If three distinct pieces of evidence have been gathered, the program identifies the ghost corresponding to those pieces. The ghost's type is then revealed, along with an indication of whether the collected evidence accurately identified the ghost. 
