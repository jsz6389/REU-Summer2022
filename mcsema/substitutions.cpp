/* SPDX-License-Identifier: MIT
 *
 * substitutions.cpp
 *
 * Substitute equivalent functions in an LLVM IR file
 *
 * Copyright (C) 2022 Jakob Zielinski <jsz6389@rit.edu>
 */
#include <iostream>
#include <fstream>
#include <utility>
#include <map>

#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>

using llvm::LLVMContext;
using llvm::SMDiagnostic;
using llvm::Module;
using llvm::IRBuilder;
using llvm::CallInst;
using llvm::StoreInst;
using llvm::BranchInst;
using llvm::Instruction;
using llvm::FunctionType;
using llvm::ConstantInt;
using llvm::Type;
using llvm::ArrayRef;
using llvm::ConstantDataArray;
using namespace llvm;


class func_map {                // A mapping of one function to another
    public:                     //
    const char* og_func;              // The name of the original function
    const char* new_func;             // The name of the function that the original function will be replaced with
    std::map<int, int> args;    // A map of the arguments by index in the original function to the new function
};


/* Dumps the LLVM IR from a module to a file
 * 
 * @param path The path to which the LLVM IR will be dumped
 *
 * @param mod The module from which the LLVM IR will be dumped
 */
void dump(const char* path, std::unique_ptr<Module>& mod)
{
    std::string ir;
    llvm::raw_string_ostream stream(ir);
    mod->print(stream, nullptr);

    std::ofstream output(path);
    output << ir;
    output.close();
}


/* Creates a new function declaration with the LLVM module
 *
 * @param mod The module
 *
 * @param build The IR builder
 *
 * @param func_name The name of the function to be declared
 *
 * TODO Create semantics for the modifications of arguments in declared function
 */
void create_declaration(const std::unique_ptr<Module>& mod, IRBuilder<>& builder, const char* func_name)
{
    //std::vector<Type*> args = { builder.getInt8Ty()->getPointerTo(), builder.getInt64Ty() };
    std::vector<Type*> args = {};
    auto function_type = FunctionType::get(builder.getInt64Ty(), args, true);

    mod->getOrInsertFunction(func_name, function_type);
    printf("Created declaration for external function %s\n", func_name);
}


/* Subititutes one function for another within a module
 *
 * @param func_name The name of the function to find
 *
 * @param mod The module
 */
void substitute(func_map map, const std::unique_ptr<Module>& mod, IRBuilder<>& builder)
{
    Function* function_call = mod->getFunction(map.og_func);
    Function* new_func_inst = mod->getFunction(map.new_func);
    
    for (const auto& user : function_call->users()){
        if (!llvm::isa<CallInst>(user)){
            continue;
        }
        printf("Identified a call to %s\n", map.og_func);
        /* TODO Substitute function call
         * https://llvm.org/doxygen/classllvm_1_1Function.html
         * https://llvm.org/doxygen/namespacellvm.html
         *
         */
        printf("%d operands in %s\n", user->getNumOperands(), map.og_func);

        std::vector<llvm::Value*> new_func_args(map.args.size());
        for (const auto &item : map.args) {
            new_func_args[item.second] = user->getOperand(item.first);
        }

        const auto call_instruction = llvm::cast<CallInst>(user);
        builder.SetInsertPoint(call_instruction->getNextNode());
        llvm::Value *hello = builder.CreateGlobalStringPtr("Hello world!\n");
        builder.CreateCall(new_func_inst, new_func_args);
    }
}


int main(int argc, char** argv)
{
    if (argc < 2) {
        errs() << "Expected an argument - IR file name\n";
        exit(1);
    }

    static LLVMContext Context;
	IRBuilder<> builder(Context);
    SMDiagnostic Err;
    std::unique_ptr<Module> Mod = parseIRFile(argv[1], Err, Context);

    if (!Mod) {
        Err.print(argv[0], errs());
        return 1;
    }

    func_map myMap;
    myMap.og_func = "__printf_chk";
    myMap.new_func = "printf";
    myMap.args[1] = 0;
    myMap.args[2] = 1;
    myMap.args[3] = 2;
    myMap.args[4] = 3;
    myMap.args[5] = 4;
    myMap.args[6] = 5;
    myMap.args[7] = 6;

	create_declaration(Mod, builder, "printf");
    substitute(myMap, Mod, builder);
    dump("output.ll", Mod);

}
