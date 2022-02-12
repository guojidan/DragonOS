//
// 内核全局通用库
// Created by longjin on 2022/1/22.
//

#pragma once

//引入对bool类型的支持
#include <stdbool.h>

#define NULL 0

#define sti() __asm__ __volatile__("sti\n\t" :: \
                                       : "memory") //开启外部中断
#define cli() __asm__ __volatile__("cli\n\t" :: \
                                       : "memory") //关闭外部中断
#define nop() __asm__ __volatile__("nop\n\t")

//内存屏障
#define io_mfence() __asm__ __volatile__("mfence\n\t" :: \
                                             : "memory") // 在mfence指令前的读写操作必须在mfence指令后的读写操作前完成。
#define io_sfence() __asm__ __volatile__("sfence\n\t" :: \
                                             : "memory") // 在sfence指令前的写操作必须在sfence指令后的写操作前完成
#define io_lfence() __asm__ __volatile__("lfence\n\t" :: \
                                             : "memory") // 在lfence指令前的读操作必须在lfence指令后的读操作前完成。
/**
 * @brief 根据结构体变量内某个成员变量member的基地址，计算出该结构体变量的基地址
 * @param ptr 指向结构体变量内的成员变量member的指针
 * @param type 成员变量所在的结构体
 * @param member 成员变量名
 *
 * 方法：使用ptr减去结构体内的偏移，得到结构体变量的基地址
 */
#define container_of(ptr, type, member)                                     \
    ({                                                                      \
        typeof(((type *)0)->member) *p = (ptr);                             \
        (type *)((unsigned long)p - (unsigned long)&(((type *)0)->member)); \
    })

// 定义类型的缩写
typedef unsigned long ul;
typedef unsigned long long int ull;
typedef long long int ll;

#define ABS(x) ((x) > 0 ? (x) : -(x)) // 绝对值

// 四舍五入成整数
ul round(double x)
{
    return (ul)(x + 0.5);
}

//链表数据结构
struct List
{
    struct List *prev, *next;
};

//初始化循环链表
static inline void list_init(struct List *list)
{
    list->next = list;
    list->prev = list;
}

/**
 * @brief

 * @param entry 给定的节点
 * @param node 待插入的节点
 **/
static inline void list_add(struct List *entry, struct List *node)
{

    node->next = entry->next;
    node->prev = entry;
    node->next->prev = node;
    entry->next = node;
}

static inline void list_append(struct List *entry, struct List *node)
{
    /**
     * @brief 将node添加到给定的list的结尾(也就是当前节点的前面)
     * @param entry 列表的入口
     * @param node 待添加的节点
     */

    struct List *tail = entry->prev;
    list_add(tail, node);
}

  void list_add_to_behind(struct List * entry,struct List * new)	////add to entry behind
{
	new->next = entry->next;
	new->prev = entry;
	new->next->prev = new;
	entry->next = new;
}

  void list_add_to_before(struct List * entry,struct List * new)	////add to entry behind
{
	new->next = entry;
	entry->prev->next = new;
	new->prev = entry->prev;
	entry->prev = new;
}

static inline void list_del(struct List *entry)
{
    /**
     * @brief 从列表中删除节点
     * @param entry 待删除的节点
     */

    entry->prev->next = entry->next;
    entry->next = entry->prev;
}

static inline bool list_empty(struct List *entry)
{
    /**
     * @brief 判断循环链表是否为空
     * @param entry 入口
     */

    if (entry->prev == entry->next)
        return true;
    else
        return false;
}

/**
 * @brief 获取链表的上一个元素
 *
 * @param entry
 * @return 链表的上一个元素
 */
static inline struct List *list_prev(struct List *entry)
{
    if (entry->prev != NULL)
        return entry->prev;
    else
        return NULL;
}

/**
 * @brief 获取链表的下一个元素
 *
 * @param entry
 * @return 链表的下一个元素
 */
static inline struct List *list_next(struct List *entry)
{
    if (entry->next != NULL)
        return entry->next;
    else
        return NULL;
}

//计算字符串的长度（经过测试，该版本比采用repne/scasb汇编的运行速度快16.8%左右）
static inline int strlen(char *s)
{
    register int __res = 0;
    while (s[__res] != '\0')
    {
        ++__res;
    }
    return __res;
}

void *memset(void *dst, unsigned char C, ul Count)
{

    int d0, d1;
    unsigned long tmp = C * 0x0101010101010101UL;
    __asm__ __volatile__("cld	\n\t"
                         "rep	\n\t"
                         "stosq	\n\t"
                         "testb	$4, %b3	\n\t"
                         "je	1f	\n\t"
                         "stosl	\n\t"
                         "1:\ttestb	$2, %b3	\n\t"
                         "je	2f\n\t"
                         "stosw	\n\t"
                         "2:\ttestb	$1, %b3	\n\t"
                         "je	3f	\n\t"
                         "stosb	\n\t"
                         "3:	\n\t"
                         : "=&c"(d0), "=&D"(d1)
                         : "a"(tmp), "q"(Count), "0"(Count / 8), "1"(dst)
                         : "memory");
    return dst;
}

/**
 * @brief 内存拷贝函数
 *
 * @param dst 目标数组
 * @param src 源数组
 * @param Num 字节数
 * @return void*
 */
void *memcpy(void *dst, void *src, long Num)
{
    int d0, d1, d2;
    __asm__ __volatile__("cld	\n\t"
                         "rep	\n\t"
                         "movsq	\n\t"
                         "testb	$4,%b4	\n\t"
                         "je	1f	\n\t"
                         "movsl	\n\t"
                         "1:\ttestb	$2,%b4	\n\t"
                         "je	2f	\n\t"
                         "movsw	\n\t"
                         "2:\ttestb	$1,%b4	\n\t"
                         "je	3f	\n\t"
                         "movsb	\n\t"
                         "3:	\n\t"
                         : "=&c"(d0), "=&D"(d1), "=&S"(d2)
                         : "0"(Num / 8), "q"(Num), "1"(dst), "2"(src)
                         : "memory");
    return dst;
}
void *memset_c(void *dst, unsigned char c, ul n)
{
    unsigned char *s = (unsigned char *)dst;
    for (int i = 0; i < n; ++i)
        s[i] = c;
    return dst;
}

// 从io口读入8个bit
unsigned char io_in8(unsigned short port)
{
    unsigned char ret = 0;
    __asm__ __volatile__("inb	%%dx,	%0	\n\t"
                         "mfence			\n\t"
                         : "=a"(ret)
                         : "d"(port)
                         : "memory");
    return ret;
}

// 从io口读入32个bit
unsigned int io_in32(unsigned short port)
{
    unsigned int ret = 0;
    __asm__ __volatile__("inl	%%dx,	%0	\n\t"
                         "mfence			\n\t"
                         : "=a"(ret)
                         : "d"(port)
                         : "memory");
    return ret;
}

// 输出8个bit到输出端口
void io_out8(unsigned short port, unsigned char value)
{
    __asm__ __volatile__("outb	%0,	%%dx	\n\t"
                         "mfence			\n\t"
                         :
                         : "a"(value), "d"(port)
                         : "memory");
}

// 输出32个bit到输出端口
void io_out32(unsigned short port, unsigned int value)
{
    __asm__ __volatile__("outl	%0,	%%dx	\n\t"
                         "mfence			\n\t"
                         :
                         : "a"(value), "d"(port)
                         : "memory");
}


/**
 * @brief 读取rsp寄存器的值（存储了页目录的基地址）
 *
 * @return unsigned*  rsp的值的指针
 */
unsigned long *get_rsp()
{
    ul *tmp;
    __asm__ __volatile__(
        "movq %%rsp, %0\n\t"
        : "=r"(tmp)::"memory");
    return tmp;
}

/**
 * @brief 读取rbp寄存器的值（存储了页目录的基地址）
 *
 * @return unsigned*  rbp的值的指针
 */
unsigned long *get_rbp()
{
    ul *tmp;
    __asm__ __volatile__(
        "movq %%rbp, %0\n\t"
        : "=r"(tmp)::"memory");
    return tmp;
}

/**
 * @brief 读取ds寄存器的值（存储了页目录的基地址）
 *
 * @return unsigned*  ds的值的指针
 */
unsigned long *get_ds()
{
    ul *tmp;
    __asm__ __volatile__(
        "movq %%ds, %0\n\t"
        : "=r"(tmp)::"memory");
    return tmp;
}

/**
 * @brief 读取rax寄存器的值（存储了页目录的基地址）
 *
 * @return unsigned*  rax的值的指针
 */
unsigned long *get_rax()
{
    ul *tmp;
    __asm__ __volatile__(
        "movq %%rax, %0\n\t"
        : "=r"(tmp)::"memory");
    return tmp;
}
/**
 * @brief 读取rbx寄存器的值（存储了页目录的基地址）
 *
 * @return unsigned*  rbx的值的指针
 */
unsigned long *get_rbx()
{
    ul *tmp;
    __asm__ __volatile__(
        "movq %%rbx, %0\n\t"
        : "=r"(tmp)::"memory");
    return tmp;
}