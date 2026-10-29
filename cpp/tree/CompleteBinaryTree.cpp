#include"CompleteBinaryTree.h"
#include<iostream>
#include <queue>
#include <algorithm>
using namespace std;

CompleteBinaryTree::CompleteBinaryTree() {
    root = nullptr;
    size = 0;
}

TreeNode::TreeNode(int val) {
    value = val;
    left = nullptr;
    right = nullptr;
}

void CompleteBinaryTree::destroyTree(TreeNode* node) {
    if (node == nullptr) return;
    destroyTree(node->left);
    destroyTree(node->right);
    delete node;
}

CompleteBinaryTree::~CompleteBinaryTree() {
    destroyTree(root);
    root = nullptr;
    size = 0;
}

void CompleteBinaryTree::insert(int value) {
    TreeNode* newNode = new TreeNode(value);

    if (root == nullptr) {
        root = newNode;
        size++;
        return;
    }

    queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        TreeNode* current = q.front();
        q.pop();

        if (current->left == nullptr) {
            current->left = newNode;
            size++;
            return;
        } else {
            q.push(current->left);
        }

        if (current->right == nullptr) {
            current->right = newNode;
            size++;
            return;
        } else {
            q.push(current->right);
        }
    }
}

bool CompleteBinaryTree::findHelper(TreeNode* node, int value) const {
    if (node == nullptr) return false;
    if (node->value == value) return true;
    return findHelper(node->left, value) || findHelper(node->right, value);
}

bool CompleteBinaryTree::find(int value) const {
    return findHelper(root, value);
}

int CompleteBinaryTree::getSize() const {
    return size;
}

TreeNode* CompleteBinaryTree::getRoot() const {
    return root;
}

vector<int> CompleteBinaryTree::getValues() const {
    vector<int> values;
    if(root==nullptr){
        return values;
    }

    queue<TreeNode*> q;
    q.push(root);

    while(!q.empty()){
        TreeNode* current = q.front();
        q.pop();

        values.push_back(current->value);

        if(current->left!=nullptr){
            q.push(current->left);
        }
        if(current->right!=nullptr){
            q.push(current->right);
        }
    }
    return values;
}

bool CompleteBinaryTree::isComplete() const{
    if (root == nullptr){
        return true;
    }
    queue<TreeNode*> q;
    q.push(root);
    bool seenNull = false;
    while(!q.empty()){
        TreeNode* current = q.front();
        q.pop();
        if(current == nullptr){
            seenNull = true;
        } else {
            if(seenNull == true){
                return false;
            }
            q.push(current->left);
            q.push(current->right);
        }
    }
    return true;
}

int CompleteBinaryTree::heightHelper(TreeNode* node) const {
    if (node == nullptr) return -1;
    return 1 + max(heightHelper(node->left), heightHelper(node->right));
}

void CompleteBinaryTree::print() const {
    if (root == nullptr) {
        cout << "[]" << endl;
        return;
    }

    int h = heightHelper(root);

    queue<TreeNode*> q;
    q.push(root);
    int level = 0;

    while (!q.empty()) {
        int levelSize = q.size();
        int spacesBefore = (1 << (h - level)) - 1;
        int spacesBetween = (1 << (h - level + 1)) - 1;

        vector<TreeNode*> levelNodes;

        for (int i = 0; i < spacesBefore; i++) cout << " ";

        for (int i = 0; i < levelSize; i++) {
            TreeNode* current = q.front();
            q.pop();
            levelNodes.push_back(current);

            cout << current->value;

            if (i < levelSize - 1) {
                for (int j = 0; j < spacesBetween; j++) cout << " ";
            }

            if (current->left) q.push(current->left);
            if (current->right) q.push(current->right);
        }
        cout << endl;

        if (level < h) {
            int lineIndent = spacesBefore - 1;
            if (lineIndent < 0) lineIndent = 0;
            for (int i = 0; i < lineIndent; i++) cout << " ";

            for (size_t i = 0; i < levelNodes.size(); i++) {
                TreeNode* node = levelNodes[i];

                if (node->left) cout << "/";
                else cout << " ";

                cout << " ";

                if (node->right) cout << "\\";
                else cout << " ";

                if (i < levelNodes.size() - 1) {
                    for (int j = 0; j < spacesBetween - 2; j++) cout << " ";
                }
            }
            cout << endl;
        }

        level++;
    }
}
