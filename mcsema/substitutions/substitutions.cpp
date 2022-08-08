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
    const char* og_func;        // The name of the original function
    const char* new_func;       // The name of the function that the original function will be replaced with
    std::map<int, int> args;    // A map of the arguments by index in the original function to the new function
};


/* Reads the function mapping config from a file
 *
 * @param filepath The config file to read from
 *
 * @return A vector containing the function maps
 */
std::vector<func_map> read_map_config(const char* filepath){
    // setup
    std::ifstream file;
    file.open(filepath);
    std::string line;
    std::vector<func_map> maps;
    
    if (file.is_open()) { while (getline(file, line)) {
        func_map* map = new func_map;

        // Skip comments
        if (line.substr(0,1) == "#") {
            continue;
        }

        // Evaluate the line 
        int start = 0;
        int count = 0;
        int end = line.find(",");
        char* og_str;
        char* new_str;
        std::string token;

        while (end != -1) {
            token = line.substr(start, end-start);
            start = end + 1;
            end = line.find(",", start);


            switch(count) {
                case 0:
                og_str = (char *)malloc(sizeof(char)*strlen(token.c_str()));
                strcpy(og_str,token.c_str());
                map->og_func = og_str;

                case 1:
                new_str = (char *)malloc(sizeof(char)*strlen(token.c_str()));
                strcpy(new_str,token.c_str());
                map->new_func = new_str;
            }

            count += 1;

        }
        token = line.substr(start, end-start);
        printf("%d:%s\n", count, token.c_str());

        // Read the function argument map
        start = 0;
        count = 0;
        end = token.find("|");
        std::string token2;
        //while (end != -1) {
        //    token = 
        //}

    } }
    file.close();

    return maps;
}


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
    std::vector<Type*> args = {};
    auto function_type = FunctionType::get(builder.getInt64Ty(), args, true);

    mod->getOrInsertFunction(func_name, function_type);
    printf("Created declaration for external function %s\n", func_name);
}


/* Subititutes one function for another within a module
 *
 * @param map Map of the old function onto the new function
 *
 * @param mod The module
 *
 * @param builder The IRBuilder
 */
void substitute(func_map map, const std::unique_ptr<Module>& mod, IRBuilder<>& builder)
{
    Function* function_call = mod->getFunction(map.og_func);
    if (!function_call) {
        fprintf(stderr, "Failed to find function call %s\n", map.og_func);
        exit(1);
    }

    Function* new_func_inst = mod->getFunction(map.new_func);
    if (!new_func_inst) {
        fprintf(stderr, "Failed to find function call %s\n", map.new_func);
        exit(1);
    }
    
    // Loop through every instance where the og function is used
    for (const auto& user : function_call->users()){
        // Check to make sure the function reference is a call instruction
        if (!llvm::isa<CallInst>(user)){
            continue;
        }
        printf("Identified a call to %s with %d operands.\n", map.og_func, user->getNumOperands());

        // Create a list of arguments for the new functions using the provided mapping
        std::vector<llvm::Value*> new_func_args(map.args.size());
        for (const auto &item : map.args) {
            new_func_args[item.second] = user->getOperand(item.first);
        }

        // Create the call instruction for the new function
        const auto call_instruction = llvm::cast<CallInst>(user);
        builder.SetInsertPoint(call_instruction->getNextNode());
        builder.CreateCall(new_func_inst, new_func_args);

        // Delete old function call
        call_instruction->eraseFromParent();
    }
}


int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "Expected an argument - IR file name\n");
        exit(1);
    }

    // Setup irbuilder and module
    static LLVMContext Context;
	IRBuilder<> builder(Context);
    SMDiagnostic Err;
    std::unique_ptr<Module> Mod = parseIRFile(argv[1], Err, Context);

    if (!Mod) {
        Err.print(argv[0], errs());
        return 1;
    }

    // TODO Read function mappings from config files
    
    //read_map_config("input.conf");

    func_map printf;
    printf.og_func = "__printf_chk";
    printf.new_func = "printf";
    printf.args[1] = 0;
    printf.args[2] = 1;
    printf.args[3] = 2;
    printf.args[4] = 3;
    printf.args[5] = 4;
    printf.args[6] = 5;
    printf.args[7] = 6;

    func_map fprintf;
    fprintf.og_func = "__fprintf_chk";
    fprintf.new_func = "fprintf";
    fprintf.args[1] = 0;
    fprintf.args[2] = 1;
    fprintf.args[3] = 2;
    fprintf.args[4] = 3;
    fprintf.args[5] = 4;
    fprintf.args[6] = 5;
    fprintf.args[7] = 6;

    std::vector<func_map> func_map_list;
    func_map_list.push_back(printf);
    func_map_list.push_back(fprintf);

    // Iterate through the list of function mappings
    for (func_map map : func_map_list) {
	    create_declaration(Mod, builder, map.new_func);
        substitute(map, Mod, builder);
        dump("output.ll", Mod);
    }

}
