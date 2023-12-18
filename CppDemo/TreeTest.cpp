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

static void CreateTree00(BiTree00 *tree)
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

static void DestoryTree00(BiTree00 tree)
{
    if (tree) {
        DestoryTree00(tree->lchild);
        DestoryTree00(tree->rchild);
        free(tree);
    }
}

void TreeTest00()
{
    BiTree00 tree;
    CreateTree00(&tree);
    std::fprintf(stdout, "lchild data=%d\n", tree->lchild->data);
    std::fprintf(stdout, "rchild data=%d\n", tree->rchild->data);
    DestoryTree00(tree);
}

////////////////////////////////////////////////////////////

typedef struct BiTNode01 {
    int data;
    struct BiTNode01 *lchild;
    struct BiTNode01 *rchild;
} BiTNode01, *BiTree01;

static void CreateTree01(BiTree01 *tree)
{
    int num;
    scanf("%d", &num);
    if (num == 0) {
        *tree = nullptr;
        return;
    }
    *tree = (BiTree01)malloc(sizeof(BiTNode01));
    (*tree)->data = num;
    CreateTree01(&(*tree)->lchild);
    CreateTree01(&(*tree)->rchild);
}

static void PreOrderTraverse01(BiTree01 tree)
{
    if (tree == nullptr) {
        return;
    }
    fprintf(stdout, "%d\n", tree->data);
    PreOrderTraverse01(tree->lchild);
    PreOrderTraverse01(tree->rchild);
}

static void DestoryTree01(BiTree01 tree)
{
    if (tree == nullptr) {
        return;
    }
    DestoryTree01(tree->lchild);
    DestoryTree01(tree->rchild);
    free(tree);
}

void TreeTest01()
{
    BiTree01 tree;
    CreateTree01(&tree);
    PreOrderTraverse01(tree);
    DestoryTree01(tree);
}

////////////////////////////////////////////////////////////
