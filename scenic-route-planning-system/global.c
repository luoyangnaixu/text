#include "global.h"

// 全局变量定义
int visited[MAXNUM] = {0};
char path[MAXNUM][NAME_LEN] = {0};
int edgnum = 0;
int pathIdx = 0;
int loopList[LOOP_MAX_CNT][LOOP_MAX_LEN] = {0};
int loopLen[LOOP_MAX_CNT] = {0};
int loopTotal = 0;
int stackPath[MAXNUM] = {0};
int stackTop = 0;
SysStat sysStat = {0,0,0,0};

/**
 * @brief 根据景点名称查找顶点下标
 * @return 存在返回下标，不存在返回-1
 */
int locate(ALGraph graph,char * e)
{
    if(e == NULL || strlen(e) == 0) return -1;
    int i=0;
    for(i=0;i<graph.nodenum;i++)
    {
        if(graph.roadlist[i].isDel == 1) continue; // 跳过已删除景点
        if(strcmp(e,graph.roadlist[i].data)==0)
        {
            return i;
        }
    }
    return -1;
}

/**
 * @brief 释放整张邻接表内存，消除内存泄漏
 */
void freeGraph(ALGraph *g)
{
    if(g == NULL) return;
    for(int i=0;i<g->nodenum;i++)
    {
        CNode *p = g->roadlist[i].first;
        while(p != NULL)
        {
            CNode *tmp = p;
            p = p->next;
            free(tmp);
        }
        g->roadlist[i].first = NULL;
    }
    g->nodenum = 0;
    g->edgnum = 0;
}

/**
 * @brief 添加新景点
 */
int addNode(ALGraph *g, char *name)
{
    if(g == NULL || name == NULL) return 0;
    if(g->nodenum >= MAXNUM)
    {
        printf("错误：景点数量已达系统上限%d！\n", MAXNUM);
        return 0;
    }
    if(strlen(name) >= NAME_LEN)
    {
        printf("错误：景点名称过长，最大支持%d位字符\n", NAME_LEN-1);
        return 0;
    }
    if(locate(*g, name) != -1)
    {
        printf("错误：该景点已存在！\n");
        return 0;
    }
    strcpy(g->roadlist[g->nodenum].data, name);
    g->roadlist[g->nodenum].first = NULL;
    g->roadlist[g->nodenum].isDel = 0;
    g->nodenum++;
    printf("成功：景点【%s】添加完成！\n", name);
    return 1;
}

/**
 * @brief 逻辑删除景点，同步删除所有关联道路
 */
int delNode(ALGraph *g, char *name)
{
    if(g == NULL || name == NULL) return 0;
    int idx = locate(*g, name);
    if(idx == -1)
    {
        printf("错误：目标景点不存在！\n");
        return 0;
    }
    // 删除所有关联边
    for(int i=0;i<g->nodenum;i++)
    {
        CNode **p = &g->roadlist[i].first;
        while(*p != NULL)
        {
            if((*p)->index == idx)
            {
                CNode *tmp = *p;
                *p = (*p)->next;
                free(tmp);
                g->edgnum--;
            }else{
                p = &(*p)->next;
            }
        }
    }
    g->roadlist[idx].isDel = 1;
    printf("成功：景点【%s】已删除，关联道路同步清除！\n", name);
    return 1;
}

/**
 * @brief 修改景点名称，重名校验
 */
int modifyNode(ALGraph *g, char *oldName, char *newName)
{
    if(g == NULL || oldName == NULL || newName == NULL) return 0;
    int idx = locate(*g, oldName);
    if(idx == -1)
    {
        printf("错误：原景点不存在！\n");
        return 0;
    }
    if(strlen(newName) >= NAME_LEN)
    {
        printf("错误：新景点名称过长！\n");
        return 0;
    }
    if(locate(*g, newName) != -1)
    {
        printf("错误：新景点名称已被占用！\n");
        return 0;
    }
    strcpy(g->roadlist[idx].data, newName);
    printf("成功：景点名称【%s】修改为【%s】！\n", oldName, newName);
    return 1;
}

/**
 * @brief 添加无向边，校验长度合法性、去重
 */
int addEdge(ALGraph *g, char *n1, char *n2, int len)
{
    if(g == NULL || n1 == NULL || n2 == NULL) return 0;
    int u = locate(*g, n1);
    int v = locate(*g, n2);
    if(u == -1 || v == -1)
    {
        printf("错误：输入景点不存在！\n");
        return 0;
    }
    if(u == v)
    {
        printf("错误：景点不能连接自身！\n");
        return 0;
    }
    if(len <= 0)
    {
        printf("错误：道路长度必须为正整数！\n");
        return 0;
    }
    // 判断边已存在
    CNode *p = g->roadlist[u].first;
    while(p)
    {
        if(p->index == v)
        {
            printf("错误：两点之间道路已存在！\n");
            return 0;
        }
        p = p->next;
    }
    // 内存分配，判空防崩溃
    CNode *n1Node = (CNode *)malloc(sizeof(CNode));
    CNode *n2Node = (CNode *)malloc(sizeof(CNode));
    if(n1Node == NULL || n2Node == NULL)
    {
        printf("错误：内存分配失败，无法创建道路！\n");
        free(n1Node);
        free(n2Node);
        return 0;
    }
    // 构建双向邻接边
    n1Node->index = v; n1Node->length = len; n1Node->next = g->roadlist[u].first;
    g->roadlist[u].first = n1Node;
    n2Node->index = u; n2Node->length = len; n2Node->next = g->roadlist[v].first;
    g->roadlist[v].first = n2Node;
    g->edgnum++;
    printf("成功：【%s】<-->【%s】道路(长度%d)添加完成！\n", n1, n2, len);
    return 1;
}

/**
 * @brief 删除两点之间双向道路
 */
int delEdge(ALGraph *g, char *n1, char *n2)
{
    if(g == NULL || n1 == NULL || n2 == NULL) return 0;
    int u = locate(*g, n1);
    int v = locate(*g, n2);
    if(u == -1 || v == -1)
    {
        printf("错误：景点不存在！\n");
        return 0;
    }
    int flag = 0;
    // 删除u->v
    CNode **p = &g->roadlist[u].first;
    while(*p)
    {
        if((*p)->index == v)
        {
            CNode *tmp = *p;
            *p = (*p)->next;
            free(tmp);
            flag = 1;
            break;
        }
        p = &(*p)->next;
    }
    // 删除v->u
    p = &g->roadlist[v].first;
    while(*p)
    {
        if((*p)->index == u)
        {
            CNode *tmp = *p;
            *p = (*p)->next;
            free(tmp);
            break;
        }
        p = &(*p)->next;
    }
    if(flag)
    {
        g->edgnum--;
        printf("成功：【%s】<-->【%s】道路已删除！\n", n1, n2);
        return 1;
    }else{
        printf("错误：两点之间无连通道路！\n");
        return 0;
    }
}

/**
 * @brief 保存图数据到文件
 */
void saveGraph(ALGraph *g)
{
    if(g == NULL) return;
    FILE *fp = fopen(FILE_NAME, "w");
    if(!fp)
    {
        printf("错误：打开保存文件%s失败！\n", FILE_NAME);
        return;
    }
    fprintf(fp, "%d %d\n", g->nodenum, g->edgnum);
    // 写入顶点
    for(int i=0;i<g->nodenum;i++)
    {
        fprintf(fp, "%s %d\n", g->roadlist[i].data, g->roadlist[i].isDel);
    }
    // 去重存储单向边
    int visEdge[MAXNUM][MAXNUM] = {0};
    for(int i=0;i<g->nodenum;i++)
    {
        CNode *p = g->roadlist[i].first;
        while(p)
        {
            int j = p->index;
            if(!visEdge[i][j])
            {
                fprintf(fp, "%d %d %d\n", i, j, p->length);
                visEdge[i][j] = visEdge[j][i] = 1;
            }
            p = p->next;
        }
    }
    fclose(fp);
    printf("成功：景区数据已持久化至%s\n", FILE_NAME);
}

/**
 * @brief 从文件加载图，重建邻接表
 */
int loadGraph(ALGraph *g)
{
    if(g == NULL) return 0;
    freeGraph(g);
    FILE *fp = fopen(FILE_NAME, "r");
    if(!fp)
    {
        printf("提示：数据文件%s不存在，请先创建景区图！\n", FILE_NAME);
        return 0;
    }
    int n, m;
    fscanf(fp, "%d %d", &n, &m);
    g->nodenum = n;
    g->edgnum = m;
    // 读取顶点
    for(int i=0;i<n;i++)
    {
        fscanf(fp, SAFE_SCANF " %d", g->roadlist[i].data, &g->roadlist[i].isDel);
        g->roadlist[i].first = NULL;
    }
    // 重建双向边
    int u, v, len;
    while(fscanf(fp, "%d %d %d", &u, &v, &len) != EOF)
    {
        CNode *n1 = (CNode *)malloc(sizeof(CNode));
        CNode *n2 = (CNode *)malloc(sizeof(CNode));
        if(n1 == NULL || n2 == NULL)
        {
            printf("警告：加载时内存分配失败，部分道路丢失！\n");
            free(n1); free(n2);
            continue;
        }
        n1->index = v; n1->length = len; n1->next = g->roadlist[u].first;
        g->roadlist[u].first = n1;
        n2->index = u; n2->length = len; n2->next = g->roadlist[v].first;
        g->roadlist[v].first = n2;
    }
    fclose(fp);
    printf("成功：景区数据加载完成！\n");
    return 1;
}

/**
 * @brief 保存系统统计日志
 */
void saveSysStat()
{
    FILE *fp = fopen(LOG_FILE, "w");
    if(!fp) return;
    fprintf(fp, "%d %d %d %d", sysStat.runCount, sysStat.createGraphCnt, sysStat.queryShortCnt, sysStat.loopCheckCnt);
    fclose(fp);
}

/**
 * @brief 加载系统统计，无文件自动置零
 */
void loadSysStat()
{
    FILE *fp = fopen(LOG_FILE, "r");
    if(!fp)
    {
        sysStat = (SysStat){0,0,0,0};
        return;
    }
    fscanf(fp, "%d %d %d %d", &sysStat.runCount, &sysStat.createGraphCnt, &sysStat.queryShortCnt, &sysStat.loopCheckCnt);
    fclose(fp);
}

/**
 * @brief 打印系统运行统计面板
 */
void printSysStat()
{
    printf("\n*****************系统使用统计*****************\n");
    printf("系统累计启动次数：%d\n", sysStat.runCount);
    printf("手动创建景区图次数：%d\n", sysStat.createGraphCnt);
    printf("最短路径查询总次数：%d\n", sysStat.queryShortCnt);
    printf("景区回路检测总次数：%d\n", sysStat.loopCheckCnt);
    printf("************************************************\n");
}
