#include "table.h"
#include <cstdlib>
#include <cstring>

Table* db_open(const std::string& filename) {
    Pager* pager = new Pager;
    
    pager->file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!pager->file.is_open()) {
        pager->file.clear();
        pager->file.open(filename, std::ios::out | std::ios::binary);
        pager->file.close();
        pager->file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }

    pager->file.seekg(0, std::ios::end);
    pager->file_length = pager->file.tellg();
    pager->file.seekg(0, std::ios::beg);

    Table* table = new Table;
    table->pager = pager;
    table->num_rows = (pager->file_length / PAGE_SIZE) * ROWS_PER_PAGE + (pager->file_length % PAGE_SIZE) / ROW_SIZE;
    
    table->index = new BTree;
    init_tree(table->index);
    
    for (uint32_t i = 0; i < table->num_rows; i++) {
        void* slot = row_slot(table, i);
        Row row;
        std::memcpy(&row, slot, ROW_SIZE);
        if (row.is_deleted == 0) {
            btree_insert(table->index, row.id, i);
        }
    }

    return table;
}

void db_close(Table* table) {
    Pager* pager = table->pager;
    uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

    for (auto& frame : pager->buffer_pool) {
        if (frame.is_dirty) {
            uint32_t write_size = PAGE_SIZE;
            if (frame.page_num == num_full_pages) {
                write_size = (table->num_rows % ROWS_PER_PAGE) * ROW_SIZE;
            }
            if (write_size > 0) {
                pager->file.seekp(frame.page_num * PAGE_SIZE, std::ios::beg);
                pager->file.write(static_cast<char*>(frame.data), write_size);
            }
        }
        free(frame.data);
    }

    pager->file.close();
    delete pager;
    delete table->index;
    delete table;
}

void* get_page(Table* table, uint32_t page_num) {
    if (page_num >= MAX_PAGES) {
        return nullptr;
    }

    Pager* pager = table->pager;
    auto it = pager->page_table.find(page_num);

    if (it != pager->page_table.end()) {
        pager->buffer_pool.splice(pager->buffer_pool.begin(), pager->buffer_pool, it->second);
        return pager->buffer_pool.begin()->data;
    }

    if (pager->buffer_pool.size() >= BUFFER_POOL_SIZE) {
        Frame lru_frame = pager->buffer_pool.back();
        if (lru_frame.is_dirty) {
            uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;
            uint32_t write_size = PAGE_SIZE;
            if (lru_frame.page_num == num_full_pages) {
                write_size = (table->num_rows % ROWS_PER_PAGE) * ROW_SIZE;
            }
            if (write_size > 0) {
                pager->file.seekp(lru_frame.page_num * PAGE_SIZE, std::ios::beg);
                pager->file.write(static_cast<char*>(lru_frame.data), write_size);
                pager->file.flush();
            }
        }
        pager->page_table.erase(lru_frame.page_num);
        free(lru_frame.data);
        pager->buffer_pool.pop_back();
    }

    void* page_data = malloc(PAGE_SIZE);
    std::memset(page_data, 0, PAGE_SIZE);

    uint32_t current_file_pages = pager->file_length / PAGE_SIZE;
    if (pager->file_length % PAGE_SIZE) {
        current_file_pages++;
    }

    if (page_num < current_file_pages) {
        pager->file.seekg(page_num * PAGE_SIZE, std::ios::beg);
        pager->file.read(static_cast<char*>(page_data), PAGE_SIZE);
        if (pager->file.eof() || pager->file.fail()) {
            pager->file.clear();
        }
    }

    Frame new_frame = { page_num, page_data, false };
    pager->buffer_pool.push_front(new_frame);
    pager->page_table[page_num] = pager->buffer_pool.begin();

    return page_data;
}

void* row_slot(Table* table, uint32_t row_num) {
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void* page = get_page(table, page_num);
    
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    
    return static_cast<char*>(page) + byte_offset;
}

void mark_page_dirty(Table* table, uint32_t page_num) {
    Pager* pager = table->pager;
    auto it = pager->page_table.find(page_num);
    if (it != pager->page_table.end()) {
        it->second->is_dirty = true;
    }
}