#ifndef CPPFUN_H
#define CPPFUN_H

#include <semaphore.h>

#define swap_buffer_size 10240
// /* 用于交互的全局变量 */
// extern uint8_t (*write_I)[swap_buffer_size];  //写指针
// extern uint8_t (*read_I)[swap_buffer_size];   //读指针
// extern uint8_t (*buffer)[swap_buffer_size];   //缓冲区
// //uint8_t *fifo_ptr[fifo_max_uint]; //指向每一块存储区的指针数组
// extern sem_t  write_en;   //可写信号量
// extern sem_t  read_en;    //可读信号量
// extern pthread_mutex_t swap_lock; 
// extern uint32_t unread_buffer;

struct pdu_header
{
  uint32_t value;
  uint32_t sub_frame;
  uint32_t harq_pid;
  uint32_t number;
};

#ifdef __cplusplus
extern "C" {
#endif
extern void lte_upper_main(void *arg);
extern int swap_buffer_init();
extern int swap_ifempty(int index);
extern int swap_iffull(int index);
extern int swap_write_in(int index, uint8_t *source, uint32_t size);
extern int swap_read_from(int index, uint8_t *dest, uint32_t size);
extern int swap_destory();
extern int swap_header_write_in(int index, uint8_t *source, uint32_t size);
extern int swap_header_read_from(int index, uint8_t *dest, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif