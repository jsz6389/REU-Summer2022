#!/bin/bash
clang++ -c $(llvm-config --cxxflags) substitutions.cpp -o substitutions.o
clang++ substitutions.o $(llvm-config --ldflags --libs) -lpthread -lncurses
rm substitutions.o
