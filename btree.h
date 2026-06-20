#pragma once
#include <cstdint>

constexpr uint32_t BTREE_ORDER = 3;
constexpr uint32_t MAX_KEYS = BTREE_ORDER - 1;

enum class NodeType {
    NODE_INTERNAL,
    NODE_LEAF
};

struct BTreeNode {
    NodeType type;
    uint32_t num_keys;
    uint32_t keys[BTREE_ORDER]; 
    uint32_t parent_id;
    uint32_t values[BTREE_ORDER]; 
    uint32_t children[BTREE_ORDER + 1]; 
    uint32_t next_leaf_id;
};

struct BTree {
    BTreeNode nodes[1000];
    uint32_t next_node_id;
    uint32_t root_id;
};

void init_tree(BTree* tree);
void btree_insert(BTree* tree, uint32_t key, uint32_t value);
uint32_t btree_search(BTree* tree, uint32_t key, bool& found);
void print_tree(BTree* tree, uint32_t node_id, int level);