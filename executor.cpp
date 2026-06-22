#include "executor.h"
#include <cstring>
#include <iostream>

ExecuteResult execute_insert(Statement* statement, Table* table) {
    if (table->num_rows >= TABLE_MAX_ROWS) {
        return ExecuteResult::EXECUTE_TABLE_FULL;
    }
    
    Row* row_to_insert = &(statement->row_to_insert);
    
    bool found;
    uint32_t row_num = btree_search(table->index, row_to_insert->id, found);
    if (found) {
        void* slot = row_slot(table, row_num);
        Row existing_row;
        std::memcpy(&existing_row, slot, ROW_SIZE);
        if (existing_row.is_deleted == 0) {
            return ExecuteResult::EXECUTE_DUPLICATE_KEY;
        }
    }
    
    row_to_insert->is_deleted = 0;
    
    void* destination = row_slot(table, table->num_rows);
    std::memcpy(destination, row_to_insert, ROW_SIZE);
    
    btree_insert(table->index, row_to_insert->id, table->num_rows);
    mark_page_dirty(table, table->num_rows / ROWS_PER_PAGE);
    
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

ExecuteResult execute_select_by_id(Statement* statement, Table* table) {
    bool found;
    uint32_t row_num = btree_search(table->index, statement->target_id, found);
    
    if (!found) {
        std::cout << "Error: Record not found.\n";
        return ExecuteResult::EXECUTE_SUCCESS;
    }
    
    void* source = row_slot(table, row_num);
    Row row;
    std::memcpy(&row, source, ROW_SIZE);
    
    if (row.is_deleted == 1) {
        std::cout << "Error: Record not found.\n";
    } else {
        std::cout << "(" << row.id << ", " << row.username << ", " << row.email << ")\n";
    }
    
    return ExecuteResult::EXECUTE_SUCCESS;
}

ExecuteResult execute_delete(Statement* statement, Table* table) {
    bool found;
    uint32_t row_num = btree_search(table->index, statement->target_id, found);
    
    if (!found) {
        std::cout << "Error: Record not found.\n";
        return ExecuteResult::EXECUTE_SUCCESS;
    }
    
    void* slot = row_slot(table, row_num);
    Row row;
    std::memcpy(&row, slot, ROW_SIZE);
    
    if (row.is_deleted == 1) {
        std::cout << "Error: Record not found.\n";
        return ExecuteResult::EXECUTE_SUCCESS;
    }
    
    row.is_deleted = 1;
    std::memcpy(slot, &row, ROW_SIZE);
    mark_page_dirty(table, row_num / ROWS_PER_PAGE);
    
    return ExecuteResult::EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement* statement, Table* table) {
    switch (statement->type) {
        case StatementType::STATEMENT_INSERT:
            return execute_insert(statement, table);
        case StatementType::STATEMENT_SELECT:
            return execute_select(statement, table);
        case StatementType::STATEMENT_SELECT_BY_ID:
            return execute_select_by_id(statement, table);
        case StatementType::STATEMENT_DELETE:
            return execute_delete(statement, table);
    }
    return ExecuteResult::EXECUTE_SUCCESS;
}