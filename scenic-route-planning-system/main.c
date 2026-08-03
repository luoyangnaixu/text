#include "main.h"

int main()
{
    loadSysStat();
    sysStat.runCount++;
    saveSysStat();
    ALGraph graph;
    int path[MAXNUM][MAXNUM];
    double shortpath[MAXNUM][MAXNUM];
    int opt = 0;
    char buf[100];
    // 初始化图
    freeGraph(&graph);
    showMenu();
    while(1)
    {
        printf("请输入功能编号：");
        scanf("%s",buf);
        while(getchar() != '\n');
        // 输入数字校验
        while(1)
        {
            if(!isdigit(buf[0]))
            {
                printf("输入非数字，请重新输入编号：");
                scanf("%s",buf);
                while(getchar() != '\n');
            }
            else
            {
                opt = atoi(buf);
                break;
            }
        }
        switch(opt)
        {
        case 0:
            freeGraph(&graph);
            saveSysStat();
            printf("系统正常退出，所有内存已释放，统计数据已保存！\n");
            return 0;
        case 1:
            createGraph(&graph);
            shortPath(graph,path,shortpath);
            break;
        case 2:
            printGraph(&graph);
            break;
        case 3:
            createGuideGraph(&graph);
            break;
        case 4:
            findAllLoop(graph);
            break;
        case 5:
            minDistance(graph,path,shortpath);
            break;
        // 添加景点
        case 6:{
            char name[NAME_LEN];
            printf("输入新增景点名称：");
            scanf(SAFE_SCANF, name);
            if(addNode(&graph, name))
                shortPath(graph,path,shortpath);
            break;
        }
        // 删除景点
        case 7:{
            char name[NAME_LEN];
            printf("输入要删除的景点名称：");
            scanf(SAFE_SCANF, name);
            if(delNode(&graph, name))
                shortPath(graph,path,shortpath);
            break;
        }
        // 修改景点名
        case 8:{
            char old[NAME_LEN], newName[NAME_LEN];
            printf("输入原景点名称：");
            scanf(SAFE_SCANF, old);
            printf("输入新景点名称：");
            scanf(SAFE_SCANF, newName);
            if(modifyNode(&graph, old, newName))
                shortPath(graph,path,shortpath);
            break;
        }
        // 添加道路
        case 9:{
            char n1[NAME_LEN], n2[NAME_LEN];
            int len;
            printf("输入两个景点和道路长度(空格分隔)：");
            scanf(SAFE_SCANF " " SAFE_SCANF " %d", n1, n2, &len);
            if(addEdge(&graph, n1, n2, len))
                shortPath(graph,path,shortpath);
            break;
        }
        // 删除道路
        case 10:{
            char n1[NAME_LEN], n2[NAME_LEN];
            printf("输入要删除道路的两个景点：");
            scanf(SAFE_SCANF " " SAFE_SCANF, n1, n2);
            if(delEdge(&graph, n1, n2))
                shortPath(graph,path,shortpath);
            break;
        }
        // Kruskal最小生成树
        case 11:
            KruskalMST(&graph);
            break;
        // 保存数据
        case 12:
            saveGraph(&graph);
            break;
        // 加载数据
        case 13:
            if(loadGraph(&graph))
                shortPath(graph,path,shortpath);
            break;
        // 查看统计
        case 14:
            printSysStat();
            break;
        default:
            printf("输入编号不存在，请选择0~14之间的功能！\n");
            break;
        }
        showMenu();
    }
    return 0;
}
