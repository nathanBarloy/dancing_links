/* DANCING LINKS

This code allows to solve the exact cover problem efficiently,
by using the dancing links algorithm X (also known as DLX) as
described by D. Knuth

The main function is "solve_exact_cover" (see description).

*/

#ifndef DANCING_LINKS_H
#define DANCING_LINKS_H

#include <stdbool.h>


// A node structure used to describe the problem
struct _node {
    struct _node *left;
    struct _node *right;
    struct _node *up;
    struct _node *down;
    struct _node *head;
    int row_id; // -1 if header, -2 if root
    int node_count;
};
typedef struct _node node_t;


// A stack structure used to describe a solution
struct _stack {
    int capacity;
    int size;
    node_t **tab;
};
typedef struct _stack stack_t;


/*** Functions to manipulate stacks ***/

// Create a stack
stack_t *init_stack(int init_size);

// Free a stack
void free_stack(stack_t *stack);

// Add a node on top of the stack
void append(stack_t *stack, node_t *elem);

// Get the last element added 
node_t *pop(stack_t *stack);



/*** Functions to manipulate nodes ***/

// Creates a node. Its neighbours are not initiated
node_t *init_node(int row_id);

// Free a probleme and its corresponding solution
void free_pb(node_t *pb, stack_t *sol);




/*** Functions to solve the exact cover problem ***/

// Solve the exact cover problem.
// The arguments are:
//     - height: the height of the matrix describing the problem
//     - width: the width of the matrix describing the problem
//     - mat: the matrix describing the problem
//     - n: a return argument. The length of the returned list will be put here.
//
// The return is:
//     An int list containing the line numbers of a solution. The length of this list
//     is put in the return argument n.
int *solve_exact_cover(int height, int width, bool **mat, int *n);

// Transform the matrix of the problem in a node structure of the problem
node_t *from_bool_matrix(int height, int width, bool **mat);

// Remove the columns and nodes covered by the argument
void cover(node_t *node);

// Restore the columns and nodes covered by the argument
void uncover(node_t *node);

// Find the column with the less elements
node_t *min_column(node_t *pb);

// Recursive function that solves the problem
bool solve(node_t *pb, stack_t *sol);

// Print a solution
void print_solution(stack_t *sol);

#endif