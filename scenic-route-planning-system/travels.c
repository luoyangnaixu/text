#include "travels.h"

/**
 * @brief 全局DFS遍历，存储遍历序列到path数组
 */
void DFSTraverse(ALGraph graph)
{
    int i = 0;
    pathIdx = 0;
    memset(visited, 0, sizeof(visited));
    if(graph.nodenum <= 0) return;
    for(i = 0; i < graph.nodenum; i++)
    {
        if(!visited[i] && graph.roadlist[i].isDel == 0)
        {
            DFS(graph, i);
        }
    }
}

void DFS(ALGraph graph, int i)
{
    if(pathIdx >= MAXNUM) return;
    if(i < 0 || i >= graph.nodenum || graph.roadlist[i].isDel) return;
    visited[i] = 1;
    strcpy(path[pathIdx++], graph.roadlist[i].data);
    // 在DFS内部局部定义遍历指针p
    CNode *p = graph.roadlist[i].first;
    while(p != NULL)
    {
        int nextIdx = p->index;
        if(nextIdx >= 0 && nextIdx < graph.nodenum && !visited[nextIdx] && graph.roadlist[nextIdx].isDel == 0)
        {
            DFS(graph, nextIdx);
        }
        p = p->next;
    }
}

/**
 * @brief 判断两点是否存在边
 */
int isedg(ALGraph graph, char *e1, char *e2)
{
    int i = locate(graph, e1);
    int j = locate(graph, e2);
    if(i == -1 || j == -1) return 0;
    CNode *pt = graph.roadlist[i].first;
    while(pt != NULL)
    {
        if(pt->index == j) return 1;
        pt = pt->next;
    }
    return 0;
}

/**
 * @brief 获取两点之间道路长度
 */
int getlength(ALGraph graph,int i,int j)
{
    if(i<0 || j<0 || i>=graph.nodenum || j>=graph.nodenum) return 0;
    CNode *pt = graph.roadlist[i].first;
    while(pt != NULL)
    {
        if(pt->index == j) return pt->length;
        pt = pt->next;
    }
    pt = graph.roadlist[j].first;
    while(pt != NULL)
    {
        if(pt->index == i) return pt->length;
        pt = pt->next;
    }
    return 0;
}

/**
 * @brief Floyd多源最短路初始化与松弛
 */
void shortPath(ALGraph graph, int pathMap[][MAXNUM], double shortpath[][MAXNUM])
{
    int i, j, k;
    // 初始化距离与前驱数组
    for(i = 0; i < graph.nodenum; i++)
    {
        for(j = 0; j < graph.nodenum; j++)
        {
            pathMap[i][j] = -1;
            if(i == j)
                shortpath[i][j] = 0;
            else if(isedg(graph, graph.roadlist[i].data, graph.roadlist[j].data))
            {
                shortpath[i][j] = getlength(graph, i, j);
                pathMap[i][j] = i;
            }
            else
                shortpath[i][j] = INF;
        }
    }
    // Floyd三重松弛，跳过已删除节点
    for(k = 0; k < graph.nodenum; k++)
    {
        if(graph.roadlist[k].isDel) continue;
        for(i = 0; i < graph.nodenum; i++)
        {
            if(graph.roadlist[i].isDel) continue;
            for(j = 0; j < graph.nodenum; j++)
            {
                if(graph.roadlist[j].isDel) continue;
                if(shortpath[i][k] != INF && shortpath[k][j] != INF && (shortpath[i][k] + shortpath[k][j] < shortpath[i][j]))
                {
                    shortpath[i][j] = shortpath[i][k] + shortpath[k][j];
                    pathMap[i][j] = pathMap[k][j];
                }
            }
        }
    }
}

/**
 * @brief 迭代输出i到j最短路径，无递归栈溢出风险
 */
void printPath(ALGraph graph, int pathMap[][MAXNUM], double shortpath[][MAXNUM], int i, int j)
{
    if(i == j)
    {
        printf("%s", graph.roadlist[i].data);
        return;
    }
    if(pathMap[i][j] == -1 || shortpath[i][j] >= INF)
    {
        printf("无连通游览路径");
        return;
    }
    int tmp[MAXNUM], cnt = 0;
    int cur = j;
    while(cur != i)
    {
        tmp[cnt++] = cur;
        cur = pathMap[i][cur];
    }
    tmp[cnt++] = i;
    // 逆序打印路线
    for(int k = cnt-1; k >= 0; k--)
    {
        printf("%s", graph.roadlist[tmp[k]].data);
        if(k > 0) printf(" → ");
    }
}

/**
 * @brief 交互查询两点最短路径
 */
void minDistance(ALGraph graph, int pathMap[][MAXNUM], double shortpath[][MAXNUM])
{
    if(graph.nodenum <= 0)
    {
        printf("提示：请先创建景区图！\n");
        return;
    }
    char s1[NAME_LEN], s2[NAME_LEN];
    printf("请输入起点景点名称：");
    scanf(SAFE_SCANF, s1);
    printf("请输入终点景点名称：");
    scanf(SAFE_SCANF, s2);
    int u = locate(graph, s1);
    int v = locate(graph, s2);
    if(u == -1 || v == -1)
    {
        printf("错误：输入景点不存在！\n");
        return;
    }
    sysStat.queryShortCnt++;
    saveSysStat();
    printf("\n最短游览路线：");
    printPath(graph, pathMap, shortpath, u, v);
    if(shortpath[u][v] >= INF)
        printf("\n两点之间无连通道路，无法直达！");
    else
        printf("\n最短总游览距离：%.0lf 单位\n", shortpath[u][v]);
}

static int loopFlag = 0;
static void loopDfs(ALGraph graph, int cur, int parent)
{
    visited[cur] = 1;
    CNode *pt = graph.roadlist[cur].first;
    for(; pt; pt = pt->next)
    {
        int next = pt->index;
        if(graph.roadlist[next].isDel) continue;
        if(!visited[next])
            loopDfs(graph, next, cur);
        else if(next != parent)
        {
            loopFlag = 1;
            return;
        }
    }
}

/**
 * @brief 简单判断图是否存在回路
 */
int islooptest(ALGraph graph)
{
    loopFlag = 0;
    memset(visited, 0, sizeof(visited));
    int i;
    for(i = 0; i < graph.nodenum; i++)
    {
        if(!visited[i] && graph.roadlist[i].isDel == 0)
            loopDfs(graph, i, -1);
        if(loopFlag) break;
    }
    return loopFlag;
}

/**
 * @brief 判断节点是否在当前回溯栈中
 */
int findInStack(int cur, int stackPath[], int top)
{
    for(int i = 0; i < top; i++)
    {
        if(stackPath[i] == cur)
            return i;
    }
    return -1;
}

/**
 * @brief 回路去重：正向/移位/反向全部判定为同一条回路
 */
int isSameLoop(int a[], int la, int b[], int lb)
{
    if(la != lb) return 0;
    // 正向移位匹配
    for(int s = 0; s < la; s++)
    {
        int match = 1;
        for(int i=0;i<la;i++)
        {
            if(a[(s+i)%la] != b[i])
            {
                match = 0;
                break;
            }
        }
        if(match) return 1;
    }
    // 反向移位匹配（A-B-C 与 C-B-A 视为同一回路）
    int rev[MAXNUM];
    for(int i=0;i<la;i++) rev[i] = a[la-1-i];
    for(int s = 0; s < la; s++)
    {
        int match = 1;
        for(int i=0;i<la;i++)
        {
            if(rev[(s+i)%la] != b[i])
            {
                match = 0;
                break;
            }
        }
        if(match) return 1;
    }
    return 0;
}

/**
 * @brief DFS深度搜索全部简单回路
 */
void findLoopDfs(ALGraph g, int cur, int parent, int vis[], int stackPath[], int *stackTop)
{
    if(g.roadlist[cur].isDel) return;
    vis[cur] = 1;
    stackPath[(*stackTop)++] = cur;
    CNode *p = g.roadlist[cur].first;
    while(p != NULL)
    {
        int next = p->index;
        if(g.roadlist[next].isDel)
        {
            p = p->next;
            continue;
        }
        if(next == parent)
        {
            p = p->next;
            continue;
        }
        if(vis[next] == 0)
        {
            findLoopDfs(g, next, cur, vis, stackPath, stackTop);
        }
        else
        {
            int pos = findInStack(next, stackPath, *stackTop);
            if(pos != -1 && loopTotal < LOOP_MAX_CNT)
            {
                int idx = 0;
                for(int i = pos; i < *stackTop; i++)
                {
                    loopList[loopTotal][idx++] = stackPath[i];
                }
                loopList[loopTotal][idx++] = next;
                loopLen[loopTotal] = idx;
                // 去重校验
                int repeat = 0;
                for(int t = 0; t < loopTotal; t++)
                {
                    if(isSameLoop(loopList[t], loopLen[t], loopList[loopTotal], loopLen[loopTotal]))
                    {
                        repeat = 1;
                        break;
                    }
                }
                if(!repeat)
                    loopTotal++;
                else
                    loopLen[loopTotal] = 0;
            }
        }
        p = p->next;
    }
    (*stackTop)--;
    vis[cur] = 0;
}

/**
 * @brief 入口函数：查找并打印全部有效回路(≥3个景点)
 */
void findAllLoop(ALGraph g)
{
    if(g.nodenum <= 0)
    {
        printf("提示：请先创建景区图！\n");
        return;
    }
    memset(loopList, 0, sizeof(loopList));
    memset(loopLen, 0, sizeof(loopLen));
    loopTotal = 0;
    int localStackTop = 0;
    int localStackPath[MAXNUM] = {0};
    int vis[MAXNUM] = {0};
    for(int i = 0; i < g.nodenum; i++)
    {
        if(vis[i] == 0 && g.roadlist[i].isDel == 0)
            findLoopDfs(g, i, -1, vis, localStackPath, &localStackTop);
    }
    sysStat.loopCheckCnt++;
    saveSysStat();
    // 过滤有效回路(节点数≥3)
    int validLoop = 0;
    for(int i=0;i<loopTotal;i++)
    {
        if(loopLen[i] > 2) validLoop++;
    }
    if(validLoop == 0)
    {
        printf("当前景区无有效游览回路（回路至少包含3个景点）\n");
        return;
    }
    printf("\n*******************景区全部回路**********************\n");
    printf("共检测到 %d 条独立游览回路：\n", validLoop);
    int printIdx = 1;
    for(int i = 0; i < loopTotal; i++)
    {
        if(loopLen[i] <= 2) continue;
        printf("第%d条回路：", printIdx++);
        for(int j = 0; j < loopLen[i]; j++)
        {
            printf("%s", g.roadlist[loopList[i][j]].data);
            if(j != loopLen[i] - 1)
                printf(" → ");
        }
        printf("\n");
    }
    printf("**************************************\n");
}

// ====================== Kruskal最小生成树 ======================
typedef struct Edge{
    int u, v, w;
}Edge;

// 边升序排序
int cmpEdge(const void *a, const void *b)
{
    Edge *ea = (Edge*)a;
    Edge *eb = (Edge*)b;
    return ea->w - eb->w;
}

// 并查集
int parent[MAXNUM];
int find(int x)
{
    if(parent[x] != x) parent[x] = find(parent[x]);
    return parent[x];
}
void unite(int x, int y)
{
    x = find(x), y = find(y);
    if(x != y) parent[y] = x;
}

/**
 * @brief Kruskal算法生成最小生成树
 */
void KruskalMST(ALGraph *g)
{
    if(g == NULL || g->nodenum <= 0)
    {
        printf("提示：请先创建景区图！\n");
        return;
    }
    // 统计有效顶点
    int validNodeCnt = 0;
    for(int i=0;i<g->nodenum;i++)
        if(g->roadlist[i].isDel == 0) validNodeCnt++;
    if(validNodeCnt <= 1)
    {
        printf("有效景点不足，无法生成最小生成树！\n");
        return;
    }
    Edge edges[MAXNUM*MAXNUM];
    int edgeCnt = 0;
    int visE[MAXNUM][MAXNUM] = {0};
    // 收集所有无向边，去重
    for(int i=0;i<g->nodenum;i++)
    {
        if(g->roadlist[i].isDel) continue;
        CNode *p = g->roadlist[i].first;
        while(p)
        {
            int j = p->index;
            if(g->roadlist[j].isDel) {p=p->next;continue;}
            if(!visE[i][j])
            {
                edges[edgeCnt++] = (Edge){i,j,p->length};
                visE[i][j] = visE[j][i] = 1;
            }
            p = p->next;
        }
    }
    // 初始化并查集
    for(int i=0;i<g->nodenum;i++) parent[i] = i;
    qsort(edges, edgeCnt, sizeof(Edge), cmpEdge);
    Edge mst[MAXNUM];
    int mstCnt = 0;
    int totalLen = 0;
    for(int i=0;i<edgeCnt;i++)
    {
        int u = edges[i].u, v = edges[i].v, w = edges[i].w;
        if(find(u) != find(v))
        {
            unite(u, v);
            mst[mstCnt++] = edges[i];
            totalLen += w;
            if(mstCnt == validNodeCnt - 1) break;
        }
    }
    if(mstCnt < validNodeCnt - 1)
    {
        printf("警告：景区图不连通，无法生成覆盖全部景点的最小生成树！\n");
        return;
    }
    printf("\n********************景区最小生成树********************\n");
    printf("连通全部景点最短总道路长度：%d\n", totalLen);
    for(int i=0;i<mstCnt;i++)
    {
        int u = mst[i].u, v = mst[i].v, w = mst[i].w;
        printf("%s ←→ %s  道路距离：%d\n", g->roadlist[u].data, g->roadlist[v].data, w);
    }
    printf("***************************************************************\n");
}
