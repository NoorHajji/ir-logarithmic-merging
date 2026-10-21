# Logarithmic Merging in C++

A C++17 implementation of logarithmic merging for a dynamic inverted index.
New documents are held in memory and flushed into disk generations. Occupied
generations merge like carries in a binary counter. Queries combine the memory
index with all existing generations.

The implementation is in `src/logarithmic.cpp`. The project also includes the
provided auxiliary plus main baseline, index I/O helpers, tools and unit tests.

## Build

Requires CMake 3.14 or later and a C++17 compiler.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 4
```

For Windows with MinGW, select the generator when configuring:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 4
.\build\test_main.exe data/tiny.txt build/test_work
```

## Tests and experiments

```sh
./build/test_main data/tiny.txt build/test_work
./build/gen_corpus data/corpus_50k.txt 50000
./build/test_main data/corpus_50k.txt build/test_big
bash scripts/run_experiments.sh
```

All 12 provided tests passed on the small collection and the 50,000 document
collection. All eight experiment outputs passed the reference verifier.
Generated corpora, results and build files are excluded from version control.

The course starter code supplies the supporting modules. The implemented
`LogarithmicIndex` class handles flushes, lookup, finalization and cleanup.
