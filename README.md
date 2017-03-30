# What?

**TBD**

# Repo Structure

```bash
./code       # The source code. Here will be the files you want to use.
./tests      # Contains test code. Usually not relevant for redistribution.
./tools      # Contains scripts and other tools to work with the repo or aid in building code.
./_build     # Generated directory that contains build artifacts.
./_workspace # Generated directory for intermediate files used by tools.
```

# Build

**TBD**

# TODO

* Tool to create self-contained single-file that does not depend on other mtb headers.
  * Scan includes and detect mtb headers and automatically include them there?
  * Must also take care of `MTB_*_IMPLEMENTATION`
* Merge `mtb_assert.hpp` into `mtb.hpp`
* Use FASTBuild as build system.

# The name?

MTB - **M**anuzor **T**ool **B**ox
