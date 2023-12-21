#include "TreeTest.h"
#include <iostream>
#include <string>
#include <stack>
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
    fprintf(stdout, "%d ", tree->data);
    PreOrderTraverse01(tree->lchild);
    PreOrderTraverse01(tree->rchild);
}

static void MidOrderTraverse01(BiTree01 tree)
{
    if (tree == nullptr) {
        return;
    }
    MidOrderTraverse01(tree->lchild);
    fprintf(stdout, "%d ", tree->data);
    MidOrderTraverse01(tree->rchild);
}

static void PostOrderTraverse01(BiTree01 tree)
{
    if (tree == nullptr) {
        return;
    }
    PostOrderTraverse01(tree->lchild);
    PostOrderTraverse01(tree->rchild);
    fprintf(stdout, "%d ", tree->data);
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

// input: 1 2 4 0 0 5 0 0 3 6 0 0 7 0 0
void TreeTest01()
{
    BiTree01 tree;
    CreateTree01(&tree);
    PreOrderTraverse01(tree);
    std::fprintf(stdout, "\n");
    MidOrderTraverse01(tree);
    std::fprintf(stdout, "\n");
    PostOrderTraverse01(tree);
    std::fprintf(stdout, "\n");
    DestoryTree01(tree);
}

////////////////////////////////////////////////////////////

typedef struct BiTNode02 {
    int data;
    struct BiTNode02 *lchild;
    struct BiTNode02 *rchild;
} BiTNode02, *BiTree02;

typedef struct StackNode02 {
    BiTree02 p;
    bool flag;
} StackNode02;

static void CreateTree02(BiTree02 *tree)
{
    int num;
    std::cin >> num;
    if (num == 0) {
        *tree = nullptr;
        return;
    }
    *tree = new BiTNode02;
    (*tree)->data = num;
    CreateTree02(&(*tree)->lchild);
    CreateTree02(&(*tree)->rchild);
}

static void PreOrderTraverse02(BiTree02 tree)
{
    std::stack<BiTNode02 *> stack;
    BiTNode02 *p;
    stack.push(tree);
    while (!stack.empty()) {
        p = stack.top();
        stack.pop();
        while (p) {
            std::fprintf(stdout, "%d ", p->data);
            if (p->rchild) {
                stack.push(p->rchild);
            }
            p = p->lchild;
        }
    }
}
/*
 * 方案一：
 * 从根结点开始，不断地遍历当前结点的左子树并将该结点压入栈中，直至不再有左子树。
 * 然后从栈顶取出一个结点并访问它，然后将它的右孩子压入栈中，继续以同样的方式遍历它的右子树。
 */
static void MidOrderTraverse02a(BiTree02 tree)
{
    std::stack<BiTNode02 *> stack;
    BiTNode02 *p;
    stack.push(tree);
    while (!stack.empty()) {
        while ((p = stack.top()) && p) {
            stack.push(p->lchild);
        }
        stack.pop();
        if (!stack.empty()) {
            p = stack.top();
            stack.pop();
            std::fprintf(stdout, "%d ", p->data);
            stack.push(p->rchild);
        }
    }
}

/*
 * 方案二：
 * 和方案一唯一的不同之处在于，压入栈中的只会是每个结点的左孩子，右孩子不需要入栈。
 */
static void MidOrderTraverse02b(BiTree02 tree)
{
    std::stack<BiTNode02 *> stack;
    BiTNode02 *p = tree;
    while (p || !stack.empty()) {
        if (p) {
            stack.push(p);
            p = p->lchild;
        } else {
            p = stack.top();
            stack.pop();
            std::fprintf(stdout, "%d ", p->data);
            p = p->rchild;
        }
    }
}

/*
 * 后序遍历是在遍历完当前结点的左右孩子之后才访问该结点，所以需要在当前结点进栈时为其配备一个标志位。
 * 当遍历该结点的左孩子时，设置当前结点的标志位为0；当要遍历该结点的右孩子时，设置当前结点的标志位为1，进栈。
 * 这样当遍历完该结点的左右子树并将其弹栈时，查看该结点标志位的值：
 * 如果是0，表示该结点的右孩子还没有遍历；如果是1，说明该结点的左右孩子都遍历完成，可以访问此结点。
 */
static void PostOrderTraverse02(BiTree02 tree)
{
    std::stack<StackNode02> stack;
    BiTNode02 *p = tree;
    bool flag;
    StackNode02 snode;
    while (p || !stack.empty()) {
        while (p) {
            snode.p = p;
            snode.flag = false;
            stack.push(snode);
            p = p->lchild;
        }
        snode = stack.top();
        stack.pop();
        p = snode.p;
        flag = snode.flag;
        if (!flag) {
            snode.p = p;
            snode.flag = true;
            stack.push(snode);
            p = p->rchild;
        } else {
            std::fprintf(stdout, "%d ", p->data);
            p = nullptr;
        }
    }
}

static void DestoryTree02(BiTree02 tree)
{
    if (tree == nullptr) {
        return;
    }
    DestoryTree02(tree->lchild);
    DestoryTree02(tree->rchild);
    delete tree;
}

// input: 1 2 4 0 0 5 0 0 3 6 0 0 7 0 0
void TreeTest02()
{
    BiTree02 tree;
    CreateTree02(&tree);
    PreOrderTraverse02(tree);
    std::fprintf(stdout, "\n");
    MidOrderTraverse02a(tree);
    std::fprintf(stdout, "\n");
    MidOrderTraverse02b(tree);
    std::fprintf(stdout, "\n");
    PostOrderTraverse02(tree);
    std::fprintf(stdout, "\n");
    DestoryTree02(tree);
}

////////////////////////////////////////////////////////////
