/* SPDX-License-Identifier: BSD-3-Clause */
/* Copyright (c) 2025 - Present Romain Augier */
/* All rights reserved. */

#include "psl/ast.h"

#include "libromano/logger.h"
#include "libromano/filesystem.h"

#include <stdio.h>

int main(void)
{
    logger_init();

    const char* example_path = TESTS_DATA_DIR"/example.psl";

    FileContent content;

    if(!fs_file_content_new(example_path, &content))
    {
        logger_log_error("Cannot open %s file", example_path);
        return 1;
    }

    printf("Source content:\n%.*s\n\n", content.content_length, content.content);

    PSL_Lexer lexer;
    psl_lexer_init(&lexer, content.content);

    Vector* tokens = vector_new(128, sizeof(PSL_Token));

    psl_lexer_lex(&lexer, tokens);

    PSL_AST* ast = psl_ast_new();

    if(!psl_ast_from_tokens(ast, tokens))
    {
        psl_ast_destroy(ast);
        vector_free(tokens);
        logger_release();
        return 1;
    }

    psl_ast_print(ast);

    psl_ast_destroy(ast);

    vector_free(tokens);

    fs_file_content_free(&content);

    logger_release();

    return 0;
}