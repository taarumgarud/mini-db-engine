#pragma once
#include <cstdint>
#include "statement.h"

constexpr uint32_t PAGE_SIZE = 4096;
constexpr uint32_t MAX_PAGES = 100;
constexpr uint32_t ROW_SIZE = sizeof(Row);
constexpr uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
constexpr uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * MAX_PAGES;

struct Pager {
    void* pages[MAX_PAGES];
};

struct Table {
    uint32_t num_rows;
    Pager* pager;
};

enum class ExecuteResult {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
};

Table* new_table();
void free_table(Table* table);
void* row_slot(Table* table, uint32_t row_num);
ExecuteResult execute_insert(Statement* statement, Table* table);
ExecuteResult execute_select(Statement* statement, Table* table);
ExecuteResult execute_statement(Statement* statement, Table* table);