import random

N = 100
M = 100
MAX_VAL = 1_000_000

# Generate a random K less than 1,000,000,000
K = random.randint(0, MAX_VAL - 1)

# Print the first line: "100 100 <K>"
print(f"{N} {M} {K}")

# Print the 100x100 grid of random integers < 1,000,000,000
for _ in range(N):
    row = [str(random.randint(0, MAX_VAL - 1)) for _ in range(M)]
    print(" ".join(row))