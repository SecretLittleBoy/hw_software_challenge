#include <iostream>
#include<cmath>
#include "distance.h"
#include<vector>
#include <algorithm>
using namespace std;
char line[1024];
int money;
extern float Distance[100][100];
extern bool isSellWorkbench[10][10];
int amountOfWorkbench;
struct nearWorkbench {
    int ID;
    float distance;
};
struct workbench {
    //int ID;ID用数组序号表示
    int type;//范围[1, 9]
    float x;
    float y;//坐标 2 个浮点 x,y
    int indexX;
    int indexY;//从0开始
    int frameLeft;//剩余生产时间（帧数） -1：表示没有生产。0：表示生产因输出格满而阻塞。>=0：表示剩余生产帧数。
    int material;//二进制位表描述，例如 48(110000)表示拥有物品 4 和 5。
    int target;//是否被作为了robot的抵达目的地。二进制位表描述，例如 48(110000)表示物品4和5，被预定了。
    int product;
    vector<nearWorkbench> nearestSellWorkbench;
    vector<nearWorkbench> nearestWorkbench;
};
struct robot {
    int inWhichWorkbench;//-1：表示当前没有处于任何工作台附近  [0,工作台总数-1] ：表示某工作台的下标，从 0 开始，按输入顺序定。当前机器人的所有购买、出售行为均针对该工作台进行。
    int productInHand;//携带物品类型 范围[0,7]。 0 表示未携带物品。 1-7 表示对应物品。
    float timeValue; //携带物品时为[0.8, 1]的浮点数，不携带物品时为 0
    float crushValue; //携带物品时为[0.8, 1]的浮点数，不携带物品时为 0。
    float angularVelocity; //角速度 单位：弧度/秒。正数：表示逆时针。负数：表示顺时针。
    float linearVelocityX;
    float linearVelocityY; //线速度由二维向量描述线速度，单位：米/秒
    float orientation; //弧度，范围[-π,π]。方向示例：0：表示右方向。π/2：表示上方向。-π/2：表示下方向。
    float x;
    float y; //坐标
    int indexX;
    int indexY;//从0开始
    vector<int> target;
};
robot robots[4];
workbench workbenchs[50];
bool calculateDistance() {
    for (int i = 0; i < amountOfWorkbench; i++) {
        for (int j = 0; j < amountOfWorkbench; j++) {
            if (j == i) continue;
            nearWorkbench nb;
            nb.ID = j;
            nb.distance = Distance[abs(workbenchs[i].indexX- workbenchs[j].indexX)] [abs(workbenchs[i].indexY - workbenchs[j].indexY)];
            workbenchs[i].nearestWorkbench.push_back(nb);
            if(isSellWorkbench[i][j]) workbenchs[i].nearestSellWorkbench.push_back(nb);
        }
        std::sort(workbenchs[i].nearestWorkbench.begin(),
            workbenchs[i].nearestWorkbench.end(),
            [](const nearWorkbench& a, const nearWorkbench& b) {
                return a.distance < b.distance;
            });
    }
    return 1;
}
bool readMapUntilOK() {
    int robotIndex = 0;
    int workBenchIndex = 0;
    int indexX = 0, indexY = 99;
    char whatInMap;
    while ((whatInMap = getchar()) != 'O') {
        if (whatInMap == 'A') {
            robots[robotIndex].indexX = indexX;
            robots[robotIndex].indexY = indexY;
            robots[robotIndex].x = 0.25 + indexX * 0.5;
            robots[robotIndex].y = 0.25 + indexY * 0.5;
            robotIndex++;
        }
        else if (whatInMap >= '1' && whatInMap <= '9') {
            workbenchs[workBenchIndex].indexX = indexX;
            workbenchs[workBenchIndex].indexY = indexY;
            workbenchs[workBenchIndex].x = 0.25 + indexX * 0.5;
            workbenchs[workBenchIndex].x = 0.25 + indexY * 0.5;
            workbenchs[workBenchIndex].type = whatInMap - '0';
            workBenchIndex++;
            amountOfWorkbench++;
        }
        if (whatInMap == '\n') {
            indexX = 0;
            indexY--;
        }
        else {
            indexX++;
        }
    }
    getchar();
    getchar();
    calculateDistance();
    puts("OK");
    fflush(stdout);
    return 1;
}
bool readFrameUntilOK() {
    scanf("%d", &money);
    scanf("%d", &amountOfWorkbench);
    for (int i = 0; i < amountOfWorkbench; i++) {
        scanf("%d %f %f %d %d %d", &((workbenchs + i)->type), 
            &((workbenchs + i)->x), &((workbenchs + i)->y),
            &((workbenchs + i)->frameLeft), &((workbenchs + i)->material), &((workbenchs + i)->product));
    }
    for (int i = 0; i < 4; i++) {
        scanf("%d %d %f %f %f %f %f %f %f %f", &((robots + i)->inWhichWorkbench), &((robots + i)->productInHand),
            &((robots + i)->timeValue), &((robots + i)->crushValue),
            &((robots + i)->angularVelocity), &((robots + i)->linearVelocityX), & ((robots + i)->linearVelocityY),
            & ((robots + i)->orientation), & ((robots + i)->x), & ((robots + i)->y) );
        robots[i].indexX = (robots[i].x - 0.25) * 2;
        robots[i].indexY = (robots[i].y - 0.25) * 2;
    }
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == 'O' && line[1] == 'K') {
            return true;
        }
    }
    return 1;
}
//bool readUntilOK() {
//    char line[1024];
//    while (fgets(line, sizeof line, stdin)) {
//        if (line[0] == 'O' && line[1] == 'K') {
//            return true;
//        }
//        //do something
//    }
//    return false;
//}
void findTargetForRobots() {

    //为每个robot找到目标
    for (int i = 0; i < 4; i++) {
        if (!robots[i].target.empty()) continue;
        int target1 = 0;int target2 = 0;
        int minDistance = 70;
        bool haveproduct = 0;
        for (int j = 0 ; j < amountOfWorkbench; j++) {
            if (workbenchs[j].target & (1 << workbenchs[j].type)) continue; //已预定则跳过
            if (haveproduct) {
                if (workbenchs[j].product) {
                    int whichSellEmpty = -1;
                    for (int whichSellEmpty_i = 0; whichSellEmpty_i < workbenchs[j].nearestSellWorkbench.size(); whichSellEmpty_i++) { //找销售方
                        if ((workbenchs[(workbenchs[j].nearestSellWorkbench[whichSellEmpty_i].ID)].material & (1 << workbenchs[j].type)) == 0 &&
                            (workbenchs[(workbenchs[j].nearestSellWorkbench[whichSellEmpty_i].ID)].target & (1 << workbenchs[j].type)) == 0) { //销售方无原材料且原材料未被预定，则
                            whichSellEmpty = whichSellEmpty_i;
                            break;
                        }
                    }
                    if (whichSellEmpty == -1) continue;
                    int tempDistance = Distance[abs(robots[i].indexX - workbenchs[j].indexX)][abs(robots[i].indexY - workbenchs[j].indexY)] + workbenchs[j].nearestSellWorkbench[whichSellEmpty].distance;
                    if (tempDistance < minDistance) {
                        minDistance = tempDistance;
                        target1 = j;
                        target2 = workbenchs[j].nearestSellWorkbench[whichSellEmpty].ID;
                    }
                }
            }
            else {
                if (workbenchs[j].frameLeft >= 0 || workbenchs[j].product) {
                    if (workbenchs[j].product) haveproduct = 1;
                    int whichSellEmpty = -1;
                    for (int whichSellEmpty_i = 0; whichSellEmpty_i < workbenchs[j].nearestSellWorkbench.size(); whichSellEmpty_i++) { //找销售方
                        if ((workbenchs[(workbenchs[j].nearestSellWorkbench[whichSellEmpty_i].ID)].material & (1 << workbenchs[j].type)) == 0 &&
                            (workbenchs[(workbenchs[j].nearestSellWorkbench[whichSellEmpty_i].ID)].target & (1 << workbenchs[j].type)) == 0) { //销售方无原材料且原材料未被预定，则
                            whichSellEmpty = whichSellEmpty_i;
                            break;
                        }
                    }
                    if (whichSellEmpty == -1) continue;
                    int tempDistance = Distance[abs(robots[i].indexX - workbenchs[j].indexX)][abs(robots[i].indexY - workbenchs[j].indexY)] + workbenchs[j].nearestSellWorkbench[whichSellEmpty].distance;
                    if (tempDistance < minDistance) {
                        minDistance = tempDistance;
                        target1 = j;
                        target2 = workbenchs[j].nearestSellWorkbench[whichSellEmpty].ID;
                    }
                }

            }
        }
        workbenchs[target1].target |= (1 << workbenchs[target1].type);//预定
        workbenchs[target2].target |= (1 << workbenchs[target1].type);
        robots[i].target.push_back(target1); robots[i].target.push_back(target2);
    }
}

bool outputCommand(int _frameID) {
    printf("%d\n", _frameID);

    //买卖
    for (int i = 0; i < 4; i++) { 
        if (robots[i].inWhichWorkbench < 0) continue;
        if (workbenchs[robots[i].inWhichWorkbench].type == robots[i].productInHand) {
            printf("sell %d\n", i);
            robots[i].target.clear();
        }
        if (workbenchs[robots[i].inWhichWorkbench].product|| robots[i].productInHand==0) {
            printf("buy %d\n", i);
            for (int whichSellEmpty_i = 0; whichSellEmpty_i < workbenchs[robots[i].inWhichWorkbench].nearestSellWorkbench.size(); whichSellEmpty_i++) { //找销售方
                if ((workbenchs[(workbenchs[robots[i].inWhichWorkbench].nearestSellWorkbench[whichSellEmpty_i].ID)].material & (1 << workbenchs[robots[i].inWhichWorkbench].type)) == 0 &&
                    (workbenchs[(workbenchs[robots[i].inWhichWorkbench].nearestSellWorkbench[whichSellEmpty_i].ID)].target & (1 << workbenchs[robots[i].inWhichWorkbench].type)) == 0) { //销售方无原材料且原材料未被预定，则
                    robots[i].target.push_back(workbenchs[robots[i].inWhichWorkbench].nearestSellWorkbench[whichSellEmpty_i].ID);
                    break;
                }
            }
            if(robots[i].target.empty())    robots[i].target.push_back(workbenchs[robots[i].inWhichWorkbench].nearestSellWorkbench[0].ID);
        }
    }

    findTargetForRobots();

    for (int robotId = 0; robotId < 4; robotId++) {
        float targetOritation;
        if (workbenchs[robots[robotId].target[0]].x - robots[robotId].x == 0) {
            targetOritation = ((workbenchs[robots[robotId].target[0]].y > robots[robotId].y) ? (3.1415926 / 2) : ( - 3.1415926 / 2) );
        }
        else {
            targetOritation = atan((workbenchs[robots[robotId].target[0]].y - robots[robotId].y) / (workbenchs[robots[robotId].target[0]].x - robots[robotId].x));
            if (workbenchs[robots[robotId].target[0]].x < robots[robotId].x) {
                if (targetOritation < 0)
                    targetOritation = -3.1515926 - targetOritation;
                else
                    targetOritation = 3.1515926 - targetOritation;
            }
        }
        double angleSpeed = (targetOritation - robots[robotId].orientation) * 20;
        float lineSpeed = Distance[abs(robots[robotId].indexX- workbenchs[robots[robotId].target[0]].indexX)] [abs(robots[robotId].indexY - workbenchs[robots[robotId].target[0]].indexY)];
        if (lineSpeed >= 0 && lineSpeed < 0.5) {
            lineSpeed = 0.5;
        }
        else if (lineSpeed < 0 && lineSpeed > -0.5) {
            lineSpeed = -0.5;
        }
        printf("forward %d %f\n", robotId, 0.5); //lineSpeed  测试
        printf("rotate %d %f\n", robotId, angleSpeed);
        //fprintf(stderr, "forward %d %d\n", robotId, lineSpeed);
        //fprintf(stderr, "rotate %d %f\n", robotId, angleSpeed);
    }
    //////////////////////////////////
    int debugi = 1;
    if (!robots[debugi].target.empty()) {
        fprintf(stderr, "target %d type %d", robots[debugi].target[0],workbenchs[robots[debugi].target[0]].type);
        if(robots[debugi].target.size()>1) fprintf(stderr, "    target %d type %d", robots[debugi].target[1], workbenchs[robots[debugi].target[1]].type);
        fprintf(stderr, "\n");
        fprintf(stderr, "x y %f %f\n", robots[debugi].x, robots[debugi].y);
    }
    //////////////////////////////////
    printf("OK\n");
    fflush(stdout);
    return 1;
}

int main() {
    readMapUntilOK();
    int frameID;
    while (scanf("%d", &frameID) != EOF) {
        readFrameUntilOK();
        outputCommand(frameID);
    }
    return 0;
}
