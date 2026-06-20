#include "table.h"
#include <cstdlib>

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

    for (uint32_t i = 0; i < MAX_PAGES; i++) {
        pager->pages[i] = nullptr;
    }

    Table* table = new Table;
    table->pager = pager;
    table->num_rows = pager->file_length / ROW_SIZE;

    return table;
}

void pager_flush(Pager* pager, uint32_t page_num, uint32_t size) {
    if (pager->pages[page_num] == nullptr) {
        return;
    }
    
    pager->file.seekp(page_num * PAGE_SIZE, std::ios::beg);
    pager->file.write(static_cast<char*>(pager->pages[page_num]), size);
    pager->file.flush();
}

void db_close(Table* table) {
    Pager* pager = table->pager;
    uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

    for (uint32_t i = 0; i < num_full_pages; i++) {
        if (pager->pages[i] != nullptr) {
            pager_flush(pager, i, PAGE_SIZE);
            free(pager->pages[i]);
            pager->pages[i] = nullptr;
        }
    }

    uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
    if (num_additional_rows > 0) {
        uint32_t page_num = num_full_pages;
        if (pager->pages[page_num] != nullptr) {
            pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);
            free(pager->pages[page_num]);
            pager->pages[page_num] = nullptr;
        }
    }

    pager->file.close();
    delete pager;
    delete table;
}

void* get_page(Pager* pager, uint32_t page_num) {
    if (page_num >= MAX_PAGES) {
        return nullptr;
    }

    if (pager->pages[page_num] == nullptr) {
        void* page = malloc(PAGE_SIZE);
        uint32_t num_pages = pager->file_length / PAGE_SIZE;

        if (pager->file_length % PAGE_SIZE) {
            num_pages += 1;
        }

        if (page_num <= num_pages) {
            pager->file.seekg(page_num * PAGE_SIZE, std::ios::beg);
            pager->file.read(static_cast<char*>(page), PAGE_SIZE);
            
            if (pager->file.eof() || pager->file.fail()) {
                pager->file.clear();
            }
        }

        pager->pages[page_num] = page;
    }

    return pager->pages[page_num];
}

void* row_slot(Table* table, uint32_t row_num) {
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void* page = get_page(table->pager, page_num);
    
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    
    return static_cast<char*>(page) + byte_offset;
}