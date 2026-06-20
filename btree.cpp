#include "btree.h"
#include <iostream>

void init_tree(BTree* tree) {
    tree->next_node_id = 0;
    tree->root_id = 0;
    
    BTreeNode& root = tree->nodes[tree->next_node_id];
    root.type = NodeType::NODE_LEAF;
    root.num_keys = 0;
    root.parent_id = 0;
    root.next_leaf_id = 0;
    tree->next_node_id++;
}

uint32_t btree_search(BTree* tree, uint32_t key, bool& found) {
    uint32_t curr_id = tree->root_id;
    
    while (tree->nodes[curr_id].type == NodeType::NODE_INTERNAL) {
        BTreeNode& node = tree->nodes[curr_id];
        uint32_t i = 0;
        while (i < node.num_keys && key >= node.keys[i]) {
            i++;
        }
        curr_id = node.children[i];
    }
    
    BTreeNode& leaf = tree->nodes[curr_id];
    for (uint32_t i = 0; i < leaf.num_keys; i++) {
        if (leaf.keys[i] == key) {
            found = true;
            return leaf.values[i];
        }
    }
    
    found = false;
    return 0;
}

void insert_into_node(BTree* tree, uint32_t node_id, uint32_t key, uint32_t value) {
    BTreeNode& node = tree->nodes[node_id];
    uint32_t i = 0;
    while (i < node.num_keys && node.keys[i] < key) {
        i++;
    }
    for (uint32_t j = node.num_keys; j > i; j--) {
        node.keys[j] = node.keys[j - 1];
        node.values[j] = node.values[j - 1];
    }
    node.keys[i] = key;
    node.values[i] = value;
    node.num_keys++;
}

void split_leaf(BTree* tree, uint32_t leaf_id) {
    BTreeNode& leaf = tree->nodes[leaf_id];
    uint32_t new_leaf_id = tree->next_node_id++;
    BTreeNode& new_leaf = tree->nodes[new_leaf_id];
    
    new_leaf.type = NodeType::NODE_LEAF;
    new_leaf.parent_id = leaf.parent_id;
    new_leaf.num_keys = 0;
    
    new_leaf.next_leaf_id = leaf.next_leaf_id;
    leaf.next_leaf_id = new_leaf_id;
    
    uint32_t split_index = leaf.num_keys / 2;
    uint32_t keys_to_move = leaf.num_keys - split_index;
    
    for (uint32_t i = 0; i < keys_to_move; i++) {
        new_leaf.keys[i] = leaf.keys[split_index + i];
        new_leaf.values[i] = leaf.values[split_index + i];
    }
    
    leaf.num_keys = split_index;
    new_leaf.num_keys = keys_to_move;
    
    uint32_t split_key = new_leaf.keys[0];
    
    if (leaf_id == tree->root_id) {
        uint32_t new_root_id = tree->next_node_id++;
        BTreeNode& new_root = tree->nodes[new_root_id];
        new_root.type = NodeType::NODE_INTERNAL;
        new_root.num_keys = 1;
        new_root.keys[0] = split_key;
        new_root.children[0] = leaf_id;
        new_root.children[1] = new_leaf_id;
        new_root.parent_id = 0;
        
        leaf.parent_id = new_root_id;
        new_leaf.parent_id = new_root_id;
        tree->root_id = new_root_id;
    } else {
        uint32_t parent_id = leaf.parent_id;
        BTreeNode& parent = tree->nodes[parent_id];
        
        uint32_t i = 0;
        while (i < parent.num_keys && parent.keys[i] < split_key) {
            i++;
        }
        
        for (uint32_t j = parent.num_keys; j > i; j--) {
            parent.keys[j] = parent.keys[j - 1];
            parent.children[j + 1] = parent.children[j];
        }
        
        parent.keys[i] = split_key;
        parent.children[i + 1] = new_leaf_id;
        parent.num_keys++;
    }
}

void btree_insert(BTree* tree, uint32_t key, uint32_t value) {
    uint32_t curr_id = tree->root_id;
    
    while (tree->nodes[curr_id].type == NodeType::NODE_INTERNAL) {
        BTreeNode& node = tree->nodes[curr_id];
        uint32_t i = 0;
        while (i < node.num_keys && key >= node.keys[i]) {
            i++;
        }
        curr_id = node.children[i];
    }
    
    insert_into_node(tree, curr_id, key, value);
    
    if (tree->nodes[curr_id].num_keys > MAX_KEYS) {
        split_leaf(tree, curr_id);
    }
}

void print_tree(BTree* tree, uint32_t node_id, int level) {
    BTreeNode& node = tree->nodes[node_id];
    for (int i = 0; i < level; i++) std::cout << "  ";
    
    if (node.type == NodeType::NODE_LEAF) {
        std::cout << "- Leaf [ ";
        for (uint32_t i = 0; i < node.num_keys; i++) {
            std::cout << node.keys[i] << ":" << node.values[i] << " ";
        }
        std::cout << "]\n";
    } else {
        std::cout << "- Internal [ ";
        for (uint32_t i = 0; i < node.num_keys; i++) {
            std::cout << node.keys[i] << " ";
        }
        std::cout << "]\n";
        for (uint32_t i = 0; i <= node.num_keys; i++) {
            print_tree(tree, node.children[i], level + 1);
        }
    }
}