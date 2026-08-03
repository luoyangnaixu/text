#ifndef GLOBAL_H
#define GLOBAL_H
#define MAXNUM 100
#define INF 32767
#define LOOP_MAX_CNT 50
#define LOOP_MAX_LEN 100
#define FILE_NAME "scenic_data.txt" // 景区数据持久化文件
#define LOG_FILE "system_log.txt"   // 系统操作统计日志
#define NAME_LEN 10                 // 景点名称最大长度
#define SAFE_SCANF "%9s"            // 字符串安全读取格式

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

// 邻接表边节点：存储邻接顶点、道路长度、下一条边
typedef struct stCNode{
    int index;        // 邻接顶点下标
    int length;       // 道路距离
    struct stCNode * next;
}CNode;

// 顶点表头节点：景点名称、边链表头、逻辑删除标记
typedef struct stHNode{
    char data[NAME_LEN];
    CNode * first;
    int isDel; // 0正常 1逻辑删除
}HNode,RoadList[MAXNUM];

// 无向图邻接表整体结构
typedef struct stGraph{
    RoadList roadlist;
    int nodenum;  // 当前顶点总数
    int edgnum;   // 当前有效边总数
}ALGraph;

// 系统运行统计结构体
typedef struct SysStat{
    int runCount;        // 系统启动次数
    int createGraphCnt;  // 创建图次数
    int queryShortCnt;   // 查询最短路次数
    int loopCheckCnt;    // 回路查询次数
}SysStat;

// 全局遍历缓存
extern int visited[MAXNUM];
extern char path[MAXNUM][NAME_LEN];
extern int edgnum;
extern int pathIdx;

// 回路存储全局数组
extern int loopList[LOOP_MAX_CNT][LOOP_MAX_LEN];
extern int loopLen[LOOP_MAX_CNT];
extern int loopTotal;
extern int stackPath[MAXNUM];
extern int stackTop;

// 系统统计全局变量
extern SysStat sysStat;

// 基础工具接口
int locate(ALGraph graph,char * e);

// 图内存管理、增删改接口
void freeGraph(ALGraph *g);
int addNode(ALGraph *g, char *name);
int delNode(ALGraph *g, char *name);
int modifyNode(ALGraph *g, char *oldName, char *newName);
int addEdge(ALGraph *g, char *n1, char *n2, int len);
int delEdge(ALGraph *g, char *n1, char *n2);

// 持久化文件接口
void saveGraph(ALGraph *g);
int loadGraph(ALGraph *g);
void saveSysStat();
void loadSysStat();
void printSysStat();

// Kruskal最小生成树
void KruskalMST(ALGraph *g);

#endif
