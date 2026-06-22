#include "test.h"
#include "table.h"
#include "executor.h"
#include <cstdio>
#include <cstring>

const std::string TEST_DB = "test.db";

void setup_test_db() {
    std::remove(TEST_DB.c_str());
    std::remove((TEST_DB + ".wal").c_str());
}

bool test_crash_recovery() {
    setup_test_db();
    Table* table = db_open(TEST_DB);

    Statement insert_stmt;
    insert_stmt.type = StatementType::STATEMENT_INSERT;
    insert_stmt.row_to_insert.id = 999;
    std::strcpy(insert_stmt.row_to_insert.username, "crash_dummy");
    std::strcpy(insert_stmt.row_to_insert.email, "crash@dtu.ac.in");

    execute_insert(&insert_stmt, table);

    table->pager->file.close();
    table->wal_file.close();

    Table* recovered_table = db_open(TEST_DB);
    
    bool found;
    uint32_t row_num = btree_search(recovered_table->index, 999, found);
    ASSERT_TRUE(found);

    void* slot = row_slot(recovered_table, row_num);
    Row recovered_row;
    std::memcpy(&recovered_row, slot, ROW_SIZE);

    ASSERT_EQ(recovered_row.id, 999);
    ASSERT_TRUE(std::strcmp(recovered_row.username, "crash_dummy") == 0);

    db_close(recovered_table);
    return true;
}

bool test_insert_and_retrieve() {
    setup_test_db();
    Table* table = db_open(TEST_DB);

    Statement insert_stmt;
    insert_stmt.type = StatementType::STATEMENT_INSERT;
    insert_stmt.row_to_insert.id = 1;
    std::strcpy(insert_stmt.row_to_insert.username, "rudra");
    std::strcpy(insert_stmt.row_to_insert.email, "rudra@dtu.ac.in");

    ExecuteResult res = execute_insert(&insert_stmt, table);
    ASSERT_EQ(res, ExecuteResult::EXECUTE_SUCCESS);
    ASSERT_EQ(table->num_rows, 1);

    bool found;
    uint32_t row_num = btree_search(table->index, 1, found);
    ASSERT_TRUE(found);

    void* slot = row_slot(table, row_num);
    Row retrieved_row;
    std::memcpy(&retrieved_row, slot, ROW_SIZE);

    ASSERT_EQ(retrieved_row.id, 1);
    ASSERT_TRUE(std::strcmp(retrieved_row.username, "rudra") == 0);
    ASSERT_EQ(retrieved_row.is_deleted, 0);

    db_close(table);
    return true;
}

bool test_duplicate_key_constraint() {
    setup_test_db();
    Table* table = db_open(TEST_DB);

    Statement insert_stmt1;
    insert_stmt1.type = StatementType::STATEMENT_INSERT;
    insert_stmt1.row_to_insert.id = 99;
    std::strcpy(insert_stmt1.row_to_insert.username, "alice");
    std::strcpy(insert_stmt1.row_to_insert.email, "alice@example.com");

    ASSERT_EQ(execute_insert(&insert_stmt1, table), ExecuteResult::EXECUTE_SUCCESS);

    Statement insert_stmt2;
    insert_stmt2.type = StatementType::STATEMENT_INSERT;
    insert_stmt2.row_to_insert.id = 99;
    std::strcpy(insert_stmt2.row_to_insert.username, "bob");
    std::strcpy(insert_stmt2.row_to_insert.email, "bob@example.com");

    ASSERT_EQ(execute_insert(&insert_stmt2, table), ExecuteResult::EXECUTE_DUPLICATE_KEY);
    ASSERT_EQ(table->num_rows, 1);

    db_close(table);
    return true;
}

bool test_persistence_and_index_rebuild() {
    setup_test_db();
    
    Table* table = db_open(TEST_DB);
    Statement insert_stmt;
    insert_stmt.type = StatementType::STATEMENT_INSERT;
    insert_stmt.row_to_insert.id = 42;
    std::strcpy(insert_stmt.row_to_insert.username, "persisted");
    std::strcpy(insert_stmt.row_to_insert.email, "test@example.com");
    
    execute_insert(&insert_stmt, table);
    db_close(table);

    Table* reopened_table = db_open(TEST_DB);
    ASSERT_EQ(reopened_table->num_rows, 1);

    bool found;
    btree_search(reopened_table->index, 42, found);
    ASSERT_TRUE(found);

    db_close(reopened_table);
    return true;
}

bool test_lru_eviction() {
    setup_test_db();
    Table* table = db_open(TEST_DB);

    for (uint32_t i = 1; i <= 170; i++) {
        Statement insert_stmt;
        insert_stmt.type = StatementType::STATEMENT_INSERT;
        insert_stmt.row_to_insert.id = i;
        std::strcpy(insert_stmt.row_to_insert.username, "user");
        std::strcpy(insert_stmt.row_to_insert.email, "user@example.com");
        execute_insert(&insert_stmt, table);
    }

    ASSERT_EQ(table->num_rows, 170);
    db_close(table);

    Table* reopened_table = db_open(TEST_DB);
    ASSERT_EQ(reopened_table->num_rows, 170);

    bool found_first, found_last;
    uint32_t row_first = btree_search(reopened_table->index, 1, found_first);
    uint32_t row_last = btree_search(reopened_table->index, 170, found_last);

    ASSERT_TRUE(found_first);
    ASSERT_TRUE(found_last);

    void* slot_first = row_slot(reopened_table, row_first);
    Row row_f;
    std::memcpy(&row_f, slot_first, ROW_SIZE);
    ASSERT_EQ(row_f.id, 1);

    void* slot_last = row_slot(reopened_table, row_last);
    Row row_l;
    std::memcpy(&row_l, slot_last, ROW_SIZE);
    ASSERT_EQ(row_l.id, 170);

    db_close(reopened_table);
    return true;
}

int main() {
    std::cout << "--- MiniDB Test Suite ---\n";
    
    RUN_TEST(test_insert_and_retrieve);
    RUN_TEST(test_duplicate_key_constraint);
    RUN_TEST(test_persistence_and_index_rebuild);
    RUN_TEST(test_lru_eviction);
    RUN_TEST(test_crash_recovery);
    
    std::cout << "-------------------------\n";
    std::remove(TEST_DB.c_str());
    return 0;
}