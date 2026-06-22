#include <iostream>
#include <chrono>
#include <string>
#include <cstring>
#include <cstdio>
#include "table.h"
#include "executor.h"

const std::string BENCH_DB = "benchmark.db";
const int NUM_OPERATIONS = 10000;

void setup_bench_db() {
    std::remove(BENCH_DB.c_str());
    std::remove((BENCH_DB + ".wal").c_str());
}

int main() {
    setup_bench_db();
    Table* table = db_open(BENCH_DB);

    std::cout << "--- MiniDB Benchmark ---\n";
    std::cout << "Target: " << NUM_OPERATIONS << " operations\n\n";

    auto start_write = std::chrono::high_resolution_clock::now();

    for (int i = 1; i <= NUM_OPERATIONS; i++) {
        Statement insert_stmt;
        insert_stmt.type = StatementType::STATEMENT_INSERT;
        insert_stmt.row_to_insert.id = i;
        std::strcpy(insert_stmt.row_to_insert.username, "bench_user");
        std::strcpy(insert_stmt.row_to_insert.email, "bench@dtu.ac.in");

        execute_insert(&insert_stmt, table);
    }

    auto end_write = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> write_duration = end_write - start_write;
    double writes_per_second = NUM_OPERATIONS / write_duration.count();

    std::cout << "[WRITE] Insertions complete.\n";
    std::cout << "Time: " << write_duration.count() << " seconds\n";
    std::cout << "Throughput: " << writes_per_second << " Ops/sec\n\n";

    auto start_read = std::chrono::high_resolution_clock::now();

    for (int i = 1; i <= NUM_OPERATIONS; i++) {
        bool found;
        uint32_t row_num = btree_search(table->index, i, found);
        
        if (found) {
            void* slot = row_slot(table, row_num);
            Row row;
            std::memcpy(&row, slot, ROW_SIZE);
        }
    }

    auto end_read = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> read_duration = end_read - start_read;
    double reads_per_second = NUM_OPERATIONS / read_duration.count();

    std::cout << "[READ] Point queries complete.\n";
    std::cout << "Time: " << read_duration.count() << " seconds\n";
    std::cout << "Throughput: " << reads_per_second << " Ops/sec\n\n";

    db_close(table);
    std::remove(BENCH_DB.c_str());
    std::remove((BENCH_DB + ".wal").c_str());

    return 0;
}