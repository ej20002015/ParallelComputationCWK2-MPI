#include <stdio.h>
#include <stdlib.h>

typedef struct TreeNode
{
    int rank;
    struct TreeNode* leftChild;
    struct TreeNode* rightChild;
} TreeNode;

typedef struct BroadcastBinaryTree
{
    int levels;
    TreeNode* root;
} BroadcastBinaryTree;

BroadcastBinaryTree* createBroadcastBinaryTree(int nodeCount);
void printTree(const BroadcastBinaryTree* tree);
void freeTree(BroadcastBinaryTree* tree);