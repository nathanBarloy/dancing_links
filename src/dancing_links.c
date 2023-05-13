#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "dancing_links.h"



/*** Functions to manipulate stacks ***/

// Create a stack
stack_t *init_stack(int init_size) {
    stack_t *stack = malloc(sizeof(*stack));
    assert(stack != NULL);
    stack->capacity = init_size;
    stack->size = 0;
    stack->tab = malloc(init_size * sizeof(*(stack->tab)));
    assert(stack->tab != NULL);
    return stack;
}

// Free a stack
void free_stack(stack_t *stack) {
    free(stack->tab);
    free(stack);
}

// Add a node on top of the stack
void append(stack_t *stack, node_t *elem) {
    // resize if needed
    if (stack->size == stack->capacity) {
        stack->capacity *= 2;
        stack->tab = realloc(stack->tab, stack->capacity);
        assert(stack->tab != NULL);
    }

    // add element
    stack->tab[stack->size++] = elem;
}

// Get the last element added 
node_t *pop(stack_t *stack) {
    return stack->tab[--stack->size];
}




/*** Functions to manipulate nodes ***/

// Creates a node. Its neighbours are not initiated
node_t *init_node(int row_id) {
    node_t *node = malloc(sizeof(*node));
    node->row_id = row_id;
    node->node_count = 0;
    return node;
}

// Free a probleme and its corresponding solution
void free_pb(node_t *pb, stack_t *sol) {
    // Put sol back in pb
    node_t *node, *head;
    pb->right = pb;
    pb->left = pb;

    while (sol->size > 0) {
        node = pop(sol);
        for (node_t *left_node = node->left; left_node != node; left_node = left_node->left) {
            uncover(left_node);
        }
        uncover(node->head);
    }

    // free the entire pb
    while (pb->right != pb) {
        head = pb->right;
        while (head->down != head) {
            node = head->down;
            head->down = node->down;
            free(node);
        }
        pb->right = head->right;
        free(head);
    }
    free(pb);
    free_stack(sol);
}





/*** Functions to solve the exact cover problem ***/

// Create the headers of the problem
void create_headers(node_t *root, int size) {
    node_t *last = root, *new_node;
    for (int i = 0; i < size; i++) {
        // create and insert new node
        new_node = init_node(-1);
        new_node->head = new_node;
        new_node->up = new_node;
        new_node->down = new_node;
        new_node->left = last;
        new_node->right = root;
        last->right = new_node;
        root->left = new_node;

        // update last
        last = new_node;
    }
}


// Transform the matrix of the problem in a node structure of the problem
node_t *from_bool_matrix(int height, int width, bool **mat) {
    // init root
    node_t *root = init_node(-2);
    root->right = root;
    root->left = root;
    root->up = NULL;
    root->down = NULL;
    root->head = NULL;

    // init headers
    create_headers(root, width);

    // fill structure
    node_t *new_node, *col_header, *prev_node;
    for (int i = 0; i < height; i++) {
        prev_node = NULL;
        col_header = root;
        for (int j = 0; j < width; j++) {
            col_header = col_header->right;
            if (mat[i][j] == 1) {
                new_node = init_node(i);
                new_node->head = col_header;
                col_header->node_count++;
                
                // Vertical link
                new_node->up = col_header->up;
                new_node->down = col_header;
                col_header->up->down = new_node;
                col_header->up = new_node;

                // Horizontal links
                if (prev_node == NULL) {
                    prev_node = new_node;
                    new_node->right = new_node;
                    new_node->left = new_node;
                } else {
                    new_node->left = prev_node;
                    new_node->right = prev_node->right;
                    prev_node->right->left = new_node;
                    prev_node->right = new_node;
                }
            }
        }
    }

    return root;
}


// Remove the columns and nodes covered by the argument
void cover(node_t *node) {
    node_t *head = node->head;

    // remove head from head list
    head->right->left = head->left;
    head->left->right = head->right;

    // remove all nodes in the same row than the ones in this column
    for (node_t *row = head->down; row != head; row = row->down) {
        for (node_t *right_node = row->right; right_node != row; right_node = right_node->right) {
            right_node->up->down = right_node->down;
            right_node->down->up = right_node->up;
            node->head->node_count--;
        }
    }
}

// Restore the columns and nodes covered by the argument
void uncover(node_t *node) {
    node_t *head = node->head;

    // puts back all nodes in the same row than the ones in this column
    for (node_t *row = head->up; row != head; row = row->up) {
        for (node_t *left_node = row->left; left_node != row; left_node = left_node->left) {
            left_node->up->down = left_node;
            left_node->down->up = left_node;
            node->head->node_count++;
        }
    }

    // put head back on head list
    head->right->left = head;
    head->left->right = head;
}

// Find the column with the less elements
node_t *min_column(node_t *pb) {
    node_t *min_node = pb->right, *current_node = pb->right->right;

    while (current_node != pb) {
        if (current_node->node_count < min_node->node_count) {
            min_node = current_node;
        }
        current_node = current_node->right;
    }
    return min_node;
}


// Recursive function that solves the problem
bool solve(node_t *pb, stack_t *sol) {
    // if nothing in pb, problem is solved
    if (pb->right == pb) {
        return true;
    }

    // choose a column
    node_t *best_head = min_column(pb);
    cover(best_head);

    for (node_t *row_node = best_head->down; row_node != best_head; row_node = row_node->down) {
        // add node in solution
        append(sol, row_node);
        for (node_t *right_node = row_node->right; right_node != row_node; right_node = right_node->right) {
            cover(right_node);
        }

        if (solve(pb, sol))
            return true;

        pop(sol);
        for (node_t *left_node = row_node->left; left_node != row_node; left_node = left_node->left) {
            uncover(left_node);
        }
    }

    uncover(best_head);
    return false;
}

// Print a solution
void print_solution(stack_t *sol) {
    printf("Solution: ");
    for (int i = 0; i < sol->size; i++) {
        printf("%d ", sol->tab[i]->row_id);
    }
    printf("\n");
}


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
int *solve_exact_cover(int height, int width, bool **mat, int *n) {
    node_t *pb = from_bool_matrix(height, width, mat);
    stack_t *solution = init_stack(16);
    solve(pb, solution);

    *n = solution->size;
    int *res = malloc(*n * sizeof(*res));
    for (int i = 0; i < *n; i++) {
        res[i] = solution->tab[i]->row_id;
    }

    free_pb(pb, solution);
    return res;
}
