#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// pair structure
typedef struct pair {
  int l1;
  int l2;
} TPair;

// node for queue which accepts pair elements
// we need pairs for line and colomn of node in BFS
typedef struct node {
  int l1;
  int l2;
  struct node *next;
} Node;

// queue structure with head and tail for O(1) access
typedef struct queue {
  Node *head, *tail;
  int size;
} * TQueue;

// Returns the maximum between two integers
int max(int a, int b) { return (a > b) ? a : b; }

// Returns the maximum between two long integers
long max_long(long a, long b) { return (a > b) ? a : b; }

// Returns the minimum between two long integers
long min_long(long a, long b) { return (a < b) ? a : b; }

// Initializes a new node with values l1 and l2, and returns a pointer to it
Node *initNode(int l1, int l2) {
  Node *node = malloc(sizeof(struct node));
  node->l1 = l1;
  node->l2 = l2;
  node->next = NULL;
  return node;
}

// Frees the memory allocated for a node and returns NULL
Node *freeNode(Node *node) {
  if (node)
    free(node);
  return NULL;
}

// Initializes a queue with a single node containing values l1 and l2
TQueue initQueue(int l1, int l2) {
  TQueue queue = malloc(sizeof(struct queue));
  queue->head = queue->tail = initNode(l1, l2);
  queue->size = 1;
  return queue;
}

// Checks if the queue is empty or uninitialized
int isEmpty(TQueue queue) {
  return queue == NULL || queue->head == NULL || queue->size == 0;
}

// Removes the front element from the queue and frees its memory
TQueue dequeue(TQueue queue) {
  Node *tmp;
  if (!isEmpty(queue)) {
    tmp = queue->head;
    queue->head = queue->head->next;
    tmp = freeNode(tmp);
    queue->size--;
  }
  return queue;
}

// Returns the values of the front node in the queue as a TPair
// Exits the program if the queue is empty
TPair front(TQueue queue) {
  if (!isEmpty(queue)) {
    TPair pair;
    pair.l1 = queue->head->l1;
    pair.l2 = queue->head->l2;
    return pair;
  } else {
    exit(1);
  }
}

// Adds a new node with values l1 and l2 to the end of the queue
// If the queue is empty or NULL, a new queue is initialized
TQueue enqueue(TQueue queue, int l1, int l2) {
  Node *node;
  if (isEmpty(queue)) {
    if (queue == NULL)
      return initQueue(l1, l2);
    free(queue);
    return initQueue(l1, l2);
  }
  node = initNode(l1, l2);
  queue->tail->next = node;
  queue->tail = node;
  queue->size++;
  return queue;
}

// Frees all nodes in the queue and the queue structure itself
TQueue freeQueue(TQueue queue) {
  while (!isEmpty(queue))
    queue = dequeue(queue);
  if (queue)
    free(queue);
  return NULL;
}

int doBFS(long **mat, int BFS_idx, bool **visited, int **BFS_visited, int line,
          int column, int K, int N, int M) {
  int dRow[] = {0, 1, 0, -1};
  int dCol[] = {-1, 0, 1, 0};

  int i = 0, j = 0;
  long dif = 0;

  // the returned size of the cluster
  int size = 0;

  // reference for the lowest element in the BFS
  // (his neighbours are only the elements higher
  // than this limit + K)
  long reference_element = mat[line][column];
  TPair pair;

  // init queue with first indexes
  TQueue q = initQueue(line, column);
  // we mark the node as visited
  BFS_visited[line][column] = BFS_idx;

  // just BFS
  while (!isEmpty(q)) {
    pair = front(q);
    q = dequeue(q);
    size++;

    for (int k = 0; k < 4; k++) {
      // compute the indexes of the possible neighbours
      i = pair.l1 + dRow[k];
      j = pair.l2 + dCol[k];

      // a check if the indexes are valid
      if (i >= 0 && i < N && j >= 0 && j < M) {
        // we compute the difference
        dif = mat[i][j] - reference_element;

        // only take unvisited nodes with a valid difference
        if (BFS_visited[i][j] != BFS_idx && dif >= 0 && dif <= K) {
          BFS_visited[i][j] = BFS_idx;

          // the optimization -> no need to
          // traverse this in another BFS
          if (dif == 0)
            visited[i][j] = true;

          q = enqueue(q, i, j);
        }
      }
    }
  }

  // free memory
  q = freeQueue(q);
  return size;
}

int main() {
  int res = 0, BFS_idx = 0;
  int N, M, K;
  long maxmat = 0;
  long minmat = LONG_MAX;
  FILE *file = fopen("p2.in", "r");
  FILE *file_out = fopen("p2.out", "w");

  fscanf(file, "%d %d %d", &N, &M, &K);

  // a matrix for each done BFS, standard visited matrix
  int **BFS_visited = malloc(sizeof(int *) * N);

  // a matrix for each BFS start (and not only -> my optimization)
  bool **visited = malloc(sizeof(bool *) * N);

  // a matrix for storing the values
  long **mat = malloc(sizeof(long *) * N);

  // initialization of the matrices
  for (int i = 0; i < N; i++) {
    visited[i] = calloc(M, sizeof(bool));
    BFS_visited[i] = calloc(M, sizeof(int));
    mat[i] = malloc(M * sizeof(long));
    for (int j = 0; j < M; j++) {
      fscanf(file, "%ld", &mat[i][j]);
      maxmat = max_long(mat[i][j], maxmat);
      minmat = min_long(mat[i][j], minmat);
    }
  }

  // if we can include all the elements in the interval
  // no need to calculate anything, the result is all the elements of the matrix
  if (maxmat - minmat <= K) {
    for (int i = 0; i < N; i++) {
      free(visited[i]);
      free(BFS_visited[i]);
      free(mat[i]);
    }

    free(visited);
    free(BFS_visited);
    free(mat);
    fprintf(file_out, "%d\n", N * M);
    fclose(file_out);
    fclose(file);
    return 0;
  }

  // for each element in the matrix, do bfs and keep the max cluster of nodes
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
      // we do this condition because the optimization
      // may visit nodes we won't be starting the BFS with
      if (!visited[i][j]) {
        res =
            max(res, doBFS(mat, BFS_idx, visited, BFS_visited, i, j, K, N, M));
        // the id of BFS so that we don't reallocate matrices everytime
        BFS_idx++;
        visited[i][j] = true;
      }
    }
  }

  // we free the memory
  for (int i = 0; i < N; i++) {
    free(visited[i]);
    free(BFS_visited[i]);
    free(mat[i]);
  }
  free(visited);
  free(BFS_visited);
  free(mat);

  // print the result and close the files
  fprintf(file_out, "%d\n", res);
  fclose(file_out);
  fclose(file);
  return 0;
}
