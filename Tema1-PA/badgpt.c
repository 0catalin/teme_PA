#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LENGTH 2000001

typedef struct matrix {
  long upLeft;
  long upRight;
  long downLeft;
  long downRight;
} TMatrix;  // representation of a 2 x 2 matrix

typedef struct vector {
  long x;
  long y;
} TVector;  // representation of a 2 x 1 vector

const TMatrix base = {0, 1, 1, 1};
const TVector baseVector = {0, 1};  // the base vector for fibonacci
const long divider = 1000000007;   // the divider (modulo number)

// adds 2 numbers keeping track of the divider formula
long plus(long x, long y) { return ((x % divider) + (y % divider)) % divider; }

/* multiplies 2 long values, has to cast to long long
before using the divider and then casts back to long and returns */
long mulLong(long x, long y) {
  return (long)((1LL * (x % divider) * (y % divider)) % divider);
}

// multiplies a matrix with a vector, returns a vector
TVector mulMatrixVector(TMatrix m, TVector v) {
  TVector result;
  result.x = plus(mulLong(m.upLeft, v.x), mulLong(m.upRight, v.y));
  result.y = plus(mulLong(m.downLeft, v.x), mulLong(m.downRight, v.y));
  return result;
}

// multiplies 2 matrices, returns another matrix
TMatrix mul(TMatrix m1, TMatrix m2) {
  TMatrix m3;
  m3.upLeft =
      plus(mulLong(m1.upLeft, m2.upLeft), mulLong(m1.upRight, m2.downLeft));
  m3.upRight =
      plus(mulLong(m1.upLeft, m2.upRight), mulLong(m1.upRight, m2.downRight));
  m3.downLeft =
      plus(mulLong(m1.downLeft, m2.upLeft), mulLong(m1.downRight, m2.downLeft));
  m3.downRight = plus(mulLong(m1.downLeft, m2.upRight),
                      mulLong(m1.downRight, m2.downRight));
  return m3;
}

// matrix pow function made with divide et impera
TMatrix matrixPow(long long pow) {
  if (pow == 1)
    return base;         // base case
  else if (pow % 2 == 1)  // if it is an even pow
    return mul(base, matrixPow(pow - 1));
  TMatrix halfPow = matrixPow(pow / 2);  // computes half of the pow
  return mul(halfPow, halfPow);         // multiples it with itself
}

// computes the nth fib number
long fib(long long index) {
  if (index == 0)
    return 0;
  if (index == 1)
    return 1;  // base case
  return (mulMatrixVector(matrixPow(index - 1), baseVector)).y;
}

int main() {
  long res = 1;
  long fibRes = 0;
  FILE *file = fopen("badgpt.in", "r");
  FILE *file_out = fopen("badgpt.out", "w");
  char *buffer = malloc(MAX_LENGTH);
  fgets(buffer, MAX_LENGTH, file);

  int index = 0;

  char c = buffer[index];
  while (c != '\0') {
    if (c == 'n' || c == 'u') {  // only cares about n or u
      index++;
      long long num = 0;
      while (isdigit(buffer[index])) {
        num = num * 10 + (buffer[index] - '0');
        index++;
      }                      // computes the number after the n or u
      fibRes = fib(num + 1);  // gets the number of combinations via fibonacci
      res = mulLong(res, fibRes);  // multiplies the result with the fib number
    } else {
      // just skips until the next n, u or \0
      index++;
    }

    c = buffer[index];
  }

  fprintf(file_out, "%ld\n", res);
  free(buffer);
  fclose(file_out);
  fclose(file);
  return 0;
}
