/* SPDX-License-Identifier: BSD-3-Clause */
/* Copyright (c) 2025 - Present Romain Augier */
/* All rights reserved. */

#pragma once

#if !defined(__PSL_AST)
#define __PSL_AST

#include "psl/parser.h"
#include "psl/arena.h"

#include "libromano/vector.h"
#include "libromano/hashmap.h"

PSL_CPP_ENTER

typedef enum {
    PSL_ASTNodeType_PSL_ASTSource,
    PSL_ASTNodeType_PSL_ASTFunction,
    PSL_ASTNodeType_PSL_ASTParameter,
    PSL_ASTNodeType_PSL_ASTBlock,
    PSL_ASTNodeType_PSL_ASTReturn,
    PSL_ASTNodeType_PSL_ASTAssignment,
    PSL_ASTNodeType_PSL_ASTBinOP,
    PSL_ASTNodeType_PSL_ASTUnOP,
    PSL_ASTNodeType_PSL_ASTFunctionCall,
    PSL_ASTNodeType_PSL_ASTVariable,
    PSL_ASTNodeType_PSL_ASTLiteral,
} PSL_ASTNodeType;

typedef struct {
    PSL_ASTNodeType type;
} PSL_ASTNode;

typedef struct {
    PSL_ASTNode base;
    PSL_ASTNode** functions;
    uint32_t num_functions;
} PSL_ASTSource;

typedef struct {
    PSL_ASTNode base;
    char* name;
    uint32_t name_length;
    PSL_ASTNode** parameters;
    uint32_t num_parameters;
    PSL_ASTNode* body;
    bool is_entry_point;
} PSL_ASTFunction;

typedef struct {
    PSL_ASTNode base;
    char* name;
    uint32_t name_length;
    bool exportable;
} PSL_ASTParameter;

typedef struct {
    PSL_ASTNode base;
    PSL_ASTNode** statements;
    uint32_t num_statements;
} PSL_ASTBlock;

typedef struct {
    PSL_ASTNode base;
    PSL_ASTNode* statement;
} PSL_ASTReturn;

typedef struct {
    PSL_ASTNode base;
    PSL_ASTNode* lvalue;
    PSL_ASTNode* rvalue;
} PSL_ASTAssignment;

typedef enum {
    PSL_ASTBinOPType_Add,
    PSL_ASTBinOPType_Sub,
    PSL_ASTBinOPType_Mul,
    PSL_ASTBinOPType_Div,
} PSL_ASTBinOPType;

typedef struct {
    PSL_ASTNode base;
    PSL_ASTBinOPType op;
    PSL_ASTNode* left;
    PSL_ASTNode* right;
} PSL_ASTBinOP;

typedef enum {
    PSL_ASTUnOPType_Neg,
} PSL_ASTUnOPType;

typedef struct {
    PSL_ASTNode base;
    PSL_ASTUnOPType op;
    PSL_ASTNode* operand;
} PSL_ASTUnOP;

typedef struct {
    PSL_ASTNode base;
    char* name;
    uint32_t name_length;
    PSL_ASTNode** arguments;
    uint32_t num_arguments;
} PSL_ASTFunctionCall;

typedef struct {
    PSL_ASTNode base;
    float value;
} PSL_ASTLiteral;

typedef struct {
    PSL_ASTNode base;
    char* name;
    uint32_t name_length;
} PSL_ASTVariable;

#define PSL_AST_CAST(__type__, __node__) ((__type__*)((__node__)->type == PSL_ASTNodeType_##__type__ ? __node__ : NULL))

#define PSL_MAX_FUNCTIONS_PER_SOURCE 32

typedef struct 
{
    Arena nodes_data;
    PSL_ASTNode* root;
    char* error;
} PSL_AST;

PSL_API PSL_AST* psl_ast_new();

PSL_API PSL_ASTNode* psl_ast_new_source(PSL_AST* ast,
                                        PSL_ASTNode** functions,
                                        uint32_t num_functions);

PSL_API PSL_ASTNode* psl_ast_new_function(PSL_AST* ast,
                                          char* name,
                                          uint32_t name_length,
                                          PSL_ASTNode** parameters,
                                          uint32_t num_parameters,
                                          PSL_ASTNode* body,
                                          bool is_entry_point);

PSL_API PSL_ASTNode* psl_ast_new_parameter(PSL_AST* ast,
                                           char* name,
                                           uint32_t name_length,
                                           bool exportable);

PSL_API PSL_ASTNode* psl_ast_new_block(PSL_AST* ast,
                                       PSL_ASTNode** statements,
                                       uint32_t num_statements);

PSL_API PSL_ASTNode* psl_ast_new_return(PSL_AST* ast, 
                                        PSL_ASTNode* statement);

PSL_API PSL_ASTNode* psl_ast_new_assignment(PSL_AST* ast,
                                            PSL_ASTNode* lvalue,
                                            PSL_ASTNode* rvalue);

PSL_API PSL_ASTNode* psl_ast_new_binop(PSL_AST* ast,
                                       PSL_ASTBinOPType op,
                                       PSL_ASTNode* left,
                                       PSL_ASTNode* right);

PSL_API PSL_ASTNode* psl_ast_new_unop(PSL_AST* ast,
                                      PSL_ASTUnOPType op,
                                      PSL_ASTNode* operand);

PSL_API PSL_ASTNode* psl_ast_new_function_call(PSL_AST* ast,
                                               char* name,
                                               uint32_t name_length,
                                               PSL_ASTNode** arguments,
                                               uint32_t num_arguments);

PSL_API PSL_ASTNode* psl_ast_new_literal(PSL_AST* ast,
                                         float value);

PSL_API PSL_ASTNode* psl_ast_new_variable(PSL_AST* ast,
                                          char* name,
                                          uint32_t name_length);

PSL_API bool psl_ast_from_tokens(PSL_AST* ast, 
                                 Vector* tokens);

PSL_API void psl_ast_print(PSL_AST* ast); 

PSL_API void psl_ast_destroy(PSL_AST* ast);

PSL_CPP_END

#endif /* !defined(__PSL_AST) */
