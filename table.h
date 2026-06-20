#pragma once
#include <cstdint>
#include <fstream>
#include <string>
#include "statement.h"
#include "btree.h"

constexpr uint32_t PAGE_SIZE = 4096;
constexpr uint32_t MAX_PAGES = 100;
constexpr uint32_t ROW_SIZE = sizeof(Row);
constexpr uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
constexpr uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * MAX_PAGES;

struct Pager {
    std::fstream file;
    uint32_t file_length;
    void* pages[MAX_PAGES];
};

struct Table {
    uint32_t num_rows;
    Pager* pager;
    BTree* index;
};

Table* db_open(const std::string& filename);
void db_close(Table* table);
void* get_page(Pager* pager, uint32_t page_num);
void* row_slot(Table* table, uint32_t row_num);