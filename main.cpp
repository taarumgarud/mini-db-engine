#include <iostream>
#include <string>
#include "parser.h"
#include "table.h"

void print_prompt() {
    std::cout << "db > ";
}

int main() {
    Table* table = new_table();
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
                free_table(table);
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
            }
            continue;
        }

        ExecuteResult execute_result = execute_statement(&statement, table);

        if (execute_result == ExecuteResult::EXECUTE_SUCCESS) {
            std::cout << "Executed.\n";
        } else if (execute_result == ExecuteResult::EXECUTE_TABLE_FULL) {
            std::cout << "Error: Table full.\n";
        }
    }

    return 0;
}