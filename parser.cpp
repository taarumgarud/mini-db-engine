#include "parser.h"
#include <sstream>
#include <cstring>
#include <iostream>

PrepareResult prepare_insert(const std::string& input, Statement& statement) {
    statement.type = StatementType::STATEMENT_INSERT;
    
    char keyword_insert[10], keyword_into[10], table_name[20], keyword_values[10];
    int id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
    
    int parsed = sscanf(input.c_str(), "%9s %9s %19s %9s (%d, %31[^,], %63[^)])", 
                        keyword_insert, keyword_into, table_name, keyword_values, 
                        &id, username, email);

    if (parsed != 7) {
        return PrepareResult::PREPARE_SYNTAX_ERROR;
    }

    statement.row_to_insert.id = id;
    std::strncpy(statement.row_to_insert.username, username, COLUMN_USERNAME_SIZE - 1);
    statement.row_to_insert.username[COLUMN_USERNAME_SIZE - 1] = '\0';
    
    std::strncpy(statement.row_to_insert.email, email, COLUMN_EMAIL_SIZE - 1);
    statement.row_to_insert.email[COLUMN_EMAIL_SIZE - 1] = '\0';

    return PrepareResult::PREPARE_SUCCESS;
}

PrepareResult prepare_delete(const std::string& input, Statement& statement) {
    statement.type = StatementType::STATEMENT_DELETE;
    
    char keyword_delete[10], keyword_from[10], table_name[20], keyword_where[10], column_name[10];
    int id;
    
    int parsed = sscanf(input.c_str(), "%9s %9s %19s %9s %9s = %d", 
                        keyword_delete, keyword_from, table_name, keyword_where, column_name, &id);

    if (parsed != 6) {
        return PrepareResult::PREPARE_SYNTAX_ERROR;
    }

    statement.delete_id = id;
    return PrepareResult::PREPARE_SUCCESS;
}

PrepareResult prepare_statement(const std::string& input, Statement& statement) {
    if (input.find("INSERT")==0) {
        return prepare_insert(input, statement);
    }
    if (input.find("SELECT")==0) {
        statement.type = StatementType::STATEMENT_SELECT;
        return PrepareResult::PREPARE_SUCCESS;
    }
    if (input.find("DELETE")==0) {
        return prepare_delete(input, statement);
    }

    return PrepareResult::PREPARE_UNRECOGNIZED_STATEMENT;
}