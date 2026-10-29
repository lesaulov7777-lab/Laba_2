#include "BST.h"
#include <vector>

BST::BST() : root(nullptr), sz(0) {}
// Итеративное удаление: на вырожденном дереве (отсортированный ввод) рекурсия переполнила бы стек
void BST::destroy(BSTNode* n) {
    std::vector<BSTNode*> st;
    if (n) st.push_back(n);
    while (!st.empty()) {
        BSTNode* cur = st.back();
        st.pop_back();
        if (cur->left) st.push_back(cur->left);
        if (cur->right) st.push_back(cur->right);
        delete cur;
    }
}
BST::~BST() { destroy(root); }
int BST::size() const { return sz; }
int BST::insert(int value) {
    if (!root) {
        root = new BSTNode(value);
        sz++;
        return 1;
    }
    BSTNode* cur = root;
    int depth = 1;
    while (true) {
        if (value == cur->value) return 0;
        depth++;
        if (value < cur->value) {
            if (!cur->left) { cur->left = new BSTNode(value); sz++; return depth; }
            cur = cur->left;
        } else {
            if (!cur->right) { cur->right = new BSTNode(value); sz++; return depth; }
            cur = cur->right;
        }
    }
}
