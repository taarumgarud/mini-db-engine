#include "btree.h"
#include <iostream>

int main() {
    BTree tree;
    init_tree(&tree);
    
    std::cout << "--- Inserting 10 and 20 ---\n";
    btree_insert(&tree, 10, 100);
    btree_insert(&tree, 20, 200);
    print_tree(&tree, tree.root_id, 0);
    
    std::cout << "\n--- Inserting 30 (Triggers Split) ---\n";
    btree_insert(&tree, 30, 300);
    print_tree(&tree, tree.root_id, 0);

    std::cout << "\n--- Inserting 40 and 50 ---\n";
    btree_insert(&tree, 40, 400);
    btree_insert(&tree, 50, 500);
    print_tree(&tree, tree.root_id, 0);
    
    bool found;
    uint32_t val = btree_search(&tree, 40, found);
    std::cout << "\nSearch Key 40: " << (found ? "Found Row " + std::to_string(val) : "Not Found") << "\n";
    
    return 0;
}