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
    std::vector<Type*> args = { builder.getInt8Ty()->getPointerTo(), builder.getInt64Ty() };
    auto function_type = FunctionType::get(builder.getInt64Ty(), args, false);

    mod->getOrInsertFunction(func_name, function_type);
    printf("Created declaration for external function %s\n", func_name);
}


/* Locates where a function is called within a module
 *
 * @param func_name The name of the function to find
 *
 * @param mod The module
 */
void find_call(const char* func_name, const std::unique_ptr<Module>& mod)
{
    const auto function_call = mod->getFunction(func_name);
    
    for (const auto& user : function_call->users()){
        if (!llvm::isa<CallInst>(user)){
            continue;
        }
        printf("Identified a call to %s\n", func_name);
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

    std::string func_name = "printf";
	create_declaration(Mod, builder, "printf");
    find_call("__printf_chk", Mod);
    dump("output.ll", Mod);

}
