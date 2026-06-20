#include "parser.h"
#include <sstream>
#include <cstring>
#include <iostream>

PrepareResult prepare_insert(const std::string& input, Statement& statement) {
    statement.type = StatementType::STATEMENT_INSERT;
    
    char keyword_insert[10], keyword_into[10], table_name[20], keyword_values[10];
    int id;
    char username[256]; 
    char email[256];
    
    int parsed = sscanf(input.c_str(), "%9s %9s %19s %9s (%d, %255[^,], %255[^)])", 
                        keyword_insert, keyword_into, table_name, keyword_values, 
                        &id, username, email);

    if (parsed != 7) {
        return PrepareResult::PREPARE_SYNTAX_ERROR;
    }

    if (std::strlen(username) >= COLUMN_USERNAME_SIZE) {
        return PrepareResult::PREPARE_STRING_TOO_LONG;
    }
    if (std::strlen(email) >= COLUMN_EMAIL_SIZE) {
        return PrepareResult::PREPARE_STRING_TOO_LONG;
    }

    statement.row_to_insert.id = id;
    std::strcpy(statement.row_to_insert.username, username);
    std::strcpy(statement.row_to_insert.email, email);

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

    statement.target_id = id;
    return PrepareResult::PREPARE_SUCCESS;
}

PrepareResult prepare_statement(const std::string& input, Statement& statement) {
    if (input.find("INSERT") == 0) {
        return prepare_insert(input, statement);
    }
    if (input.find("SELECT") == 0) {
        if (input.find("WHERE id =") != std::string::npos) {
            statement.type = StatementType::STATEMENT_SELECT_BY_ID;
            int id;
            sscanf(input.c_str(), "SELECT * FROM users WHERE id = %d", &id);
            statement.target_id = id;
        } else {
            statement.type = StatementType::STATEMENT_SELECT;
        }
        return PrepareResult::PREPARE_SUCCESS;
    }
    if (input.find("DELETE") == 0) {
        return prepare_delete(input, statement);
    }

    return PrepareResult::PREPARE_UNRECOGNIZED_STATEMENT;
}