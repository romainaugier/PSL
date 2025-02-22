/* SPDX-License-Identifier: BSD-3-Clause */
/* Copyright (c) 2025 - Present Romain Augier */
/* All rights reserved. */

#pragma once
#if !defined(__PSL_LEXER)
#define __PSL_LEXER

#include "psl/psl.h"
#include "libromano/vector.h"

PSL_CPP_ENTER

typedef enum {
    PSL_TokenType_Eof = 0,
    PSL_TokenType_Error,
    PSL_TokenType_Identifier,
    PSL_TokenType_Literal,
    PSL_TokenType_Keyword,
    PSL_TokenType_Operator,
    PSL_TokenType_Assign,
    PSL_TokenType_LParen,
    PSL_TokenType_RParen,
    PSL_TokenType_LBrace,
    PSL_TokenType_RBrace,
    PSL_TokenType_Comma,
    PSL_TokenType_Semicolon,
    PSL_TokenType_Count
} PSL_TokenType;

typedef enum {
    PSL_KeywordType_f32,
    PSL_KeywordType_f64,
    PSL_KeywordType_Main,
    PSL_KeywordType_Export,
    PSL_KeywordType_Return,
    PSL_KeywordType_Count,
} PSL_KeywordType;

typedef struct {
    PSL_TokenType type;
    char* start;
    uint32_t length;
    uint32_t line;
    uint32_t subtype;
} PSL_Token;

typedef struct {
    char* start;
    char* current;
    uint32_t line;
    char* error;
} PSL_Lexer;

/* Initializes the lexer keyword lookup table */
PSL_API void psl_lexer_init_keywords_table();

PSL_API uint32_t* psl_lexer_find_keyword(char* word, uint32_t word_len);

/* Destroys the lexer keyword lookup table */
PSL_API void psl_lexer_destroy_keywords_table();

/* Initializes the lexer */
PSL_API void psl_lexer_init(PSL_Lexer* lexer, const char* source);

/* Start lexing and return true on success */
PSL_API bool psl_lexer_lex(PSL_Lexer* lexer, Vector* tokens);

/* Returns a pointer to the current error message, NULL if no error happened */ 
PSL_FORCE_INLINE char* psl_lexer_get_error(PSL_Lexer* lexer)
{
    return lexer->error;
}

/* Prints the tokens in order to stdout */
PSL_API void psl_lexer_print_tokens(Vector* tokens);

PSL_CPP_END

#endif /* !defined(__PSL_LEXER) */
