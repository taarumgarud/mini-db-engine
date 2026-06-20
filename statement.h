#pragma once
#include <cstdint>

constexpr uint32_t COLUMN_USERNAME_SIZE = 32;
constexpr uint32_t COLUMN_EMAIL_SIZE = 64;

struct Row {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
};

enum class StatementType {
    STATEMENT_INSERT,
    STATEMENT_SELECT,
    STATEMENT_DELETE
};

struct Statement {
    StatementType type;
    Row row_to_insert;
    uint32_t delete_id;
};

enum class PrepareResult {
    PREPARE_SUCCESS,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT
};