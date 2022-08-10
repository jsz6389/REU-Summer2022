# substitutions.cpp
**substitutions.cpp** is a c++ program written with the [LLVM compiler infrastructure](https://llvm.org/docs/index.html). Its purpose is to take a list of equivalent function mapping from a config file (input.conf) and substitute each equivalent function within an LLVM IR file. In its current form, **substitutions.cpp** is only designed to work with IR produced by the binary lifter [McSema](https://github.com/lifting-bits/mcsema), however that may change in future versions of the software.
# Table of Contents
* [Building](#building)
* [Usage](#usage)
* [Function Maps](#function-maps)
    - [Config File](#config-file)
* [Limitations](#limitations)
# Building
To build **substitutions.cpp**, simply clone this directory, and run **build.sh**. This will produce an `a.out`, that is the compiled program.

If you are getting weird llvm errors when running the program, try changing the `llvm-config` in **build.sh** to the version that the IR file is written in. For example, if the IR files are written in LLVM 9, then change it to `llvm-config-9`.
# Usage
Once **substitutions.cpp** has been compiled, substitutions can be performed with the following syntax:
```
a.out [IR file]
```
Substitutions can be performed on as many IR files as you want at a time, and bash-style wildcards are supported:
```
a.out [IR file 1] [IR file 2] ... [IR file n] #Run on n IR files at a time
a.out *.ll #Using bash-style wildcards run on all files with the extension .ll
```
Once run, the program will create an *output* directory containing all of the transformed IR files.
# Function Maps
In their current form, function maps have three values; the original function name, the new function name, and the argument map. These values are explained in further detail below.

**Original Function Name** `const char* og_func` : The name of the original function

**New Function Name** `const char* new_func` : The name of the new function that will replace the original function

**Argument Map** `std::map<int, int> args` : A mapping of the arguments of the original function onto the new function. To understand this, think of each argument of a function as the index of an array. Lets take the function header for *div* for example: `div_t div(int numer, int denom)`. *div* takes 2 integer arguments: *numer* and *denom*. In this case, the index of *numer* would be **0** and the index of denom would be **1**.

Now lets say we have an equivalent function `div_t div_backwards(int denom, int numer)`. This is the same function, but the function arguments are flipped. So, if we wanted to create an argument map from `div` onto `div_backwards` it would look something like this:
```
args[0] = 1;
args[1] = 0;
```

While these three values cover a lot of the substitutions that one may want to perform, they are not comprehesive, and I hope to add more functionality to function maps in the future.
## Config File
The config is essentially a file of comma separated values. The first column represents the original function name, the second column represents the new function name, and the third column represents the argument map. The argument map is a set of key:value pairs. Each key:value pair is separated by |. Comments can be made using #. Going back to our *div* example from before:
```
#This is a comment
div,div_backwards,1:0|0:1
```
For more realistic examples, see [input.conf](https://github.com/jsz6389/REU-Summer2022/blob/main/mcsema/substitutions/input.conf)

The config file must be called input.conf and must be in the same directory as the program when it is run.

# Limitations

## McSema

Currently, **subsitutions.cpp** only supports IR that has been lifted by McSema. When attempting to replace a function call, anywhere that the call's return value is referenced will be replaced with a `badref`. These cases need to be accounted for and the `badref` must be replaced with the return value of the new function call. In IR produced by McSema, I have currently only identified two patterns, so I can easily check for these two patterns and replace the badref as necessary. 

In other applications, however, the function return value could be referenced any number of times, and there are simply too many cases to account for. It may be possible to search for every badref within a function and replace it, though given that the purpose of this software is to instrument IR produced by McSema, I do not believe that to be necessary at the moment.

## Build Script

Currently, the build script has no way of accounting for different LLVM versions, and must be manually modified if the user wants to use a different version of LLVM.

Ideally, we could use a build system like [GNU Make](https://www.gnu.org/software/make/) to allow for easy modification of compilation parameters, though given the simplicity of the current compilation process, I do not believe it be a priority.

## Function Maps

In their current form, function maps are quite simple, and do not cover all cases of equivalent function mapping. There may be cases where a single function could map onto two or more functions, making the current function maps insufficient.

## Scalability of Config Files

Currently, config files are read manually using the read_map_config file. Reading config files manually limits the scalability of function maps. If we wanted to add additional functionality to function maps, a great deal of overhead would be incurred by needing to rewrite the read_map_config_file.

Ideally, we would use some c++ class serialization library to trivialize the process of reading from and writing to a config file.

## Program Arguments

**substitutions.cpp** currently only takes IR file names as arguments. Ideally, we would provide wider functionality through program arguments. For example, it would be nice if users could manually specify a config file via a function argument.
