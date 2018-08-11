/* COMP20003
 * 2048 AI
 * Author: <Justin Bugeja> <jbugeja> <758397>
 */

#include <time.h>
#include <stdlib.h>
#include <inttypes.h>
#include <locale.h>
#include "ai.h"
#include "utils.h"
#include "priority_queue.h"

/*Global variables*/
struct heap h;
int arrsize = 100000;
int maxDepth = 0, generated = 0, expanded = 0;

 /*initialize the ai*/
void initialize_ai(){
	heap_init(&h);
}

/**
 * Find best action by building all possible paths up to depth max_depth
 * and back propagate using either max or avg
 */
move_t get_next_move( uint8_t board[SIZE][SIZE], int max_depth, 
	propagation_t propagation ){
		//Initally choose random move
	move_t best_action = rand() % 4;	

	 /*malloc all pointers*/
	maxDepth = max_depth;
	node_t **explored = malloc(sizeof(node_t*)*arrsize);
	if (explored == NULL) { 
		perror("malloc Node"); 
		exit (EXIT_FAILURE); 
	}
	nodeScore_t *scoreArr = malloc(4*sizeof(nodeScore_t));
	if (scoreArr == NULL) { 
		perror("malloc Node"); 
		exit (EXIT_FAILURE); 
	}
	/*initialize scores to 0*/
	scoreArr[left].score = 0;
	scoreArr[right].score = 0;
	scoreArr[up].score = 0;
	scoreArr[down].score = 0;
	
	scoreArr[left].total= 0;
	scoreArr[right].total = 0;
	scoreArr[up].total = 0;
	scoreArr[down].total = 0;
	
	
	node_t *start = initializePointer();
	assign_board(start->board, board);

	heap_push(&h, start);
	 
	int countExpl = 0;
	 /*main loop algorithm*/
	while (h.count > 0) {
		node_t* pointer = heap_delete(&h);
		expanded++;//count expanded
		add_explored(pointer, explored, countExpl++);
		if (pointer->depth <= max_depth) {
			move_t i;
			for (i = left; i<=down; i++) {
				node_t *pointerNew = applyAction(pointer, i);
				generated++;//count generated
				if  (pointerNew != NULL &&
					!equalsBoard(pointerNew->board, pointer->board)) {
					heap_push(&h, pointerNew);
					propagateBack(pointerNew, scoreArr, propagation); 
				} else {
					free(pointerNew);
				}
			}
		}
	}
	if (max_depth != 0) {
		best_action = select_bestAction(scoreArr); //SELECT BEST ACTION
	}
	 /*free nodes from memory*/
	free_memory(explored, countExpl);
	free(scoreArr);
	return best_action;
}

 /*Initializes and mallocs a pointer*/
node_t* initializePointer() {
	
	node_t* pointer = malloc(sizeof(node_t));
	if (pointer == NULL) { 
		perror("malloc Node"); 
		exit (EXIT_FAILURE); 
	}
	//Initialize to 0
	pointer->priority = (uint32_t)0;
	pointer->depth = 0;
	pointer->num_childs = 0;
	pointer->parent = NULL;
	
	return pointer;
}
 /*Assigns the to board with the same values as the from board*/
void assign_board(uint8_t to[SIZE][SIZE], uint8_t from[SIZE][SIZE]) {
	int i, j;
	for (i = 0; i<SIZE; i++) {
		for (j=0; j<SIZE; j++) {
			to[i][j] = from[i][j];
		}
	}
}
 /*Updates variables after an action is made*/
void updateVariables(node_t* pointer, node_t* parent, move_t move, 
	int doubles) {
	pointer->parent = parent;
	pointer->depth = parent->depth + 1;
	pointer->move = move;
}
 /*Checks if two boards are equal*/
int equalsBoard(uint8_t board1[SIZE][SIZE], uint8_t board2[SIZE][SIZE]) {
	int i, j;
	for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            if (board1[i][j] != board2[i][j]) {
            	return 0;
            }
        }
    }
    return 1;
}

 /*Frees all nodes from memory*/
void free_memory(node_t** explored, int num) {
	int i;
	for(i = 0; i<num; i++) {
		free(explored[i]);
	}
	free(explored);
}

 /*Adds a node pointer into the explored array*/
void add_explored(node_t* pointerNew, node_t **explored, int i) {

	if (i > arrsize) {
		node_t **temp = realloc(explored, (++arrsize)*sizeof(node_t*));
		if(!temp) {
			perror("realloc Node"); 
			exit (EXIT_FAILURE); 
		}
		explored = temp;
	}
	explored[i] = pointerNew; 
}
 /*Propagates back to the first move*/
void propagateBack(node_t* pointer, nodeScore_t* scoreArr, 
	propagation_t propagation) {

	double score = pointer->priority;
	double avrg = 0;
	
	node_t* prop = pointer;
	if (propagation == max) {
		//Iterate until first move
		for (; prop->depth > 1; prop = prop->parent);
		move_t i = prop->move;
		if (score>=scoreArr[i].score) {
			scoreArr[i].score = score;
		}
	 /*Decides whether to propagate avg or max*/
	} else if (propagation == avg) {
		//Iterate until first move
		for (; prop->depth > 1; prop = prop->parent);
		move_t i = prop->move;
		double numChild = ++prop->num_childs;
		scoreArr[i].total += score;
		//Divide by zero check
		if (numChild != 0) {
			avrg = scoreArr[i].total/numChild;
		}
		scoreArr[i].score = avrg;
	} 

}
 /*Selects the best action from the propagated scoreArr*/
move_t select_bestAction(nodeScore_t* scoreArr) {
	move_t bestMove;
	uint32_t score = (uint32_t)0;
	
	move_t i;
	for (i = left; i<=down; i++) {
		if (scoreArr[i].score >= score) {
			bestMove = i;
			score = scoreArr[i].score;
		}
	}
	return bestMove;
}
 /*Applys a move in a given direction to the board and adds a random tile*/
node_t* applyAction(node_t* pointer, move_t dirn) {
	int i,j, corner = 0;
	
	node_t* new = initializePointer();
	assign_board(new->board, pointer->board); //Assign node elements
	new->priority = pointer->priority;
	bool success = execute_move_t(new->board, &new->priority, dirn);	
	if (success) {
		addRandom(new->board);
	}
	//Updates the new pointer
	updateVariables(new, pointer, dirn, corner);
	return new;
}

 /*Returns the maximum tile on the board*/
int getMaxTile(uint8_t board[SIZE][SIZE]) {
	int tile = 0;
	int i,j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            if(board[i][j] > tile) {
            	tile = board[i][j];
            }
        }
    } 
    //Bitwise operator 2^(tile)
    tile =  1 << tile;
    return tile;
}

 /*Prints out the output to file*/
void printOutput(float timeTotal, int max_tile, uint32_t score) {
	FILE *fp = fopen(OUTPUTFILE, "w");
	if (fp == NULL) {
		printf("Error opening file!\n");
    	exit(1);
    }
    //Use commas in number
    setlocale(LC_NUMERIC, "");
    int expandedSec = expanded/timeTotal;
	fprintf(fp,"MaxDepth: %'d\n", maxDepth);
	fprintf(fp,"Generated: %'d\n", generated);
	fprintf(fp,"Expanded: %'d\n", expanded);
	fprintf(fp,"Time: %.2f seconds\n", timeTotal);
	fprintf(fp,"Expanded/Second: %'d\n", expandedSec);
	fprintf(fp,"max_tile: %d\n" , max_tile);
	fprintf(fp,"Score: %'"PRIu32"\n" , score);
	fclose(fp);
}