#include "executor.h"
#include <cstring>
#include <iostream>

ExecuteResult execute_insert(Statement* statement, Table* table) {
    if (table->num_rows >= TABLE_MAX_ROWS) {
        return ExecuteResult::EXECUTE_TABLE_FULL;
    }
    
    Row* row_to_insert = &(statement->row_to_insert);
    row_to_insert->is_deleted = 0;
    
    void* destination = row_slot(table, table->num_rows);
    std::memcpy(destination, row_to_insert, ROW_SIZE);
    
    table->num_rows++;
    return ExecuteResult::EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement* statement, Table* table) {
    Row row;
    for (uint32_t i = 0; i < table->num_rows; i++) {
        void* source = row_slot(table, i);
        std::memcpy(&row, source, ROW_SIZE);
        
        if (row.is_deleted == 0) {
            std::cout << "(" << row.id << ", " << row.username << ", " << row.email << ")\n";
        }
    }
    return ExecuteResult::EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement* statement, Table* table) {
    switch (statement->type) {
        case StatementType::STATEMENT_INSERT:
            return execute_insert(statement, table);
        case StatementType::STATEMENT_SELECT:
            return execute_select(statement, table);
        case StatementType::STATEMENT_DELETE:
            return ExecuteResult::EXECUTE_SUCCESS;
    }
    return ExecuteResult::EXECUTE_SUCCESS;
}