#include "BroadcastBinaryTree.h"

#include <stdio.h>
#include <stdlib.h>

static void createChildren(TreeNode* node, int currentLevel, int levels);
static void printNode(const TreeNode* node);
static void freeNode(TreeNode* node);

BroadcastBinaryTree* createBroadcastBinaryTree(int processCount)
{
    int isProcessCountAPowerOf2 = processCount && ((processCount & (processCount - 1)) == 0);
    if (!isProcessCountAPowerOf2)
    {
        printf("Cannot construct broadcast binary tree - processCount needs to be a power of 2\n");
        return NULL;
    }

    BroadcastBinaryTree* tree = (BroadcastBinaryTree*)malloc(sizeof(BroadcastBinaryTree));
    TreeNode* root = (TreeNode*)malloc(sizeof(TreeNode));
    root->rank = 0;

    int levels = 1;
    while ((1 << levels) <= processCount)
        levels++;

    createChildren(root, 1, levels);

    tree->root = root;
    tree->levels = levels;

    return tree;
}

void printTree(const BroadcastBinaryTree* tree)
{
    printf("Printing broadcast binary tree with %d levels\n\n", tree->levels);

    printNode(tree->root);
}

static void createChildren(TreeNode* node, int currentLevel, int levels)
{
    if (currentLevel == levels)
    {
        node->leftChild = NULL;
        node->rightChild = NULL;
        return;
    }

    node->leftChild = (TreeNode*)malloc(sizeof(TreeNode));
    node->rightChild = (TreeNode*)malloc(sizeof(TreeNode));

    node->leftChild->rank = node->rank;
    node->rightChild->rank = node->rank + (1 << (currentLevel - 1));

    createChildren(node->leftChild, currentLevel + 1, levels);
    createChildren(node->rightChild, currentLevel + 1, levels);
}

static void printNode(const TreeNode* node)
{
    if (!node)
        return;

    printf("Address of node: %p\n", node);
    printf("\tRank:                   %d\n", node->rank);
    printf("\tAddress of left child:  %p\n", node->leftChild);
    printf("\tAddress of right child: %p\n", node->rightChild);

    printNode(node->leftChild);
    printNode(node->rightChild);
}

void freeTree(BroadcastBinaryTree* tree)
{
    freeNode(tree->root);
    free(tree);
}

static void freeNode(TreeNode* node)
{
    if (!node)
        return;
    
    freeNode(node->leftChild);
    freeNode(node->rightChild);

    free(node);
}