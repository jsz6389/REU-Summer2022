#!/bin/python
#
# Takes a set of LLVM IR files and carries out substitutions on them based on a config file
#

from llvmlite import binding,ir


def readfile(filename):
    file = open(filename, 'r')
    filetext = file.read()
    file.close()

    return filetext


def main():
    llvmir = readfile("ll/cat.ll")
    llvmmodule = binding.parse_assembly(llvmir,context=None)

    builder = ir.IRBuilder()

if __name__=="__main__":
    main()


