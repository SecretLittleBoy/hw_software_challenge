#include <iostream>
#include "distance.h"
using namespace std;
char line[1024];
int money;
extern float distance[100][100];
int amountOfWorkbench;
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
    int product; //产品放入material一起表示
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
};
robot robots[4];
workbench workbenchs[50];
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
    }
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == 'O' && line[1] == 'K') {
            return true;
        }
    }
    return 1;
}
bool readUntilOK() {
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == 'O' && line[1] == 'K') {
            return true;
        }
        //do something
    }
    return false;
}

int main() {
    readMapUntilOK();
    puts("OK");
    fflush(stdout);
    int frameID;
    while (scanf("%d", &frameID) != EOF) {
        readFrameUntilOK();
        //readUntilOK();
        printf("%d\n", frameID);
        int lineSpeed = 3;
        double angleSpeed = 1.5;
        for (int robotId = 0; robotId < 4; robotId++) {
            printf("forward %d %d\n", robotId, lineSpeed);
            printf("rotate %d %f\n", robotId, angleSpeed);
        }
        printf("OK\n", frameID);
        fflush(stdout);
    }
    return 0;
}
