#include "GraphTest.h"
#include <iostream>

////////////////////////////////////////////////////////////

#define MAX_VERTEX_NUM 20
struct Arc00 {
    int adjvex;
    Arc00 *next;
    void *info;
};

struct Vertex00 {
    int data;
    Arc00 *arc;
};

struct Graph00 {
    Vertex00 vertexes[MAX_VERTEX_NUM];
    int vexNum;
    int arcNum;
    int kind;
};

void GraphTest00()
{
    std::cout << std::endl;
}

////////////////////////////////////////////////////////////

void GraphTest01()
{
    std::cout << std::endl;
}

////////////////////////////////////////////////////////////
