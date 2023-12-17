#include "TreeTest.h"
#include <iostream>
#include <string>
#include <stdlib.h>

////////////////////////////////////////////////////////////

#define TElemType00 int

typedef struct BiTNode00 {
    TElemType00 data;
    struct BiTNode00 *lchild;
    struct BiTNode00 *rchild;
} BiTNode00, *BiTree00;

static void CreateTree(BiTree00 *tree)
{
    BiTNode00 *root = (BiTNode00 *)malloc(sizeof(BiTNode00));
    root->data = 1;
    root->lchild = (BiTNode00 *)malloc(sizeof(BiTNode00));
    root->lchild->data = 2;
    root->rchild = (BiTNode00 *)malloc(sizeof(BiTNode00));
    root->rchild->data = 3;
    root->rchild->lchild = nullptr;
    root->rchild->rchild = nullptr;
    root->lchild->lchild = (BiTNode00 *)malloc(sizeof(BiTNode00));
    root->lchild->lchild->data = 4;
    root->lchild->rchild = nullptr;
    root->lchild->lchild->lchild = nullptr;
    root->lchild->lchild->rchild = nullptr;
    *tree = root;
}

static void DestoryTree(BiTree00 tree)
{
    if (tree) {
        DestoryTree(tree->lchild);
        DestoryTree(tree->rchild);
        free(tree);
    }
}

void TreeTest00()
{
    BiTree00 tree;
    CreateTree(&tree);
    std::fprintf(stdout, "lchild data=%d\n", tree->lchild->data);
    std::fprintf(stdout, "rchild data=%d\n", tree->rchild->data);
    DestoryTree(tree);
}

////////////////////////////////////////////////////////////
