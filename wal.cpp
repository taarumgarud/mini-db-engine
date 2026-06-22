#include "wal.h"
#include <fstream>
#include <cstring>
#include <cstdio>

void wal_open(Table* table, const std::string& filename) {
    table->wal_file.open(filename, std::ios::out | std::ios::app | std::ios::binary);
}

void wal_append(Table* table, Row* row) {
    table->wal_file.write(reinterpret_cast<char*>(row), ROW_SIZE);
    table->wal_file.flush();
}

void wal_replay(Table* table, const std::string& filename) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        return;
    }

    Row row;
    while (file.read(reinterpret_cast<char*>(&row), ROW_SIZE)) {
        bool found;
        uint32_t row_num = btree_search(table->index, row.id, found);
        
        if (found) {
            void* slot = row_slot(table, row_num);
            std::memcpy(slot, &row, ROW_SIZE);
            mark_page_dirty(table, row_num / ROWS_PER_PAGE);
        } else {
            void* slot = row_slot(table, table->num_rows);
            std::memcpy(slot, &row, ROW_SIZE);
            btree_insert(table->index, row.id, table->num_rows);
            mark_page_dirty(table, table->num_rows / ROWS_PER_PAGE);
            table->num_rows++;
        }
    }
    file.close();
}

void wal_truncate(Table* table, const std::string& filename) {
    if (table->wal_file.is_open()) {
        table->wal_file.close();
    }
    std::remove(filename.c_str());
}