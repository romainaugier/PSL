/* SPDX-License-Identifier: BSD-3-Clause */
/* Copyright (c) 2025 - Present Romain Augier */
/* All rights reserved. */

#pragma once

#if !defined(__PSL_PARSER)
#define __PSL_PARSER

#include "psl/lexer.h"

#include "libromano/vector.h"

PSL_CPP_ENTER

typedef struct {
    Vector* tokens;
    uint32_t current_token;
} PSL_Parser;

PSL_FORCE_INLINE void psl_parser_init(PSL_Parser* parser, Vector* tokens)
{
    parser->tokens = tokens;
    parser->current_token = 0;
}

PSL_FORCE_INLINE bool psl_parser_is_at_end(PSL_Parser* parser)
{
    return parser->current_token == (vector_size(parser->tokens) - 1);
}

PSL_FORCE_INLINE void psl_parser_advance(PSL_Parser* parser)
{
    if(psl_parser_is_at_end(parser))
    {
        return;
    }

    parser->current_token++;
}

PSL_FORCE_INLINE PSL_Token* psl_parser_current_token(PSL_Parser* parser)
{
    return (PSL_Token*)vector_at(parser->tokens, parser->current_token);
}

PSL_FORCE_INLINE PSL_Token* psl_parser_peek(PSL_Parser* parser, uint32_t offset)
{
    if(parser->current_token + offset >= (vector_size(parser->tokens) - 1))
    {
        return NULL;
    }

    return (PSL_Token*)vector_at(parser->tokens, parser->current_token + offset);
}

PSL_FORCE_INLINE bool psl_parser_peek_check(PSL_Parser* parser, uint32_t offset, PSL_TokenType type)
{
    PSL_Token* token = psl_parser_peek(parser, offset);

    return token != NULL && token->type == type;
}

PSL_CPP_END

#endif /* !defined(__PSL_PARSER) */