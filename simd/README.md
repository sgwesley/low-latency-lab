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
Processing time for 1 million plays (Array of Structures): p50 (1524 us) p90 (2313 us)
Processing time for 1 million plays (Structure of Arrays): p50 (1218 us) p90 (1945 us)
```

Now, compare this with the output using SIMD optimizations, compiled with the same `-march=native` flag:

```
Processing time for 1 million plays (SIMD): p50 (1208 us) p90 (1942 us)
```

**Conclusion:** GCC does a wonderful job. For this specific workload, when using the -march=native flag, the performance difference between a manual SIMD implementation and a Structure of Arrays approach becomes minimal. This is due to the compiler's ability to automatically vectorize the SoA loop. This works well because:
- The data layout is cache friendly, with contiguous and homogeneous arrays
- The CPU supports vector instructions such as AVX2
- The compiler can safely and aggressively apply SIMD transformations

---

## Contributing

Contributions, improvements to benchmarks, and additional measurement scripts are welcome. Please open issues or merge requests on the GitLab repo.

---

## License

MIT — see the repository `LICENSE` file.


