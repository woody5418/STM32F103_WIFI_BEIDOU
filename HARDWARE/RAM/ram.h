#ifndef __MALLOC_H_
#define __MALLOC_H_

#include "stm32f10x.h"

//���������ڴ��
#define SRAMIN	 0	//�ڲ��ڴ��

#define SRAMBANK 	1	//����֧�ֵ�SRAM����.

//mem1�ڴ�����趨.mem1��ȫ�����ڲ�SRAM����.
#define MEM1_BLOCK_SIZE			32  	  													//�ڴ���СΪ32�ֽ�
#define MEM1_MAX_SIZE			5*1024  													//�������ڴ� 5K
#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE/MEM1_BLOCK_SIZE 	//�ڴ���С

//�ڴ���������
struct _m_malloc_dev {
    void (*init)(u8);																						//��ʼ��
    u8 (*perused)(u8);		  	    															//�ڴ�ʹ����
    u8 	*membase[SRAMBANK];																			//�ڴ�� ����SRAMBANK��������ڴ�
    u16 *memmap[SRAMBANK]; 																			//�ڴ����״̬��
    u8  memrdy[SRAMBANK]; 																			//�ڴ�����Ƿ����
};
extern struct _m_malloc_dev malloc_dev;	 //��mallco.c���涨��

void mymemcpy(void *des,void *src,u32 n);
void mymemset(void *s,u8 c,u32 count);
void init_my_mem(u8 memx);
u8 my_mem_perused(u8 memx);
u32 my_mem_malloc(u8 memx,u32 size);
u8 my_mem_free(u8 memx,u32 offset);

void myfree(u8 memx,void *ptr);
void *mymalloc(u8 memx,u32 size);
void *myrealloc(u8 memx,void *ptr,u32 size);

#endif
