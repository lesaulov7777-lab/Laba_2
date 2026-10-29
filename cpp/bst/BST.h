#ifndef BST_H
#define BST_H

struct BSTNode {
    int value;
    BSTNode* left;
    BSTNode* right;
    BSTNode(int v) : value(v), left(nullptr), right(nullptr) {}
};

class BST {
    BSTNode* root;
    int sz;
    void destroy(BSTNode* n);
public:
    BST();
    ~BST();
    int insert(int value); // глубина, 0 если уже есть
    int size() const;
};

#endif
