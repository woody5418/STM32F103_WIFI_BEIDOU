#include "includes.h"


//ϵͳʱ�ӳ�ʼ��

void NVIC_Configuration(void)
{
    //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}
