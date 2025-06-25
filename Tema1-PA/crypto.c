#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const long divider = 1000000007;  // the divider (modulo number)

// adds 2 numbers keeping track of the divider formula
long plus(long x, long y) { return ((x % divider) + (y % divider)) % divider; }

// multiplies 2 long values, has to cast to long long before using the divider
// and then casts back to long and returns
long mul(long x, long y) {
  return (long)((1LL * (x % divider) * (y % divider)) % divider);
}

long count_distinct_characters(char *str) {
  bool hasAppeared[256] = {0};  // apparition vector
  long count = 0;

  for (int i = 0; str[i] != '\0'; i++) {
    unsigned char ch =
        (unsigned char)str[i];  // unsigned char because you need 0:255 values
    if (!hasAppeared[ch]) {    // if it hasn't appeared increments the result
      hasAppeared[ch] = true;
      count++;
    }
  }
  return count;
}

int main() {
  FILE *file = fopen("crypto.in", "r");
  FILE *file_out = fopen("crypto.out", "w");

  int N, L;
  fscanf(file, "%d", &N);
  fscanf(file, "%d", &L);

  char *string = malloc((N + 1) * sizeof(char));
  char *subseq = malloc((L + 1) * sizeof(char));

  fscanf(file, "%s", string);
  fscanf(file, "%s", subseq);

  string[N] = '\0';
  subseq[L] = '\0';

  long **dp = malloc((N + 1) * sizeof(long *));
  for (int i = 0; i < (N + 1); i++) {
    dp[i] = calloc((L + 1), sizeof(long));
  }

  long distinct_characters = count_distinct_characters(subseq);

  dp[0][0] = 1;  // the "" (empty) sequence appears once in the empty string
  for (int i = 1; i < N + 1; i++) {
    if (string[i - 1] == '?')
      dp[i][0] = mul(distinct_characters,
                     dp[i - 1][0]);  // you get {distinct_characters} more
                                    // possible strings for each '?'
    else
      dp[i][0] = dp[i - 1][0];  // otherwise same amount
  }

  for (int i = 1; i < (N + 1); i++) {
    for (int j = 1; j < (L + 1); j++) {
      if (string[i - 1] == '?')
        dp[i][j] =
            plus(mul(distinct_characters, dp[i - 1][j]), dp[i - 1][j - 1]);
      // adds the prev possibilities to the multiplication of
      // all the possible characters and the same subseq in prev string, since
      // it has a ramification for each '?'

      // character match
      else if (string[i - 1] == subseq[j - 1])
        // takes the prev match (dp[i - 1][j - 1]) and adds the previous
        // possibilities of the same subseq in the prev string adds the subseq
        // number on the string before the last character and the subseq number
        dp[i][j] = plus(dp[i - 1][j - 1], dp[i - 1][j]);
      else                       // no character match
        dp[i][j] = dp[i - 1][j];  // the subseq number is the same
    }
  }

  fprintf(file_out, "%ld\n", dp[N][L]);

  // frees memory
  for (int i = 0; i < (N + 1); i++) {
    free(dp[i]);
  }
  free(dp);

  fclose(file);
  fclose(file_out);
  return 0;
}
