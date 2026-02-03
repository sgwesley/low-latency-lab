# soa-vs-aos ✅

## Overview

This project is an experiment in the *Low Latency Lab* that compares performance characteristics between two memory layouts:

- **Array of Structures (AoS)** — an array where each element is a struct containing all fields.
- **Structure of Arrays (SoA)** — a struct of separate arrays, one per field.

The goal is to measure how layout affects the performance for a simple, realistic workload.

---

## The Lottery Problem

The program models a simplified lottery processor used as an exercise workload:

- Input: a list of lottery entries in a simple text format (`sample/input_sample.txt`).
- Each entry contains five numbers (1-50).
- The processor reads the entries, filters/validates them, performs a small amount of processing, and computes aggregate results (e.g., winners, counts, sums).

This workload is intentionally compact and representative: it performs light compute and lots of memory access, which makes the effect of data layout (AoS vs SoA) observable in wall-clock time and microbenchmarks.

---

## Implementation Details

- C++ project using **CMake**.
- Two implementations are provided: one using AoS and one using SoA. Both share the same logic and test suite to keep comparisons fair.
- Unit tests are included under `tests/` and quick sample I/O under `sample/input_sample.txt`.

---

## Build Instructions

```bash
# from project root
mkdir -p build
cmake -S . -B build -DBUILD_TESTS=ON -DENABLE_SOA=ON # or -DENABLE_SOA=OFF for AoS
cmake --build build --parallel $(nproc)
```

Artifacts are placed in `build/bin/`:

- `app` — demo application that reads an input file and runs the processor
- `run_tests` — built test runner

---

## Run & Test

Run the demo with the sample input:

```bash
./build/bin/app sample/input_sample.txt
```

Run unit tests:

```bash
cd build
ctest --output-on-failure
# or
./build/bin/run_tests
```

---

## Benchmarking & Reproducible Measurements

### Running application with `perf`

**Array of Structures (AoS)**

```
$ perf stat -e cache-misses,cycles,instructions ./build/bin/app sample/input_sample.txt
READY
1 2 3 4 5
479 21 1 0
(elapsed time: 1 ms)

 Performance counter stats for './build/bin/app sample/input_sample.txt':

           237,043      cache-misses                                                          
        29,944,509      cycles                                                                
        51,259,294      instructions                     #    1.71  insn per cycle            

       3.982141864 seconds time elapsed

       0.016194000 seconds user
       0.011431000 seconds sys
```

**Structure of Arrays (SoA)**

```
$ perf stat -e cache-misses,cycles,instructions ./build/bin/app sample/input_sample.txt
READY
1 2 3 4 5
479 21 1 0
(elapsed time: 1813 us)

 Performance counter stats for './build/bin/app sample/input_sample.txt':

           195,122      cache-misses                                                          
        27,358,652      cycles                                                                
        50,287,396      instructions                     #    1.84  insn per cycle            

       4.147494362 seconds time elapsed

       0.013879000 seconds user
       0.010905000 seconds sys
```

**Outcome:**  The memory layout has a significant impact on cache misses and overall performance. In this example, the SoA layout shows fewer cache misses (20%) and better instruction per cycle (IPC) compared to AoS.

### Processing time with 1 million plays

The unit test `ValidatingProcessingTimeWith1MPlays` measures processing time for 1 million lottery entries and prints the elapsed time in microseconds for the **90th percentile** over multiple runs.

**Array of Structures (AoS)**

```
$ cmake -S . -B build -DBUILD_TESTS=ON -DENABLE_SOA=OFF
$ cmake --build build --parallel && ctest --test-dir build --output-on-failure --verbose
```

Output snippet:
```
[ RUN      ] LotteryProcessorTest.ValidatingProcessingTimeWith1MPlays
...
Processing time for 1 million plays: 3322 us
[       OK ] LotteryProcessorTest.ValidatingProcessingTimeWith1MPlays (8712 ms)
```

**Structure of Arrays (SoA)**

```
$ cmake -S . -B build -DBUILD_TESTS=ON -DENABLE_SOA=ON
$ cmake --build build --parallel && ctest --test-dir build --output-on-failure --verbose
```

Output snippet:
```
[ RUN      ] LotteryProcessorTest.ValidatingProcessingTimeWith1MPlays
...
Processing time for 1 million plays (with Structure of Arrays): 3201 us
[       OK ] LotteryProcessorTest.ValidatingProcessingTimeWith1MPlays (8268 ms)
```

**Outcome:** The SoA implementation again shows a performance improvement, with a lower processing time for the same workload.

---

## Contributing

Contributions, improvements to benchmarks, and additional measurement scripts are welcome. Please open issues or merge requests on the GitLab repo.

---

## License

MIT — see the repository `LICENSE` file.


