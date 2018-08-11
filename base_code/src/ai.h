/* COMP20003
 * 2048 AI
 * Author: <Justin Bugeja> <jbugeja> <758397>
 */

#ifndef __AI__
#define __AI__
#define OUTPUTFILE "output.txt"

#include <stdint.h>
#include <unistd.h>
#include "node.h"
#include "priority_queue.h"

/*struct needed for propagation*/
typedef struct nodeScore{
	double score;
	double total;
} nodeScore_t;

/*function prototypes*/
void initialize_ai();
move_t get_next_move(uint8_t board[SIZE][SIZE], int max_depth, propagation_t 
	propagation);
void free_memory(node_t** explored, int num);
node_t* applyAction(node_t* pointerNew, move_t dirn);
void add_explored(node_t* pointerNew, node_t** explored, int i);
void propagateBack(node_t* pointer, nodeScore_t* scoreArr, propagation_t 
	propagation);
void updateVariables(node_t* pointer, node_t* parent, move_t move, int doubles);
void assign_board(uint8_t to[SIZE][SIZE], uint8_t from[SIZE][SIZE]);
void assign_elements(node_t* p1, node_t* p2);
void printBoard(uint8_t board[SIZE][SIZE]);
node_t* initializePointer();
move_t select_bestAction(nodeScore_t* scoreArr);
int equalsBoard(uint8_t board1[SIZE][SIZE], uint8_t board2[SIZE][SIZE]);
void printOutput(float timeTotal, int max_tile, uint32_t score);
int getMaxTile(uint8_t board[SIZE][SIZE]);

#endif
