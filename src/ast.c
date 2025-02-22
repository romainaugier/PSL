/* SPDX-License-Identifier: BSD-3-Clause */
/* Copyright (c) 2025 - Present Romain Augier */
/* All rights reserved. */

#include "psl/ast.h"
#include "psl/lexer.h"

#include "libromano/stack_no_alloc.h"

#include <string.h>

PSL_AST* psl_ast_new()
{
    PSL_AST* new_ast = (PSL_AST*)malloc(sizeof(PSL_AST));
    psl_arena_init(&new_ast->nodes_data, 4096);
    new_ast->root = NULL;
    new_ast->error = NULL;

    return new_ast;
}

PSL_ASTNode* psl_ast_new_source(PSL_AST* ast,
                                PSL_ASTNode** functions,
                                uint32_t num_functions)
{
    PSL_ASTSource* src = (PSL_ASTSource*)psl_arena_push(&ast->nodes_data, NULL, sizeof(PSL_ASTFunction));
    src->base.type = PSL_ASTNodeType_PSL_ASTSource;
    src->functions = (PSL_ASTNode**)malloc(num_functions * sizeof(PSL_ASTNode*));
    memcpy(src->functions, functions, num_functions * sizeof(PSL_ASTNode*));
    src->num_functions = num_functions;

    return (PSL_ASTNode*)src;
}

PSL_ASTNode* psl_ast_new_function(PSL_AST* ast,
                                  char* name,
                                  uint32_t name_length,
                                  PSL_ASTNode** parameters,
                                  uint32_t num_parameters,
                                  PSL_ASTNode* body,
                                  bool is_entry_point)
{
    PSL_ASTFunction* func = (PSL_ASTFunction*)psl_arena_push(&ast->nodes_data, NULL, sizeof(PSL_ASTFunction));
    func->base.type = PSL_ASTNodeType_PSL_ASTFunction;
    func->name = name;
    func->name_length = name_length;
    func->parameters = (PSL_ASTNode**)malloc(num_parameters * sizeof(PSL_ASTNode*));
    memcpy(func->parameters, parameters, num_parameters * sizeof(PSL_ASTNode*));
    func->num_parameters = num_parameters;
    func->body = body;
    func->is_entry_point = is_entry_point;

    return (PSL_ASTNode*)func;
}

PSL_ASTNode* psl_ast_new_parameter(PSL_AST* ast,
                                   char* name,
                                   uint32_t name_length,
                                   bool exportable)
{
    PSL_ASTParameter* param = (PSL_ASTParameter*)psl_arena_push(&ast->nodes_data, NULL, sizeof(PSL_ASTParameter));
    param->base.type = PSL_ASTNodeType_PSL_ASTParameter;
    param->name = name;
    param->name_length = name_length;
    param->exportable = exportable;

    return (PSL_ASTNode*)param;
}

PSL_ASTNode* psl_ast_new_block(PSL_AST* ast,
                               PSL_ASTNode** statements,
                               uint32_t num_statements)
{
    PSL_ASTBlock* block = (PSL_ASTBlock*)psl_arena_push(&ast->nodes_data, NULL, sizeof(PSL_ASTBlock));
    block->base.type = PSL_ASTNodeType_PSL_ASTBlock;
    block->statements = (PSL_ASTNode**)malloc(num_statements * sizeof(PSL_ASTNode*));
    memcpy(block->statements, statements, num_statements * sizeof(PSL_ASTNode*));
    block->num_statements = num_statements;

    return (PSL_ASTNode*)block;
}

PSL_ASTNode* psl_ast_new_return(PSL_AST* ast, 
                                PSL_ASTNode* statement)
{
    PSL_ASTReturn* ret = (PSL_ASTReturn*)psl_arena_push(&ast->nodes_data, NULL, sizeof(PSL_ASTReturn));
    ret->base.type = PSL_ASTNodeType_PSL_ASTReturn;
    ret->statement = statement;

    return (PSL_ASTNode*)ret;
}

PSL_ASTNode* psl_ast_new_assignment(PSL_AST* ast,
                                    PSL_ASTNode* lvalue,
                                    PSL_ASTNode* rvalue)
{
    PSL_ASTAssignment* assignment = (PSL_ASTAssignment*)psl_arena_push(&ast->nodes_data, NULL, sizeof(PSL_ASTAssignment));
    assignment->base.type = PSL_ASTNodeType_PSL_ASTAssignment;
    assignment->lvalue = lvalue;
    assignment->rvalue = rvalue;

    return (PSL_ASTNode*)assignment;
}

PSL_ASTNode* psl_ast_new_binop(PSL_AST* ast,
                               PSL_ASTBinOPType op,
                               PSL_ASTNode* left,
                               PSL_ASTNode* right)
{
    PSL_ASTBinOP* binop = (PSL_ASTBinOP*)psl_arena_push(&ast->nodes_data, NULL, sizeof(PSL_ASTBinOP));
    binop->base.type = PSL_ASTNodeType_PSL_ASTBinOP;
    binop->op = op;
    binop->left = left;
    binop->right = right;

    return (PSL_ASTNode*)binop;
}

PSL_ASTNode* psl_ast_new_unop(PSL_AST* ast,
                              PSL_ASTUnOPType op,
                              PSL_ASTNode* operand)
{
    PSL_ASTUnOP* unop = (PSL_ASTUnOP*)psl_arena_push(&ast->nodes_data, NULL, sizeof(PSL_ASTUnOP));
    unop->base.type = PSL_ASTNodeType_PSL_ASTUnOP;
    unop->op = op;
    unop->operand = operand;

    return (PSL_ASTNode*)unop;
}

PSL_ASTNode* psl_ast_new_function_call(PSL_AST* ast,
                                       char* name,
                                       uint32_t name_length,
                                       PSL_ASTNode** arguments,
                                       uint32_t num_arguments)
{
    PSL_ASTFunctionCall* funccall = (PSL_ASTFunctionCall*)psl_arena_push(&ast->nodes_data, NULL, sizeof(PSL_ASTFunctionCall));
    funccall->base.type = PSL_ASTNodeType_PSL_ASTFunctionCall;
    funccall->name = name;
    funccall->name_length = name_length;
    funccall->arguments = (PSL_ASTNode**)malloc(num_arguments * sizeof(PSL_ASTNode*));
    memcpy(funccall->arguments, arguments, num_arguments * sizeof(PSL_ASTNode*));
    funccall->num_arguments = num_arguments;

    return (PSL_ASTNode*)funccall;
}

PSL_ASTNode* psl_ast_new_literal(PSL_AST* ast,
                                 float value)
{
    PSL_ASTLiteral* lit = (PSL_ASTLiteral*)psl_arena_push(&ast->nodes_data, NULL, sizeof(PSL_ASTLiteral));
    lit->base.type = PSL_ASTNodeType_PSL_ASTLiteral;
    lit->value = value;

    return (PSL_ASTNode*)lit;
}

PSL_ASTNode* psl_ast_new_variable(PSL_AST* ast,
                                  char* name,
                                  uint32_t name_length)
{
    PSL_ASTVariable* var = (PSL_ASTVariable*)psl_arena_push(&ast->nodes_data, NULL, sizeof(PSL_ASTVariable));
    var->base.type = PSL_ASTNodeType_PSL_ASTVariable;
    var->name = name;
    var->name_length = name_length;

    return (PSL_ASTNode*)var;
}

PSL_ASTBinOPType psl_ast_token_op_to_binop(const PSL_Token* token)
{
    char op_char = *(token->start);

    switch (op_char) 
    {
        case '+':  
            return PSL_ASTBinOPType_Add;
        case '-':  
            return PSL_ASTBinOPType_Sub;
        case '*':  
            return PSL_ASTBinOPType_Mul;
        case '/':  
            return PSL_ASTBinOPType_Div;
        default:   
            return PSL_ASTBinOPType_Add;
    }
}

uint32_t psl_ast_get_binop_precedence(PSL_ASTBinOPType op)
{
    switch(op) 
    {
        case PSL_ASTBinOPType_Mul:
        case PSL_ASTBinOPType_Div:
            return 3;
        case PSL_ASTBinOPType_Add:
        case PSL_ASTBinOPType_Sub:         
            return 2;
        default:
            return 0;
    }
}

PSL_ASTNode* psl_parse_expression(PSL_AST* ast, PSL_Parser* parser);
PSL_ASTNode* psl_parse_binary_expression(PSL_AST* ast, PSL_Parser* parser, uint32_t min_prec);
PSL_ASTNode* psl_parse_primary(PSL_AST* ast, PSL_Parser* parser);
PSL_ASTNode* psl_parse_function_call(PSL_AST* ast, PSL_Parser* parser);

PSL_ASTNode* psl_parse_function_body(PSL_AST* ast, PSL_Parser* parser)
{
    if(psl_parser_current_token(parser)->type != PSL_TokenType_LBrace) 
    {
        ast->error = "Expected \"{\" at function body start";
        return NULL;
    }

    psl_parser_advance(parser);

    Vector* statements = vector_new(128, sizeof(PSL_ASTNode*));
    
    while(psl_parser_current_token(parser)->type != PSL_TokenType_RBrace) 
    {
        PSL_Token* current = psl_parser_current_token(parser);
        
        /* Return statements */
        if(current->type == PSL_TokenType_Keyword && 
           current->subtype == PSL_KeywordType_Return)
        {
            psl_parser_advance(parser); /* Consume return */

            PSL_ASTNode* expr = psl_parse_expression(ast, parser);

            if(expr == NULL)
            {
                return false;
            }

            PSL_ASTNode* ret = psl_ast_new_return(ast, expr);

            vector_push_back(statements, &ret);
        }
        /* Assignments and expressions */
        else if(current->type == PSL_TokenType_Identifier) 
        {
            PSL_ASTNode* lvalue = psl_ast_new_variable(ast, current->start, current->length);

            psl_parser_advance(parser); /* Consume identifier */
            psl_parser_advance(parser); /* Consume = */

            PSL_ASTNode* rvalue = psl_parse_expression(ast, parser);

            if(rvalue == NULL)
            {
                return false;
            }

            PSL_ASTNode* assignment = psl_ast_new_assignment(ast, lvalue, rvalue);
            
            vector_push_back(statements, &assignment);
        }
        
        if(psl_parser_current_token(parser)->type != PSL_TokenType_Semicolon) 
        {
            ast->error = "Expected ';' after statement";
            return false;
        }

        psl_parser_advance(parser);
    }
    
    psl_parser_advance(parser);

    PSL_ASTNode* block = psl_ast_new_block(ast, 
                                           (PSL_ASTNode**)vector_at(statements, 0),
                                           (uint32_t)vector_size(statements));

    vector_free(statements);

    return block;
}

PSL_ASTNode* psl_parse_expression(PSL_AST* ast, PSL_Parser* parser) 
{
    return psl_parse_binary_expression(ast, parser, 0);
}

PSL_ASTNode* psl_parse_binary_expression(PSL_AST* ast, PSL_Parser* parser, uint32_t min_prec) 
{
    PSL_ASTNode* left = psl_parse_primary(ast, parser);

    if(left == NULL) 
    {
        return NULL;
    }

    while(true) 
    {
        PSL_Token* current = psl_parser_current_token(parser);

        if(current->type != PSL_TokenType_Operator) 
        {
            break;
        }

        PSL_ASTBinOPType binop_type = psl_ast_token_op_to_binop(current);
        uint32_t binop_precedence = psl_ast_get_binop_precedence(binop_type);
        
        if(binop_precedence < min_prec) 
        {
            break;
        }

        psl_parser_advance(parser); /* Consume operator */
        
        PSL_ASTNode* right = psl_parse_binary_expression(ast, parser, binop_precedence + 1);

        if(right == NULL) 
        {
            return NULL;
        }
        
        left = psl_ast_new_binop(ast, binop_type, left, right);
    }
    
    return left;
}

PSL_ASTNode* psl_parse_function_call(PSL_AST* ast, PSL_Parser* parser)
{
    PSL_Token* name_token = psl_parser_current_token(parser);
    
    psl_parser_advance(parser); /* Consume identifier */
    psl_parser_advance(parser); /* Consume ( */

    stack_init(PSL_ASTNode*, arguments, 32);
    uint32_t num_arguments = 0;
    
    while(psl_parser_current_token(parser)->type != PSL_TokenType_RParen) 
    {
        PSL_ASTNode* arg = psl_parse_expression(ast, parser);

        if(arg == NULL) 
        {
            return NULL;
        }

        stack_push(arguments, arg);
        
        if(psl_parser_current_token(parser)->type == PSL_TokenType_Comma) 
        {
            psl_parser_advance(parser);
        }
    }

    psl_parser_advance(parser); /* Consume ) */

    return psl_ast_new_function_call(ast,
                                     name_token->start,
                                     name_token->length,
                                     arguments,
                                     num_arguments);
}

PSL_ASTNode* psl_parse_primary(PSL_AST* ast, PSL_Parser* parser) 
{
    PSL_Token* current = psl_parser_current_token(parser);
    
    /* Parenthesized expressions */
    if(current->type == PSL_TokenType_LParen) 
    {
        psl_parser_advance(parser);

        PSL_ASTNode* expr = psl_parse_expression(ast, parser);

        if(expr == NULL) 
        {
            return NULL;
        }
        
        if(psl_parser_current_token(parser)->type != PSL_TokenType_RParen) 
        {
            ast->error = "Expected closing parenthesis";
            return NULL;
        }

        psl_parser_advance(parser);

        return expr;
    }
    
    /* Handle function calls */
    if(current->type == PSL_TokenType_Identifier && 
       psl_parser_peek_check(parser, 1, PSL_TokenType_LParen)) 
    {
        return psl_parse_function_call(ast, parser);
    }
    
    /* Literals and variables */
    PSL_ASTNode* node = NULL;

    if(current->type == PSL_TokenType_Identifier) 
    {
        node = psl_ast_new_variable(ast, current->start, current->length);
    }
    else if(current->type == PSL_TokenType_Literal) 
    {
        node = psl_ast_new_literal(ast, strtof(current->start, NULL));
    }
    else 
    {
        ast->error = "Unexpected token in expression";
        return NULL;
    }
    
    psl_parser_advance(parser);

    return node;
}

bool psl_ast_from_tokens(PSL_AST* ast, Vector* tokens)
{
    PSL_Parser parser;
    psl_parser_init(&parser, tokens);

    // Validate initial token structure
    if(!psl_parser_peek_check(&parser, 0, PSL_TokenType_Keyword) ||
       !psl_parser_peek_check(&parser, 1, PSL_TokenType_Identifier) ||
       !psl_parser_peek_check(&parser, 2, PSL_TokenType_LParen))
    {
        ast->error = "Invalid function declaration structure";
        return false;
    }

    stack_init(PSL_ASTNode*, functions, 32);
    uint32_t num_functions = 0;
    
    while(!psl_parser_is_at_end(&parser))
    {
        PSL_Token* current = psl_parser_current_token(&parser);
        
        /* Function declarations */
        if(current->type == PSL_TokenType_Keyword && 
           (current->subtype == PSL_KeywordType_Main ||
            current->subtype == PSL_KeywordType_f32 || 
            current->subtype == PSL_KeywordType_f64))
        {
            psl_parser_advance(&parser);

            bool is_entry_point = current->subtype == PSL_KeywordType_Main;

            /* Function name */
            PSL_Token* name_token = psl_parser_current_token(&parser);

            if(name_token->type != PSL_TokenType_Identifier) 
            {
                ast->error = "Expected function name";
                return false;
            }

            psl_parser_advance(&parser);

            /* Parameters */
            psl_parser_advance(&parser); /* Consume ( */

            stack_init(PSL_ASTNode*, parameters, 32);
            uint32_t num_parameters = 0;
            
            while(psl_parser_current_token(&parser)->type != PSL_TokenType_RParen) 
            {
                bool export = psl_parser_current_token(&parser)->subtype == PSL_KeywordType_Export;
                
                if(export)
                {
                    psl_parser_advance(&parser);
                }

                if(psl_parser_current_token(&parser)->type != PSL_TokenType_Keyword) 
                {
                    ast->error = "Expected parameter type";
                    return false;
                }

                psl_parser_advance(&parser);

                /* Parameter name */
                PSL_Token* param_name = psl_parser_current_token(&parser);

                if(param_name->type != PSL_TokenType_Identifier) 
                {
                    ast->error = "Expected parameter name";
                    return false;
                }

                PSL_ASTNode* param = psl_ast_new_parameter(ast, param_name->start, param_name->length, export);

                stack_push(parameters, param);
                num_parameters++;
                
                psl_parser_advance(&parser);

                if(psl_parser_current_token(&parser)->type == PSL_TokenType_Comma) 
                {
                    psl_parser_advance(&parser);
                }
            }
            
            psl_parser_advance(&parser); /* Consume ) */

            /* Function body */
            PSL_ASTNode* body = psl_parse_function_body(ast, &parser);

            if(body == NULL) 
            {
                return false;
            }

            PSL_ASTNode* func = psl_ast_new_function(ast,
                                                     name_token->start,
                                                     name_token->length,
                                                     &parameters,
                                                     num_parameters,
                                                     body,
                                                     is_entry_point);

            stack_push(functions, func);
            num_functions++;
        }
        else 
        {
            ast->error = "Unexpected token in global scope";
            return false;
        }
    }

    ast->root = psl_ast_new_source(ast, &functions, num_functions);

    return true;
}

const char* psl_binop_type_to_string(PSL_ASTBinOPType op) 
{
    switch(op) 
    {
        case PSL_ASTBinOPType_Add: 
            return "+";
        case PSL_ASTBinOPType_Sub: 
            return "-";
        case PSL_ASTBinOPType_Mul: 
            return "*";
        case PSL_ASTBinOPType_Div: 
            return "/";
        default:
            return "?";
    }
}

const char* psl_unop_type_to_string(PSL_ASTUnOPType op) 
{
    switch(op) 
    {
        case PSL_ASTUnOPType_Neg:
            return "-";
        default:
            return "?";
    }
}

void print_indent(int level) 
{
    for(int i = 0; i < level; i++) 
    {
        printf("  ");
    }
}

void psl_ast_print_node(PSL_ASTNode* node, int indent) 
{
    if(node == NULL) 
    {
        return;
    }

    switch(node->type) 
    {
        case PSL_ASTNodeType_PSL_ASTSource: 
        {
            PSL_ASTSource* src = PSL_AST_CAST(PSL_ASTSource, node);
            PSL_ASSERT(src != NULL, "Wrong type casting, should be PSL_ASTSource*");
            print_indent(indent);
            printf("AST Source (%u functions):\n", src->num_functions);
            for(uint32_t i = 0; i < src->num_functions; i++) {
                psl_ast_print_node(src->functions[i], indent + 1);
            }
            break;
        }

        case PSL_ASTNodeType_PSL_ASTFunction: {
            PSL_ASTFunction* func = PSL_AST_CAST(PSL_ASTFunction, node);
            print_indent(indent);
            printf("Function %.*s (%s)%s:\n",
                   func->name_length,
                   func->name,
                   func->num_parameters > 0 ? "parameters" : "no parameters",
                   func->is_entry_point ? " [main]" : "");
            
            // Print parameters
            for(uint32_t i = 0; i < func->num_parameters; i++) {
                psl_ast_print_node(func->parameters[i], indent + 1);
            }
            
            // Print body
            psl_ast_print_node(func->body, indent + 1);
            break;
        }

        case PSL_ASTNodeType_PSL_ASTParameter: {
            PSL_ASTParameter* param = PSL_AST_CAST(PSL_ASTParameter, node);
            print_indent(indent);
            printf("Parameter %.*s%s\n",
                   param->name_length,
                   param->name,
                   param->exportable ? " (export)" : "");
            break;
        }

        case PSL_ASTNodeType_PSL_ASTBlock: {
            PSL_ASTBlock* block = PSL_AST_CAST(PSL_ASTBlock, node);
            print_indent(indent);
            printf("Block (%u statements):\n", block->num_statements);
            for(uint32_t i = 0; i < block->num_statements; i++) {
                psl_ast_print_node(block->statements[i], indent + 1);
            }
            break;
        }

        case PSL_ASTNodeType_PSL_ASTReturn: {
            PSL_ASTReturn* ret = PSL_AST_CAST(PSL_ASTReturn, node);
            print_indent(indent);
            printf("Return:\n");
            psl_ast_print_node(ret->statement, indent + 1);
            break;
        }

        case PSL_ASTNodeType_PSL_ASTAssignment: {
            PSL_ASTAssignment* assign = PSL_AST_CAST(PSL_ASTAssignment, node);
            print_indent(indent);
            printf("Assignment:\n");
            print_indent(indent + 1);
            printf("LHS:\n");
            psl_ast_print_node(assign->lvalue, indent + 2);
            print_indent(indent + 1);
            printf("RHS:\n");
            psl_ast_print_node(assign->rvalue, indent + 2);
            break;
        }

        case PSL_ASTNodeType_PSL_ASTBinOP: {
            PSL_ASTBinOP* binop = PSL_AST_CAST(PSL_ASTBinOP, node);
            print_indent(indent);
            printf("Binary Operation (%s):\n", psl_binop_type_to_string(binop->op));
            psl_ast_print_node(binop->left, indent + 1);
            psl_ast_print_node(binop->right, indent + 1);
            break;
        }

        case PSL_ASTNodeType_PSL_ASTUnOP: {
            PSL_ASTUnOP* unop = PSL_AST_CAST(PSL_ASTUnOP, node);
            print_indent(indent);
            printf("Unary Operation (%s):\n", psl_unop_type_to_string(unop->op));
            psl_ast_print_node(unop->operand, indent + 1);
            break;
        }

        case PSL_ASTNodeType_PSL_ASTFunctionCall: {
            PSL_ASTFunctionCall* call = PSL_AST_CAST(PSL_ASTFunctionCall, node);
            print_indent(indent);
            printf("Call %.*s (%u arguments):\n", call->name_length, call->name, call->num_arguments);
            for(uint32_t i = 0; i < call->num_arguments; i++) {
                psl_ast_print_node(call->arguments[i], indent + 1);
            }
            break;
        }

        case PSL_ASTNodeType_PSL_ASTVariable: {
            PSL_ASTVariable* var = PSL_AST_CAST(PSL_ASTVariable, node);
            print_indent(indent);
            printf("Variable: %.*s\n", var->name_length, var->name);
            break;
        }

        case PSL_ASTNodeType_PSL_ASTLiteral: {
            PSL_ASTLiteral* lit = PSL_AST_CAST(PSL_ASTLiteral, node);
            print_indent(indent);
            printf("Literal: %f\n", lit->value);
            break;
        }

        default: {
            print_indent(indent);
            printf("Unknown node type: %d\n", node->type);
            break;
        }
    }
}

void psl_ast_print(PSL_AST* ast) 
{
    psl_ast_print_node(ast->root, 0);
}

void psl_free_malloced_nodes(PSL_ASTNode* root)
{
    stack_init(PSL_ASTNode*, nodes_to_visit, 128);

    stack_push(nodes_to_visit, root);

    while(!stack_is_empty(nodes_to_visit))
    {
        PSL_ASTNode* current = stack_pop(nodes_to_visit);

        switch(current->type)
        {
            case PSL_ASTNodeType_PSL_ASTSource:
            {
                PSL_ASTSource* src = PSL_AST_CAST(PSL_ASTSource, current);
                PSL_ASSERT(src != NULL, "Wrong type casting, should be PSL_ASTSource*");

                for(uint32_t i = 0; i < src->num_functions; i++)
                {
                    stack_push(nodes_to_visit, src->functions[i]);
                }

                free(src->functions);

                break;
            }
            case PSL_ASTNodeType_PSL_ASTFunction:
            {
                PSL_ASTFunction* func = PSL_AST_CAST(PSL_ASTFunction, current);
                PSL_ASSERT(func != NULL, "Wrong type casting, should be PSL_ASTFunction*");

                stack_push(nodes_to_visit, func->body);

                for(uint32_t i = 0; i < func->num_parameters; i++)
                {
                    stack_push(nodes_to_visit, func->parameters[i]);
                }

                free(func->parameters);

                break;
            }
            case PSL_ASTNodeType_PSL_ASTBlock:
            {
                PSL_ASTBlock* block = PSL_AST_CAST(PSL_ASTBlock, current);
                PSL_ASSERT(block != NULL, "Wrong type casting, should be PSL_ASTBlock*");

                for(uint32_t i = 0; i < block->num_statements; i++)
                {
                    stack_push(nodes_to_visit, block->statements[i]);
                }

                free(block->statements);

                break;
            }
            case PSL_ASTNodeType_PSL_ASTReturn:
            {
                PSL_ASTReturn* ret = PSL_AST_CAST(PSL_ASTReturn, current);
                PSL_ASSERT(ret != NULL, "Wrong type casting, should be PSL_ASTReturn*");

                stack_push(nodes_to_visit, (PSL_ASTNode*)ret->statement);

                break;
            }
            case PSL_ASTNodeType_PSL_ASTAssignment:
            {
                PSL_ASTAssignment* assignment = PSL_AST_CAST(PSL_ASTAssignment, current);
                PSL_ASSERT(assignment != NULL, "Wrong type casting, should be PSL_ASTAssignment*");

                stack_push(nodes_to_visit, (PSL_ASTNode*)assignment->lvalue);
                stack_push(nodes_to_visit, (PSL_ASTNode*)assignment->rvalue);

                break;
            }
            case PSL_ASTNodeType_PSL_ASTBinOP:
            {
                PSL_ASTBinOP* binop = PSL_AST_CAST(PSL_ASTBinOP, current);
                PSL_ASSERT(binop != NULL, "Wrong type casting, should be PSL_ASTBinOP*");

                stack_push(nodes_to_visit, (PSL_ASTNode*)binop->left);
                stack_push(nodes_to_visit, (PSL_ASTNode*)binop->right);

                break;
            }
            case PSL_ASTNodeType_PSL_ASTUnOP:
            {
                PSL_ASTUnOP* unop = PSL_AST_CAST(PSL_ASTUnOP, current);
                PSL_ASSERT(unop != NULL, "Wrong type casting, should be PSL_ASTUnOP*");

                stack_push(nodes_to_visit, (PSL_ASTNode*)unop->operand);

                break;
            }
            case PSL_ASTNodeType_PSL_ASTFunctionCall:
            {
                PSL_ASTFunctionCall* funccall = PSL_AST_CAST(PSL_ASTFunctionCall, current);
                PSL_ASSERT(funccall != NULL, "Wrong type casting, should be PSL_ASTFunctionCall*");

                for(uint32_t i = 0; i < funccall->num_arguments; i++)
                {
                    stack_push(nodes_to_visit, funccall->arguments[i]);
                }

                free(funccall->arguments);

                break;
            }
            case PSL_ASTNodeType_PSL_ASTVariable:
            case PSL_ASTNodeType_PSL_ASTLiteral:
            case PSL_ASTNodeType_PSL_ASTParameter:
                break;
            default: 
                break;
        }
    }
}

void psl_ast_destroy(PSL_AST* ast)
{
    if(ast != NULL)
    {
        psl_free_malloced_nodes(ast->root);

        psl_arena_destroy(&ast->nodes_data);

        free(ast);
    }
}