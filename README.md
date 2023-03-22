# Calculation of π Using the Monte Carlo Method (using C and OpenMP)

Concurrency and Parallelism 2021-22

Hervé Paulino <<herve.paulino@fct.unl.pt>>

Alex Davidson <<a.davidson@fct.unl.pt>>

March 2023

## Assignment

Task: Write a parallel implementation of the Monte Carlo method simulation for estimating the size of π using C (or C++) and [OpenMP](https://www.openmp.org/).

Assignment PDF: <https://raw.githubusercontent.com/MEI-CP/lab-assignments/main/lab03-pi-montecarlo-openmp.pdf>

## Installation

OpenMP should be packaged with the `gcc` compiler on your system. Note that in Mac OSX the standard `gcc` program by default uses `clang` as the compiler, so you may need to install `gcc` directly (e.g. using homebrew).

## Demos

There are some simple demonstration source file for showing how to run a parallel loop using OpenMP in the `c/openmp-demo` folder.

You can compile using the following:

```
gcc -O3 -fopenmp <src-file>.c -o <output_file>
```

You should be able to use these examples to help you build a solution to the Monte Carlo simulation. You can also use [these slides](https://courses.grainger.illinois.edu/cs484/sp2020/6_merged.pdf) and the [official documentation](https://www.openmp.org/spec-html/5.0/openmp.html) to learn more about the openmp syntax.

### Sleep
****
The `c/openmp-demo/demo-sleep.c` simply sleeps on every iteration for `0.00001` seconds. When running the code with multiple threads, this shows how performance can be improved compared to an invocation with a single thread.

### Variable increment

The `c/openmp-demo/demo-inc.c` file shows how to increment a variable across multiple threads atomically using the openmp syntax. Note that performance of this code is not guaranteed to improve when introducing more threads.

## Performance profiling

To learn more about the performance details (i.e. memory/CPU usage) of your programme, it is possible to use tools such as `gprof` (only compatible with Linux) or `pprof` (gperftools) to execute your binaries with profiling enabled, and so that you can later visualise execution loads.

The following is an example of how to get this working with `pprof`. If you want to use `gprof`, you can add the `-pg` flag when compiling your code.

### Install (`pprof`)

```bash
brew install gperftools # Mac OSX
apt install google-perftools # Linux (ubuntu)
```

### Compiling and running code (`pprof`)

For example, using the `demo-sleep` C code.

```bash
$ gcc -O3 -fopenmp demo-sleep.c -o demo-sleep -lprofiler -L/usr/local/opt/gperftools/lib # compile program with profiling enabled
$ env LD_PRELOAD=/usr/local/lib/libprofiler.so CPUPROFILE=main.prof ./demo-sleep 10000 1 # generate profile file
```

The `CPUPROFILE` env variable shows where the output profile file will be generated.

### Visualisations of profiles

```
$ pprof --text ./demo-sleep main.prof # analyse profile file with text output
```

You can other methods of visualisation using alternative `pprof` flags such as:
```
Output type:
   --text              Generate text report
   --stacks            Generate stack traces similar to the heap profiler (requires --text)
   --callgrind         Generate callgrind format to stdout
   --gv                Generate Postscript and display
   --evince            Generate PDF and display
   --web               Generate SVG and display
   --list=<regexp>     Generate source listing of matching routines
   --disasm=<regexp>   Generate disassembly of matching routines
   --symbols           Print demangled symbol names found at given addresses
   --dot               Generate DOT file to stdout
   --ps                Generate Postscript to stdout
   --pdf               Generate PDF to stdout
   --svg               Generate SVG to stdout
   --gif               Generate GIF to stdout
   --raw               Generate symbolized pprof data (useful with remote fetch)
   --collapsed         Generate collapsed stacks for building flame graphs
```

Try to find methods of visualisation that allow you to make sense of the output, the `--collapsed` may help using the blog post written [here](https://www.brendangregg.com/FlameGraphs/cpuflamegraphs.html).
