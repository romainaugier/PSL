/* SPDX-License-Identifier: BSD-3-Clause */
/* Copyright (c) 2025 - Present Romain Augier */
/* All rights reserved. */

#include "psl/lexer.h"

#include "libromano/hashmap.h"
#include "libromano/logger.h"

#include <string.h>
#include <stdlib.h>

static HashMap* _keywords_table = NULL;

void psl_lexer_init_keywords_table()
{
    if(_keywords_table != NULL)
    {
        return;
    }

    _keywords_table = hashmap_new(PSL_KeywordType_Count);

    uint32_t value = (uint32_t)PSL_KeywordType_f32;
    hashmap_insert(_keywords_table, "f32", 3, &value, sizeof(uint32_t));
    value = (uint32_t)PSL_KeywordType_f64;
    hashmap_insert(_keywords_table, "f64", 3, &value, sizeof(uint32_t));
    value = (uint32_t)PSL_KeywordType_Main;
    hashmap_insert(_keywords_table, "main", 4, &value, sizeof(uint32_t));
    value = (uint32_t)PSL_KeywordType_Export;
    hashmap_insert(_keywords_table, "export", 6, &value, sizeof(uint32_t));
    value = (uint32_t)PSL_KeywordType_Return;
    hashmap_insert(_keywords_table, "return", 6, &value, sizeof(uint32_t));
}

uint32_t* psl_lexer_find_keyword(char* word, uint32_t word_len)
{
    if(_keywords_table == NULL)
    {
        logger_log_error("PSL lexer keywords table has not been initialized");
        return NULL;
    }

    return hashmap_get(_keywords_table, (const void*)word, word_len, NULL);
}

void psl_lexer_destroy_keywords_table()
{
    if(_keywords_table != NULL)
    {
        hashmap_free(_keywords_table);
        _keywords_table = NULL;
    }
}

PSL_FORCE_INLINE bool is_digit(unsigned int c)
{
    return (c - 48) < 10;
}

PSL_FORCE_INLINE bool is_letter(unsigned int c)
{
    return ((c & ~0x20) - 65) < 26;
}

PSL_FORCE_INLINE bool is_alnum(unsigned int c)
{
    return is_digit(c) | is_letter(c);
}

PSL_FORCE_INLINE bool is_operator(unsigned int c)
{
    return c == '+' |
           c == '-' |
           c == '*' |
           c == '/' |
           c == '%';
}

PSL_FORCE_INLINE bool is_dot(unsigned int c)
{
    return c == '.';
}

PSL_FORCE_INLINE bool is_paren(unsigned int c)
{
    return c == '(' | c == ')';
}

PSL_FORCE_INLINE bool lexer_is_at_end(PSL_Lexer* lexer) 
{
    return *lexer->current == '\0';
}

PSL_FORCE_INLINE char lexer_advance(PSL_Lexer* lexer) 
{
    lexer->current++;
    return lexer->current[-1];
}

PSL_FORCE_INLINE char lexer_peek(PSL_Lexer* lexer) 
{
    return *lexer->current;
}

PSL_FORCE_INLINE char lexer_peek_next(PSL_Lexer* lexer) 
{
    if(lexer_is_at_end(lexer)) 
    {
        return '\0';
    }

    return lexer->current[1];
}

bool lexer_match_char(PSL_Lexer* lexer, char expected) 
{
    if(lexer_is_at_end(lexer))
    {
        return false;
    }

    if(*lexer->current != expected)
    {
        return false;
    }

    lexer->current++;

    return true;
}

PSL_Token lexer_make_token(PSL_Lexer* lexer, PSL_TokenType type, uint32_t subtype) 
{
    PSL_Token token;
    token.type = type;
    token.start = lexer->start;
    token.length = (uint32_t)(lexer->current - lexer->start);
    token.line = lexer->line;
    token.subtype = subtype;
    return token;
}

PSL_Token lexer_make_error_token(PSL_Lexer* lexer, char* error_message)
{
    lexer->error = error_message;
    return lexer_make_token(lexer, PSL_TokenType_Error, 0);
}

void lexer_skip_whitespace(PSL_Lexer* lexer) 
{
    while(true)
    {
        char c = lexer_peek(lexer);

        switch(c) 
        {
            case ' ':
            case '\r':
            case '\t':
                lexer_advance(lexer);
                break;
            case '\n':
                lexer->line++;
                lexer_advance(lexer);
                break;
            case '/':
                if(lexer_peek_next(lexer) == '/') 
                {
                    while(lexer_peek(lexer) != '\n' && !lexer_is_at_end(lexer)) 
                    {
                        lexer_advance(lexer);
                    }
                } 
                else 
                {
                    return;
                }

                break;
            default:
                return;
        }
    }
}

bool lexer_number(PSL_Lexer* lexer, PSL_Token* out) 
{
    while(is_digit(lexer_peek(lexer))) 
    {
        lexer_advance(lexer);
    }

    if(lexer_peek(lexer) == '.' && is_digit(lexer_peek_next(lexer))) 
    {
        lexer_advance(lexer);

        while(is_digit(lexer_peek(lexer))) 
        {
            lexer_advance(lexer);
        }
    }

    *out = lexer_make_token(lexer, PSL_TokenType_Literal, 0);

    return true;
}

PSL_FORCE_INLINE PSL_TokenType lexer_check_keyword(const char* start, uint32_t length) 
{
    return psl_lexer_find_keyword((char*)start, length) == NULL ? PSL_TokenType_Identifier : PSL_TokenType_Keyword;
}

PSL_Token lexer_identifier(PSL_Lexer* lexer) 
{
    while(is_alnum(lexer_peek(lexer)) || lexer_peek(lexer) == '_') 
    {
        lexer_advance(lexer);
    }

    uint32_t* keyword = psl_lexer_find_keyword((char*)lexer->start, (uint32_t)(lexer->current - lexer->start));

    return lexer_make_token(lexer, 
                            keyword == NULL ? PSL_TokenType_Identifier : PSL_TokenType_Keyword,
                            keyword == NULL ? 0 : *keyword);
}

PSL_Token lexer_scan_token(PSL_Lexer* lexer) 
{
    lexer_skip_whitespace(lexer);
    lexer->start = lexer->current;

    if(lexer_is_at_end(lexer)) 
    {
        return lexer_make_token(lexer, PSL_TokenType_Eof, 0);
    }

    char c = lexer_advance(lexer);

    if(is_letter(c)) 
    {
        return lexer_identifier(lexer);
    }

    if(is_digit(c)) 
    {
        PSL_Token out;

        if(!lexer_number(lexer, &out))
        {
            return lexer_make_error_token(lexer, "Malformed number literal");
        }

        return out;
    }

    switch(c) 
    {
        case '(': return lexer_make_token(lexer, PSL_TokenType_LParen, 0);
        case ')': return lexer_make_token(lexer, PSL_TokenType_RParen, 0);
        case '{': return lexer_make_token(lexer, PSL_TokenType_LBrace, 0);
        case '}': return lexer_make_token(lexer, PSL_TokenType_RBrace, 0);
        case ';': return lexer_make_token(lexer, PSL_TokenType_Semicolon, 0);
        case ',': return lexer_make_token(lexer, PSL_TokenType_Comma, 0);
        case '-':
        case '+':
        case '/':
        case '*': return lexer_make_token(lexer, PSL_TokenType_Operator, 0);
        case '=': return lexer_make_token(lexer, PSL_TokenType_Assign, 0);
    }

    return lexer_make_error_token(lexer, "Unexpected character");
}

void psl_lexer_init(PSL_Lexer* lexer, const char* source)
{
    lexer->start = (char*)source;
    lexer->current = (char*)source;
    lexer->line = 1;
    lexer->error = NULL;
}

bool psl_lexer_lex(PSL_Lexer* lexer, Vector* tokens)
{
    while(true)
    {
        PSL_Token token = lexer_scan_token(lexer);
        vector_push_back(tokens, &token);

        switch(token.type)
        {
            case PSL_TokenType_Eof:
                return true;
            case PSL_TokenType_Error:
                return false;
            default:
                break;
        }
    }
}

const char* psl_token_type_to_string(PSL_TokenType type)
{
    switch(type)
    {
        case PSL_TokenType_Eof:
            return "EOF";
        case PSL_TokenType_Error:
            return "ERROR";
        case PSL_TokenType_Identifier:
            return "IDENTIFIER";
        case PSL_TokenType_Literal:
            return "LITERAL";
        case PSL_TokenType_Keyword:
            return "KEYWORD";
        case PSL_TokenType_Operator:
            return "OPERATOR";
        case PSL_TokenType_Assign:
            return "ASSIGN";
        case PSL_TokenType_LParen:
            return "LPAREN";
        case PSL_TokenType_RParen:
            return "RPAREN";
        case PSL_TokenType_LBrace:
            return "LBRACE";
        case PSL_TokenType_RBrace:
            return "RBRACE";
        case PSL_TokenType_Comma:
            return "COMMA";
        case PSL_TokenType_Semicolon:
            return "SEMICOLON";
        default:
            return "UNKNOWN";
    }
}

void psl_lexer_print_tokens(Vector* tokens)
{
    for(uint32_t i = 0; i < vector_size(tokens); i++)
    {
        PSL_Token* token = (PSL_Token*)vector_at(tokens, i);

        printf("%s %.*s\n", 
               psl_token_type_to_string(token->type),
               token->length,
               token->start);
    }
}