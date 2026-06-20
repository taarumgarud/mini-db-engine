#include "table.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

Table* new_table() {
    Table* table = new Table;
    table->num_rows = 0;
    table->pager = new Pager;
    for (uint32_t i = 0; i < MAX_PAGES; i++) {
        table->pager->pages[i] = nullptr;
    }
    return table;
}

void free_table(Table* table) {
    for (int i = 0; table->pager->pages[i] != nullptr; i++) {
        free(table->pager->pages[i]);
    }
    delete table->pager;
    delete table;
}

void* row_slot(Table* table, uint32_t row_num) {
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void* page = table->pager->pages[page_num];
    
    if (page == nullptr) {
        page = malloc(PAGE_SIZE);
        table->pager->pages[page_num] = page;
    }
    
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    
    return static_cast<char*>(page) + byte_offset;
}

ExecuteResult execute_insert(Statement* statement, Table* table) {
    if (table->num_rows >= TABLE_MAX_ROWS) {
        return ExecuteResult::EXECUTE_TABLE_FULL;
    }
    
    Row* row_to_insert = &(statement->row_to_insert);
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
        std::cout << "(" << row.id << ", " << row.username << ", " << row.email << ")\n";
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