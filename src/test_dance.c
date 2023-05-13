#include <stdio.h>
#include <stdlib.h>

#include "dancing_links.h"


/* This files test the use of de dancing links library with an example.

This example is the following matrix:
   0 ( 0 0 1 0 1 1 0 )
   1 ( 1 0 0 1 0 0 1 )
   2 ( 0 1 1 0 0 1 0 )
   3 ( 1 0 0 1 0 0 0 )
   4 ( 0 1 0 0 0 0 1 )
   5 ( 0 0 0 1 1 0 1 )

We can see that lines 0, 3 and 4 make a solution of the problem.

*/


int main(int argc, char *argv[]) {
    int l = 6, c = 7;

    bool *tab = calloc(l * c, sizeof(*tab));
    bool **mat = malloc(l * sizeof(*mat));
    for (int i = 0; i < l; i++) {
        mat[i] = tab + i * c;
    }

    int ones[] = {2,4,5,7,10,13,15,16,19,21,24,29,34,38,39,41};
    for (int i = 0; i < 16; i++)
        tab[ones[i]] = true;
    
    int n;
    int *res = solve_exact_cover(l, c, mat, &n);
    for (int i = 0; i < n; i++) {
        printf("%d ", res[i]);
    }
    printf("\n");

    free(res);
    free(tab);
    free(mat);
    return 0;
}