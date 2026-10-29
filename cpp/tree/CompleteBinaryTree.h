#ifndef COMPLETEBINARYTREE_H
#define COMPLETEBINARYTREE_H

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

struct TreeNode {
    int value;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int val);
};

class CompleteBinaryTree {
private:
    TreeNode* root;
    int size;

    void destroyTree(TreeNode* node);
    bool findHelper(TreeNode* node, int value) const;
    int heightHelper(TreeNode* node) const;

public:
    CompleteBinaryTree();
    ~CompleteBinaryTree();

    void insert(int value);
    bool find(int value) const;
    bool isComplete() const;
    int getSize() const;
    TreeNode* getRoot() const;
    void print() const;
    vector<int> getValues() const;
};

#endif
