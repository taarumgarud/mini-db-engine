#pragma once
#include <cstdint>
#include <fstream>
#include <string>
#include <list>
#include <unordered_map>
#include "statement.h"
#include "btree.h"

constexpr uint32_t PAGE_SIZE = 4096;
constexpr uint32_t MAX_PAGES = 100;
constexpr uint32_t ROW_SIZE = sizeof(Row);
constexpr uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
constexpr uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * MAX_PAGES;
constexpr uint32_t BUFFER_POOL_SIZE = 4;

struct Frame {
    uint32_t page_num;
    void* data;
    bool is_dirty;
};

struct Pager {
    std::fstream file;
    uint32_t file_length;
    std::list<Frame> buffer_pool;
    std::unordered_map<uint32_t, std::list<Frame>::iterator> page_table;
};

struct Table {
    uint32_t num_rows;
    Pager* pager;
    BTree* index;
};

Table* db_open(const std::string& filename);
void db_close(Table* table);
void* get_page(Table* table, uint32_t page_num);
void* row_slot(Table* table, uint32_t row_num);
void mark_page_dirty(Table* table, uint32_t page_num);