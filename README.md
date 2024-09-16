# MagicCube
filters all permutaions of a given set that satisfy the requirements for a magic shape (square/cube/hypercube) of dimension k and order n

## Usage

```MagicCube.exe -k [1,2,3,...] -n [1,2,3,...]```

## Details

Utilizes multithreading in order to speed up things (operation is at least of complexity `O(n^2)`). The given set of numbers e.g. for `k = 2` and `n = 3` consists of the set `{1,2,3,4,5,6,7,8,9}` stored in a 1D array and interpreted as a cube with k dimensions, where each has a length of 3:

```
{{1,2,3},
{4,5,6},
{7,8,9}}
```

The program iterates through all possible permutations of the given set and checks if the sum of each `row`, `collumn`, `pillar`, `file`, ... equals the magic number M(n) = (n^k + 1) * n / 2. When the condition is satisfied it prints the corresponding set to the console.

(Work in progress, may contain errors and is more or less just for fun)
