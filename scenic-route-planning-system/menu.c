#include "menu.h"
void showMenu()
{
    printf("\n*********************************************************\n");
    printf("            欢迎使用景区路径规划系统           \n\n");
    printf("1  创建景区景点图\n");
    printf("2  打印景区邻接矩阵分布图\n");
    printf("3  DFS自动生成导游游览路线\n");
    printf("4  检测景区全部游览回路\n");
    printf("5  查询两点最短游览路径(Floyd算法)\n");
    printf("6  添加新景点\n");
    printf("7  删除景点(逻辑删除)\n");
    printf("8  修改景点名称\n");
    printf("9  新增景点之间道路\n");
    printf("10 删除景点之间道路\n");
    printf("11 生成景区最小生成树(Kruskal)\n");
    printf("12 保存当前景区数据到scenic_data.txt\n");
    printf("13 从文件加载景区数据\n");
    printf("14 查看系统运行统计信息\n");
    printf("0  退出系统(自动释放内存、保存统计)\n\n");
    printf("***********************************************************\n");
    printf("请输入功能编号：");
}
