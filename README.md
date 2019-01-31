# fixpq

Simple small app which removed `    AS integer` from SQL dump which is invalid for postgresql 9.6 for `SEQUENCE`. This statement was added in postgresql 10.0.

## Build

```bash
mkdir build
cd build
cmake ..
make

# OR
gcc -I./include ./src/main.c -o fixpq
```

## How to use

```bash
fixpq -f ./db/structure.sql # if output is same as input
fixpq -f ./db/structure.sql -o ./db/structure.fixed.sql # if you want to write somewhere else
```

