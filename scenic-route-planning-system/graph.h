#ifndef GRAPH_H
#define GRAPH_H
#include "global.h"

// 创建原图邻接表
void createGraph(ALGraph * graph);
// 打印邻接矩阵分布图
void printGraph(ALGraph * graph);
// 生成导游简化路线子图(DFS遍历输出)
void createGuideGraph(ALGraph * graph);

#endif
