#include <iostream>
#include <string>
#include "parser.h"

void print_prompt() {
    std::cout << "db > ";
}

int main() {
    std::string input_buffer;

    while (true) {
        print_prompt();
        if (!std::getline(std::cin, input_buffer)) {
            break;
        }

        if (input_buffer.empty()) {
            continue;
        }

        if (input_buffer.find(".")==0) {
            if (input_buffer == ".exit") {
                break;
            } else {
                std::cout << "Unrecognized command '" << input_buffer << "'.\n";
                continue;
            }
        }

        Statement statement;
        PrepareResult result = prepare_statement(input_buffer, statement);

        switch (result) {
            case PrepareResult::PREPARE_SUCCESS:
                std::cout << "Parsed successfully.\n";
                if (statement.type == StatementType::STATEMENT_INSERT) {
                    std::cout << "Action: INSERT, ID: " << statement.row_to_insert.id 
                              << ", User: " << statement.row_to_insert.username 
                              << ", Email: " << statement.row_to_insert.email << "\n";
                } else if (statement.type == StatementType::STATEMENT_SELECT) {
                    std::cout << "Action: SELECT\n";
                } else if (statement.type == StatementType::STATEMENT_DELETE) {
                    std::cout << "Action: DELETE, ID: " << statement.delete_id << "\n";
                }
                break;
            case PrepareResult::PREPARE_SYNTAX_ERROR:
                std::cout << "Syntax error. Could not parse statement.\n";
                break;
            case PrepareResult::PREPARE_UNRECOGNIZED_STATEMENT:
                std::cout << "Unrecognized keyword at start of '" << input_buffer << "'.\n";
                break;
        }
    }

    return 0;
}