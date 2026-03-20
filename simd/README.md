# SIMD (Single Instruction, Multiple Data) ✅

## Overview

This project is an experiment in the *Low Latency Lab* that extends the [SoA-vs-AoS](../soa-vs-aos) experiment and compares performance characteristics of using SIMD computing techniques to optimize the processing of lottery entries.

For an overview of the base problem and implementation details, please refer to the [SoA-vs-AoS README](../soa-vs-aos/README.md). This project builds on that foundation and focuses specifically on SIMD related optimizations

---

## Build Instructions

```bash
# from project root
mkdir -p build
cmake -S . -B build -DBUILD_TESTS=ON
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

The unit test `ValidatingProcessingTimeWith1MPlays` measures processing time for 1 million lottery entries and prints the elapsed time in microseconds for the **50th percentile** and **90th percentile** over multiple runs.

After building **SoA-vs-AoS** with the `-march=native` optimization flag, the output on each run is as follows:

```
Processing time for 1 million plays (Array of Structures): p50 (1015 us) p90 (1172 us)
Processing time for 1 million plays (Structure of Arrays): p50 (913 us) p90 (1042 us)
```

Now, compare this with the output using SIMD optimizations, compiled with the same `-march=native` flag:

```
Processing time for 1 million plays (SIMD): p50 (786 us) p90 (924 us)
```

Note that actual times may vary based on the machine and its current load. In this case, the machine was only running the benchmark and had no other significant background processes.

**Conclusion:** After implementing the AND operation and popcount approach using AVX2 (128 bit SIMD instructions), I saw a measurable improvement:
- around 14% reduction in p50 latency
- around 12% reduction in p90 latency

Not massive, but real and consistent.

Lessons learned:
1. Without measurement, we are just guessing.
2. If you are iterating over contiguous memory and not getting the expected results from auto vectorization, it is worth exploring SIMD.

---

## Contributing

Contributions, improvements to benchmarks, and additional measurement scripts are welcome. Please open issues or merge requests on the GitLab repo.

---

## License

MIT — see the repository `LICENSE` file.


