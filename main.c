//
//  main.c
//  COVID-19_3.3
//
//  Created by 好好学习 on 2021/3/3.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include "function.h"

//参数设置：

#define INFECT_RATE 1     //在无任何防护措施下近距离接触后的感染几率

//医疗水平
#define CURE_DAY 30         //治愈周期及潜伏周期
#define DISCOVER_DAY 14

//防范意识
#define MASK_RATE 0       //人群中戴口罩的比例以及口罩的效用
#define MASK_ABILITY 0.9

#define SOCIAL_DIS 0        //距离10米以上则无法通过人传人传播
#define DIS_ABILITY 0.1

#define STAY_HOME = 0       //确诊患者居家隔离

//城市属性
#define THRESHOLD 0.5       //失控阀值
#define SIZE 20            //城市大小（视为以Size为边长的正方形）

typedef struct{     //人的属性包括：位置、是否戴口罩
    int Site[2];
    int Mask;
} People;

typedef struct PNode{   //以一个人为一个结点建立链表
    People people ;
    struct PNode *prior;
    struct PNode *next;
}PNode, *PLinkList;

//函数声明
void SetList(PNode **p, PNode **r, PLinkList *head, int num); //建立链表
void Update_property(PNode *healthy_p, PLinkList healthy, int healthy_num); //更新人群属性
void Infect(PNode **diag_p,      PNode **diag_r,     PLinkList diag,     int *diag_num,
            PNode **healthy_p,                       PLinkList *healthy, int *healthy_num); //传播病毒

int main(int argc, const char * argv[]) {
    
    //定义链表指针
    PLinkList Healthy = NULL, Diag = NULL, Hide = NULL; //健康链表、确诊链表、潜伏期链表的头指针
    PNode   *Healthy_p  = NULL,     *Healthy_r  = NULL,
            *Diag_p     = NULL,     *Diag_r     = NULL,
            *Hide_p     = NULL,     *Hide_r     = NULL;   //各链表的工作指针和尾指针
    
    //初始病情
    int Healthy_num = 20000, Diag_num = 4000, Hide_num = 100;                    //健康人数、确诊人数、潜伏人数
    int Sickbed_num = 100, Bed_full = 0, Bed_empty = Sickbed_num - Bed_full;    //隔离病床数
    
    
    //建立三类人群的链表
    SetList(&Healthy_p, &Healthy_r, &Healthy, Healthy_num); //建立健康人群链表
    SetList(&Diag_p, &Diag_r, &Diag, Diag_num);             //建立确诊患者链表
    SetList(&Hide_p, &Hide_r, &Hide, Hide_num);             //建立潜伏期患者链表
    
    //统计感染情况
    int All_people = Healthy_num + Diag_num + Hide_num, Sick_people = Diag_num + Hide_num; //患者数量情况
    
    int day = 0;
    
    while ((Sick_people < All_people * THRESHOLD) && (Sick_people != 0)){
        
        day++;
        
        //更新病床及在外确诊患者数量：

        //每一个潜伏周期有4/5的潜伏病人被确诊
        int Hide_num_delt = Hide_num * (1 - pow(0.2, 1.0 / DISCOVER_DAY));
        Diag_num += Hide_num_delt;
        Hide_num -= Hide_num_delt;
        for (int i = 0; i < Hide_num_delt; i++){ //将确诊患者移入确诊队列
            Diag_r->next = Hide;
            Hide = Hide->next;
            Hide->prior = NULL;
            Diag_r->next->prior = Diag_r;
            Diag_r = Diag_r->next;
            Diag_r->next = NULL;
        }

        //每一个治愈周期有1/2的在院病人被治愈
        Bed_full *= pow(0.5, 1.0 / CURE_DAY);
        Bed_empty = Sickbed_num - Bed_full;
        if (Diag_num <= Bed_empty){                     //确诊病人进入医院隔离病房
            Bed_full += Diag_num;
            Bed_empty -= Diag_num;
            for (int i = 0; i < Diag_num; i++){        //进入隔离病房后痊愈的患者将获得免疫力，不再进入模型中讨论
                PNode *temp = Diag;
                Diag = Diag->next;
                free(temp);
            }
            Diag_num = 0;
        } else{
            for (int i = 0; i < Bed_empty; i++){
                PNode *temp = Diag;
                Diag = Diag->next;
                free(temp);
            }
            Diag_num -= Bed_empty;
            Bed_full += Bed_empty;
            Bed_empty = 0;
        }
        
        //更新人群属性
        Update_property(Healthy_p, Healthy, Healthy_num);  //更新健康人群属性
        Update_property(Diag_p, Diag, Diag_num);           //更新确诊患者属性
        Update_property(Hide_p, Hide, Hide_num);           //更新潜伏期患者属性
        
        //传播过程
        Infect(&Diag_p, &Diag_r, Diag, &Diag_num, &Healthy_p, &Healthy, &Healthy_num);
        Infect(&Hide_p, &Hide_r, Hide, &Hide_num, &Healthy_p, &Healthy, &Healthy_num);
        
        All_people = Healthy_num + Diag_num + Hide_num;
        int delt = Diag_num + Hide_num - Sick_people;
        Sick_people = Diag_num + Hide_num;
        printf("第%d\t天新增感染人数%d\n", day, delt);
        printf("第%d天外在病人数：%d\n", day, Sick_people);
    }
    if (Sick_people == 0) {
        printf("非医院病人已全部排查完毕，");
        printf("病毒在%d天内被消灭！\n", day);
    }
    else printf("病毒在%d天内失去控制！\n", day);
    
    
    return 0;
}

//建立链表函数：
void SetList(PNode **p, PNode **r, PLinkList *head, int num){
    
    *p = (PNode*)malloc(sizeof(PNode));     //建立第一个结点
    *head = *p;
    *r = *p;
    (*p)->prior = NULL;
    (*p)->next  = NULL;
    for (int i = 1; i < num; i++){          //循环建立剩下的num-1个结点
        *p = (PNode*)malloc(sizeof(PNode));
        (*r)->next = *p;
        (*p)->prior = *r;
        *r = *p;
    }
    (*r)->next = NULL;                      //尾指针指向NULL
    
}

//更新人群属性函数：
void Update_property(PNode *healthy_p, PLinkList healthy, int healthy_num){
    healthy_p = healthy;
    for (int i = 0; i < healthy_num; i++){
        if (i < healthy_num * MASK_RATE) {
            ((healthy_p)->people).Mask = 1;
        } else{
            ((healthy_p)->people).Mask = 0;
        }
        ((healthy_p)->people).Site[0] = rand() % SIZE;
        ((healthy_p)->people).Site[1] = rand() % SIZE;
    }
}

//感染函数
void Infect(PNode **diag_p,      PNode **diag_r,     PLinkList diag,     int *diag_num,
            PNode **healthy_p,                       PLinkList *healthy,  int *healthy_num){
    int delt = 0; //记录新增感染量
    *diag_p = diag; *healthy_p = *healthy; //从头开始遍历
    for(int i = 0; i < *diag_num; i++){
        for (int j = 0; j < *healthy_num && healthy_p; j++){
            double Dis0 =abs((*diag_p)->people.Site[0] - (*healthy_p)->people.Site[0]);
            double Dis1 =abs((*diag_p)->people.Site[1] - (*healthy_p)->people.Site[1]);
            double Dis = sqrt(pow(Dis0, 2) + pow(Dis1, 2)) + SOCIAL_DIS; //人与人之间的距离等于位置差加上社交距离
            if (Dis * DIS_ABILITY < 1){ //距离小于10则无法传播
                double Odds = (1 - MASK_ABILITY * (*diag_p)->people.Mask) * (1 - MASK_ABILITY * (*healthy_p)->people.Mask) *
                              (1 - Dis * DIS_ABILITY) * INFECT_RATE;
                if ((rand()%1000)/1000.0 < Odds){
                    delt++;
                    (*diag_r)->next = *healthy_p;
                    if (*healthy_p == *healthy){
                        *healthy = (*healthy)->next;
                    }
                    if ((*healthy_p)->prior){
                        (*healthy_p)->prior->next = (*healthy_p)->next;
                    }
                    if ((*healthy_p)->next){
                        (*healthy_p)->next->prior = (*healthy_p)->prior;
                    }
                    *healthy_p = (*healthy_p)->next;
                    (*diag_r)->next->prior = *diag_r;
                    *diag_r = (*diag_r)->next;
                }
            }
        }
    }
    *diag_num += delt; *healthy_num -= delt;
    (*diag_r)->next = NULL;
}
