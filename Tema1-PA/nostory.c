#include <stdio.h>
#include <stdlib.h>

// pair of elements
typedef struct pair {
  long elem1;
  long elem2;
} TPair;

long max(long elem1, long elem2) {
  if (elem1 > elem2)
    return elem1;
  return elem2;
}

long min(long elem1, long elem2) {
  if (elem1 > elem2)
    return elem2;
  return elem1;
}

// compares 2 numbers, will sort everything decreasingly
int compare(const void *a, const void *b) {
  long first = *(long *)a;
  long second = *(long *)b;
  return (first > second) ? -1 : (first < second);
}

// compare function, compares first half, orders depending on max element,
// increasingly
int compare1st(const void *a, const void *b) {
  TPair *first = (TPair *)a;
  TPair *second = (TPair *)b;

  long max_first = (first->elem1 > first->elem2) ? first->elem1 : first->elem2;
  long max_second =
      (second->elem1 > second->elem2) ? second->elem1 : second->elem2;

  if (max_first > max_second)
    return 1;
  if (max_first < max_second)
    return -1;
  return 0;
}

// compare function, compares second half, orders depending on min element,
// increasingly
int compare2nd(const void *a, const void *b) {
  TPair *first = (TPair *)a;
  TPair *second = (TPair *)b;
  long min_first = min(first->elem1, first->elem2);
  long min_second = min(second->elem1, second->elem2);

  if (min_first > min_second)
    return -1;
  if (min_first < min_second)
    return 1;
  return 0;
}

int main() {
  int D, N;
  FILE *file = fopen("nostory.in", "r");
  FILE *file_out = fopen("nostory.out", "w");
  fscanf(file, "%d", &D);
  fscanf(file, "%d", &N);
  long sum = 0;

  if (D == 1) {
    // if we have an unlimited number of moves we just take the n highest
    // numbers from the vector and add them
    long *numbers = malloc(2 * N * sizeof(long));
    for (int i = 0; i < 2 * N; i++) {
      fscanf(file, "%ld", &numbers[i]);
    }
    qsort(numbers, 2 * N, sizeof(long), compare);
    for (int i = 0; i < N; i++) {
      sum += numbers[i];
    }
    free(numbers);
  } else if (D == 2) {
    int K;

    long middle_value_elem;
    fscanf(file, "%d", &K);

    long *numbers = malloc(2 * N * sizeof(long));
    TPair *pairs = malloc(sizeof(struct pair) * N);

    // we initialize the vector and the pairs
    for (int i = 0; i < 2 * N; i++) {
      fscanf(file, "%ld", &numbers[i]);
      if (i < N) {
        pairs[i].elem1 = numbers[i];
      } else {
        pairs[i - N].elem2 = numbers[i];
        sum += max(pairs[i - N].elem1, numbers[i]);
      }
    }

    qsort(numbers, 2 * N, sizeof(long),
          compare);  // sort the first array to find the middle
    middle_value_elem = numbers[N - 1];
    // if number >= middle_value_elem it belongs to the right side else to the
    // left side

    TPair *sorted_pairs = malloc(sizeof(struct pair) *
                                 N);  // pointer to where the sorted pairs are
    // pointers to start of first and 2nd half
    TPair *first_1st_half = sorted_pairs,
          *first_2nd_half = &sorted_pairs[N - 1];
    for (int i = 0; i < N; i++) {
      // first half of pairs has ONLY elements lower than the middle
      if (pairs[i].elem1 < middle_value_elem &&
          pairs[i].elem2 < middle_value_elem) {
        *first_1st_half = pairs[i];
        first_1st_half++;
        // second half of pairs has ONLY elements higher than the middle
      } else if (pairs[i].elem1 >= middle_value_elem &&
                 pairs[i].elem2 >= middle_value_elem) {
        *first_2nd_half = pairs[i];
        first_2nd_half--;
      }
    }
    first_2nd_half++;

    int size_1st_half = first_1st_half - sorted_pairs;
    int size_2nd_half = &sorted_pairs[N - 1] - first_2nd_half + 1;
    qsort(sorted_pairs, size_1st_half, sizeof(TPair), compare1st);
    qsort(first_2nd_half, size_2nd_half, sizeof(TPair), compare2nd);
    // we sort based on the compare functions presented above

    int left_1st = 0, left_2nd = 0;
    // while we do not reach the end and we have moves left
    while (left_1st < size_1st_half && left_2nd < size_2nd_half && K > 0) {
      sum -= max(sorted_pairs[left_1st].elem1, sorted_pairs[left_1st].elem2);
      sum +=
          min(first_2nd_half[left_2nd].elem1, first_2nd_half[left_2nd].elem2);
      // the current sum gets optimized by the difference of the min elem of the
      // 2nd pair and the max of the first
      left_1st++;
      left_2nd++;
      K--;
    }
    free(numbers);
    free(pairs);
    free(sorted_pairs);
  }

  fprintf(file_out, "%ld\n", sum);
  fclose(file_out);

  fclose(file);
  return 0;
}
