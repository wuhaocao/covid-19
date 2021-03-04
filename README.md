# covid-19

C语言主程序：
Step1. 建立健康、确诊、潜伏期三个人群链表，链表中存放具有个人属性的结构体，个人属性包括：是否戴口罩、所在位置等

For（病人数小于总人数一半且不为零时）
Step2. 每天以一个确定速率对潜伏期链表（暴露确诊）、确诊链表（住院治疗）进行更	新；
Step3. 每天遍历三个链表对所有人是否佩戴口罩、所处位置进行更新；
Step4. 遍历确诊（/潜伏）链表，对每个结点都遍历一次健康链表，根据双方是否戴口罩、相距距离等参数判断是否对健康链表中的人构成感染；

Step5. 根据消灭病毒（或失去控制）所用的天数，判断防疫措施的效果


可控变量包括：是否戴口罩、所处位置、社交距离、患者是否居家隔离、医院隔离病床数等
