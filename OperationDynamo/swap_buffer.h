#ifndef SWAPBUFFER_H
#define SWAPBUFFER_H

#include <stdint.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <vector>
#include <stdio.h>

#define swap_buffer_size 10240
#define fifo_max_uint 20 //FIFO中的最大存储块数

// extern uint8_t (*write_I)[swap_buffer_size]; //写指针
// extern uint8_t (*read_I)[swap_buffer_size];  //读指针
// extern uint8_t (*buffer)[swap_buffer_size];  //缓冲区
// extern sem_t write_en;                       //可写信号量
// extern sem_t read_en;                        //可读信号量
// extern pthread_mutex_t swap_lock;
// extern uint32_t unread_buffer;

class fifo_information
{
  public:
    fifo_information();
    ~fifo_information();
    bool swap_buffer_init();
    bool swap_buffer_init(int i);
    bool swap_ifempty();
    bool swap_iffull();
    bool swap_write_in(uint8_t *source, uint32_t size);
    bool swap_read_from(uint8_t *source, uint32_t size);
    bool swap_destory();

  private:
    int index;
    uint8_t (*write_I)[swap_buffer_size]; //写指针
    uint8_t (*read_I)[swap_buffer_size];  //读指针
    uint8_t (*buffer)[swap_buffer_size];  //缓冲区
    sem_t write_en;                       //可写信号量
    sem_t read_en;                        //可读信号量
    pthread_mutex_t swap_lock;
    uint32_t unread_buffer;
};

class fifo_control
{
  public:
    int fifo_request();
    bool swap_ifempty(int index);
    bool swap_iffull(int index);
    bool swap_write_in(int index, uint8_t *source, uint32_t size); //从source将数据存入buffer
    bool swap_read_from(int index, uint8_t *dest, uint32_t size);  //从buffer把数据读到dest
    bool swap_destory(int index);                                  //回收资源
    bool fifo_destroy();

  private:
    std::vector<fifo_information> fifo_pool;
    int num_fifo = 0;
    int index = 0;
};

// struct pdu_header
// {
//   uint32_t value = 0;
//   uint32_t sub_frame = 0;
//   uint32_t harq_pid = 0;
// };

extern "C" int swap_buffer_init();
extern "C" int swap_ifempty(int index);
extern "C" int swap_iffull(int index);
extern "C" int swap_write_in(int index, uint8_t *source, uint32_t size);
extern "C" int swap_read_from(int index, uint8_t *dest, uint32_t size);
extern "C" int swap_destory();
extern "C" int swap_header_write_in(int index, uint8_t *source, uint32_t size);
extern "C" int swap_header_read_from(int index, uint8_t *dest, uint32_t size);

#endif