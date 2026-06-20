#pragma once
#include "statement.h"
#include "table.h"

enum class ExecuteResult {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL,
    EXECUTE_DUPLICATE_KEY
};

ExecuteResult execute_insert(Statement* statement, Table* table);
ExecuteResult execute_select(Statement* statement, Table* table);
ExecuteResult execute_select_by_id(Statement* statement, Table* table);
ExecuteResult execute_delete(Statement* statement, Table* table);
ExecuteResult execute_statement(Statement* statement, Table* table);