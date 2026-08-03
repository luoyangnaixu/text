#ifndef TRAVELS_H
#define TRAVELS_H
#include "global.h"

void DFSTraverse(ALGraph graph);
void DFS(ALGraph graph,int i);
int isedg(ALGraph graph,char * e1,char * e2);
int getlength(ALGraph graph,int i,int j);

// Floyd多源最短路
void shortPath(ALGraph graph,int path[][MAXNUM],double shortpath[][MAXNUM]);
void printPath(ALGraph graph,int path[][MAXNUM],double shortpath[][MAXNUM],int i,int j);
void minDistance(ALGraph graph,int path[][MAXNUM],double shortpath[][MAXNUM]);

// 回路检测
int islooptest(ALGraph graph);
void findAllLoop(ALGraph g);

#endif
