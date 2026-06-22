#pragma once
#include "table.h"
#include "statement.h"
#include <string>

void wal_open(Table* table, const std::string& filename);
void wal_append(Table* table, Row* row);
void wal_replay(Table* table, const std::string& filename);
void wal_truncate(Table* table, const std::string& filename);