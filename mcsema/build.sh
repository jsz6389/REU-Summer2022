#!/bin/bash
# https://stackoverflow.com/questions/54170006/undefined-reference-to-llvmenableabibreakingchecks
clang++ -c $(llvm-config --cxxflags) substitutions.cpp -o substitutions.o
clang++ substitutions.o $(llvm-config --ldflags --libs) -lpthread -lncurses
rm substitutions.o
