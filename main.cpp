#include <iostream>
#include <string>
#include "parser.h"
#include "table.h"
#include "executor.h"

void print_prompt() {
    std::cout << "db > ";
}

int main() {
    Table* table = db_open("database.db");
    std::string input_buffer;

    while (true) {
        print_prompt();
        if (!std::getline(std::cin, input_buffer)) {
            break;
        }

        if (input_buffer.empty()) {
            continue;
        }

        if (input_buffer.find(".") == 0) {
            if (input_buffer == ".exit") {
                db_close(table);
                break;
            } else {
                std::cout << "Unrecognized command '" << input_buffer << "'.\n";
                continue;
            }
        }

        Statement statement;
        PrepareResult result = prepare_statement(input_buffer, statement);

        if (result != PrepareResult::PREPARE_SUCCESS) {
            if (result == PrepareResult::PREPARE_SYNTAX_ERROR) {
                std::cout << "Syntax error. Could not parse statement.\n";
            } else if (result == PrepareResult::PREPARE_UNRECOGNIZED_STATEMENT) {
                std::cout << "Unrecognized keyword at start of '" << input_buffer << "'.\n";
            } else if (result == PrepareResult::PREPARE_STRING_TOO_LONG) {
                std::cout << "String is too long.\n";
            }
            continue;
        }

        ExecuteResult execute_result = execute_statement(&statement, table);

        if (execute_result == ExecuteResult::EXECUTE_SUCCESS) {
            std::cout << "Executed.\n";
        } else if (execute_result == ExecuteResult::EXECUTE_TABLE_FULL) {
            std::cout << "Error: Table full.\n";
        } else if (execute_result == ExecuteResult::EXECUTE_DUPLICATE_KEY) {
            std::cout << "Error: Duplicate key.\n";
        }
    }

    return 0;
}