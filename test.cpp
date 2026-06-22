#include "test.h"
#include "table.h"
#include "executor.h"
#include <cstdio>
#include <cstring>

const std::string TEST_DB = "test.db";

void setup_test_db() {
    std::remove(TEST_DB.c_str());
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

int main() {
    std::cout << "--- MiniDB Test Suite ---\n";
    
    RUN_TEST(test_insert_and_retrieve);
    RUN_TEST(test_duplicate_key_constraint);
    RUN_TEST(test_persistence_and_index_rebuild);
    
    std::cout << "-------------------------\n";
    std::remove(TEST_DB.c_str());
    return 0;
}