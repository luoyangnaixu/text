#include "graph.h"
#include "travels.h"

static void dfsPrint(ALGraph g, int cur, int v[])
{
    v[cur] = 1;
    printf("%s", g.roadlist[cur].data);
    CNode *p = g.roadlist[cur].first;
    while(p != NULL)
    {
        int nx = p->index;
        if(!v[nx] && g.roadlist[nx].isDel == 0)
        {
            printf("->");
            dfsPrint(g, nx, v);
        }
        p = p->next;
    }
}

/**
 * @brief 手动创建景区无向图
 */
void createGraph(ALGraph *graph)
{
    if(graph == NULL) return;
    freeGraph(graph);
    int i = 0, k = 0;
    printf("请输入景区景点总数、道路总数（空格分隔）：");
    while(scanf("%d %d", &graph->nodenum, &graph->edgnum) != 2)
    {
        while(getchar() != '\n');
        printf("输入格式错误，请输入两个数字：");
    }
    while(getchar() != '\n');

    // 录入景点名称
    for(i = 0; i < graph->nodenum; i++)
    {
        char name[NAME_LEN];
        printf("请输入第%d个景点名称：", i+1);
        scanf(SAFE_SCANF, name);
        while(getchar() != '\n');
        strcpy(graph->roadlist[i].data, name);
        graph->roadlist[i].first = NULL;
        graph->roadlist[i].isDel = 0;
    }

    int realEdge = 0;
    for(k = 0; k < graph->edgnum; k++)
    {
        int length = 0;
        char c1[NAME_LEN], c2[NAME_LEN];
        printf("请输入第%d条道路：景点1 景点2 长度：", k+1);
        scanf(SAFE_SCANF " " SAFE_SCANF " %d", c1, c2, &length);
        while(getchar() != '\n');
        int u = locate(*graph, c1);
        int v = locate(*graph, c2);
        if(u == -1 || v == -1 || u == v)
        {
            printf("输入景点不存在/不能自连！跳过本条道路\n");
            continue;
        }
        if(length <= 0)
        {
            printf("道路长度必须大于0，跳过本条道路\n");
            continue;
        }
        // 去重判断
        CNode *p = graph->roadlist[u].first;
        int exist = 0;
        while(p)
        {
            if(p->index == v){exist=1;break;}
            p = p->next;
        }
        if(exist)
        {
            printf("该道路已存在，跳过\n");
            continue;
        }
        CNode *n1 = (CNode *)malloc(sizeof(CNode));
        CNode *n2 = (CNode *)malloc(sizeof(CNode));
        if(n1 == NULL || n2 == NULL)
        {
            printf("内存分配失败，本条道路创建失败\n");
            free(n1); free(n2);
            continue;
        }
        n1->index = v; n1->length = length;
        n2->index = u; n2->length = length;
        n1->next = graph->roadlist[u].first;
        graph->roadlist[u].first = n1;
        n2->next = graph->roadlist[v].first;
        graph->roadlist[v].first = n2;
        realEdge++;
    }
    graph->edgnum = realEdge;
    sysStat.createGraphCnt++;
    saveSysStat();
    printf("景区图创建完成，有效道路共%d条！\n", realEdge);
}

/**
 * @brief 打印邻接矩阵，过滤逻辑删除节点
 */
void printGraph(ALGraph *graph)
{
    if(graph == NULL || graph->nodenum <= 0)
    {
        printf("提示：请先创建景区图！\n");
        return;
    }
    // 统计有效未删除顶点
    int validCnt = 0;
    for(int i=0;i<graph->nodenum;i++)
        if(graph->roadlist[i].isDel == 0) validCnt++;
    if(validCnt == 0)
    {
        printf("当前无有效景点！\n");
        return;
    }
    double **parray = (double **)malloc(sizeof(double *) * graph->nodenum);
    for(int i=0;i<graph->nodenum;i++)
    {
        parray[i] = (double *)malloc(sizeof(double) * graph->nodenum);
        for(int j=0;j<graph->nodenum;j++)
        {
            parray[i][j] = (i==j) ? 0 : INF;
        }
    }
    // 填充邻接距离
    for(int i=0;i<graph->nodenum;i++)
    {
        if(graph->roadlist[i].isDel) continue;
        CNode *pt = graph->roadlist[i].first;
        while(pt!=NULL)
        {
            int j = pt->index;
            parray[i][j] = pt->length;
            pt = pt->next;
        }
    }
    // 打印表头
    printf("\n***************景区邻接矩阵****************\n\t");
    for(int i=0;i<graph->nodenum;i++)
    {
        if(graph->roadlist[i].isDel) continue;
        printf("%s\t",graph->roadlist[i].data);
    }
    printf("\n");
    // 逐行打印
    for(int i=0;i<graph->nodenum;i++)
    {
        if(graph->roadlist[i].isDel) continue;
        printf("%s\t",graph->roadlist[i].data);
        for(int j=0;j<graph->nodenum;j++)
        {
            if(graph->roadlist[j].isDel) continue;
            if(parray[i][j] == INF)
                printf("∞\t");
            else
                printf("%.0f\t",parray[i][j]);
        }
        printf("\n");
    }
    printf("**********************************************\n");
    // 释放二维数组
    for(int i=0;i<graph->nodenum;i++)
        free(parray[i]);
    free(parray);
}

/**
 * @brief DFS生成导游游览路线（修复头文件参数不匹配BUG、移除嵌套函数）
 */
void createGuideGraph(ALGraph *graph)
{
    if(graph == NULL || graph->nodenum <= 0)
    {
        printf("提示：请先创建景区图！\n");
        return;
    }
    int vis[MAXNUM] = {0};
    printf("\n**********导游DFS游览路线（连通块分段）*********\n");
    int block = 1;
    for(int i = 0; i < graph->nodenum; i++)
    {
        if(!vis[i] && graph->roadlist[i].isDel == 0)
        {
            printf("【连通区域%d】：", block++);
            dfsPrint(*graph, i, vis);
            printf("\n");
        }
    }
    printf("游览路线生成完毕\n");
}
