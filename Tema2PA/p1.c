#include <stdio.h>
#include <stdlib.h>


// pair structure
typedef struct pair {
    int distance;
    int index;
} TPair;


// compare function which sorts all the pairs
// ascending based on the distance from the first node
int compare(const void *a, const void *b) {
    TPair *pa = (TPair *)a;
    TPair *pb = (TPair *)b;
    return pa->distance - pb->distance;
}


int main() {
    int N;
    FILE *file = fopen("p1.in", "r");
    FILE *file_out = fopen("p1.out", "w");

    // allocate memory for the pairs
    fscanf(file, "%d", &N);
    TPair* pairArray = malloc(sizeof(struct pair) * N);

    fscanf(file, "%d", &pairArray[0].distance);
    pairArray[0].index = 1;

    // connectNode is the most recent node at distance d
    //  such that we can connect the current node (at distance d+1)
    // to it. We update it if we move to a deeper level.
    TPair connectNode = {0, 1};

    // initialize the array, with attention at the index (indexes start from 1)
    for (int i = 1; i < N; i++) {
        fscanf(file, "%d", &pairArray[i].distance);
        pairArray[i].index = i + 1;
    }

    // we sort the array ascending based on the distances
    qsort(pairArray, N, sizeof(TPair), compare);

    for (int i = 1; i < N; i++) {
        // if there is any node with 0 distance (not the first one)
        // or if any node is 2 positions higher than another
        // we can't create the graph
        if (pairArray[i].distance > (pairArray[i - 1].distance + 1)
                        || pairArray[i].distance == 0) {
            fprintf(file_out, "%d\n", -1);
            free(pairArray);
            fclose(file_out);
            fclose(file);
            return 0;
        }
    }

    // the graph will have N - 1 edges since we have N nodes
    fprintf(file_out, "%d\n", N - 1);

    for (int i = 1; i < N; i++) {
        // if the "last node" is 2 edges away from the current node
        // we can't connect it to our node anymore so we reinitialize it
        // with the node before the last since it is guaranteed to be
        // 1 edge away P.S. we could have done that just by a
        // reinitialization with the pair array element but
        // I will leave it this way
        if (connectNode.distance + 1 < pairArray[i].distance) {
            connectNode.distance = pairArray[i - 1].distance;
            connectNode.index = pairArray[i - 1].index;
        }
        // print the edge
        fprintf(file_out, "%d %d\n", connectNode.index, pairArray[i].index);
    }

    // close the file
    fclose(file_out);
    fclose(file);
    return 0;
}
