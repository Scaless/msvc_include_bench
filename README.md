

# \#include \<benchmarks\>
Benchmark of MSVC STL and other headers on compilation time.

If you just want to see the results, go HERE (link to be added later)

This was partially inspired by this [C++ Compile Health](https://artificial-mind.net/projects/compile-health/) post in 2020, which had #include timing for GCC and Clang, albeit a bit dated at this point. Since I primarily develop on Windows, I wanted to figure out similar numbers for MSVC.

## Running the Tests
The test script is set up assuming you have Visual Studio 2022 Community with the C++ compiler installed. If you have Professional or Enterprise versions installed, you'll have to change the path for`vcvarsall.bat` in `run_test.bat`.

Once that's set up, just run `run_test.bat`.

run_test allows the following flags:
* /analyzeonly : Compiles the analysis program and does not run tests. Use for faster iteration of analysis program.

At the moment, running the tests produces about 1GB of artifacts (/build, /results, /temp folders).

## \#define WIN32_LEAN_AND_MEAN
In 2009, Raymond Chen had [some words](https://devblogs.microsoft.com/oldnewthing/20091130-00/?p=15863) on the `WIN32_LEAN_AND_MEAN` definition:

> Moore’s Law and precompiled headers have conspired to render the `WIN32_LEAN_AND_MEAN` symbol relative useless. It doesn’t really save you much any more. But at one point, it did.

It is true that we are not shaving off seconds anymore, but halving the include time for the price of a single define seems well worth it to me.

## Methodology

There are a few compiler flags that we are utilizing to derive information from:

 `/d1reportTime` for header timing.
 `/EP` for line counts without comments.
 `/EP /C` for line counts with comments.

The counts for lines of code are totals of all dependencies *after* preprocessing.

Some additional flags are available to us (`/Bt+` and `/d2cgsummary`), but their outputs do not provide any particularly useful information.

The difference in size between the baseline binary and the one including the entire STL is 1kB, on a 112kB file. It might be interesting to find out where that difference comes from, but it's trivial enough to ignore.

Debug vs. Release has a marginal effect on processed header sizes, typically less than 5% when comparing non-commented lines of code. Tests were run using the Release configuration.

Tests were run using the following hardware and software:

| Product| Version |
|--|--|
| CPU | Ryzen 9 5950X |
| RAM | 128GB DDR4 3200Mhz |
| SSD | Samsung 990 PRO 2TB |
| Windows | 10.0.19045
| Visual Studio | 2022 / 17.9.6 |
| MSVC | 14.39.33519 |
| Windows SDK | 10.0.22621 |
