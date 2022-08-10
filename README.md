# REU-Summer2022
Files for my REU summer 2022 on binary lifters

## McSema

Scripts and files relating to [McSema](https://github.com/lifting-bits/mcsema)

### Substitutions
**substitutions.cpp** is a c++ program written with the [LLVM compiler infrastructure](https://llvm.org/docs/index.html). Its purpose is to take a list of equivalent function mapping from a config file (input.conf) and substitute each equivalent function within an LLVM IR file. In its current form, **substitutions.cpp** is only designed to work with IR produced by the binary lifter [McSema](https://github.com/lifting-bits/mcsema), however that may change in future versions of the software.

### liftdrop.sh
Runs through the lifting and recompilation process on a single binary

### sizes.sh
Runs through the lifting and recompilation on all binaries in the current directory. Produces a csv file containing relevant information such as files sizes and the step at which lifting failed.

### substitution.sh
A single proof of concept subsitution that replaces a call to `__printf_chk` with a call to `printf` on all llvm IR files in the current directory. This method of instrumentation proved to not scale to a larger set of transformations.

## Reopt

Scripts and files relating to [Reopt](https://github.com/GaloisInc/reopt)

All files here are currently abandoned as Reopt does not allow for instrumentation of lifted binaries.

### test_reopt.py
A python script that runs a set of binaries to test the success of lifting.
