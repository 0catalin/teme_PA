#include <stdio.h>
#include <stdlib.h>

int get_bit(int size, int x, int y) {
  int res = 0;
  while (size != 1) {  // the base case
    int half_size =
        size >> 1;  // division by 2, bitwise since we know it is a power of 2
    if (x <= half_size && y > half_size) {  // second quadrant
      y = y - half_size;
    } else if (x > half_size && y <= half_size) {  // 3rd quadrant
      x = x - half_size;
    } else if (x <= half_size && y <= half_size) {  // first quadrant
      size = half_size;
    } else {
      // 4th quadrant needs to change bit (1 - 0 = 1, 1 - 1 = 0 is perfect)
      res = 1 - res;
      y = y - half_size;
      x = x - half_size;
    }
    size = half_size;
  }
  return res;
}

int main() {
  FILE *file = fopen("walsh.in", "r");
  FILE *file_out = fopen("walsh.out", "w");
  int N, K;

  fscanf(file, "%d", &N);
  fscanf(file, "%d", &K);

  int x, y;
  int res;

  for (int i = 0; i < K; i++) {
    fscanf(file, "%d", &x);
    fscanf(file, "%d", &y);

    res = get_bit(N, x, y);  // uses the function for each pair of coordinates
    fprintf(file_out, "%d\n", res);
  }

  fclose(file_out);
  fclose(file);
  return 0;
}
