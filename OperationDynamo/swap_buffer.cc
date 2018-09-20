#include "swap_buffer.h"

fifo_information::fifo_information()
{
    index = -1;
    uint8_t(*write_I)[swap_buffer_size] = NULL; //写指针
    uint8_t(*read_I)[swap_buffer_size] = NULL;  //读指针
    uint8_t(*buffer)[swap_buffer_size] = NULL;  //缓冲区
    uint32_t unread_buffer = 0;
}

fifo_information::~fifo_information()
{
}

bool fifo_information::swap_buffer_init()
{
    buffer = new uint8_t[fifo_max_uint][swap_buffer_size]; //缓冲区
    write_I = buffer;                                      //写指针
    read_I = buffer;                                       //读指针
    sem_init(&write_en, 0, fifo_max_uint);                 //可写信号量,初始化时为空，所以可写信号量为最大值
    sem_init(&read_en, 0, 0);                              //可读信号量,初始化时为满，所以可读信号量为0
    pthread_mutex_init(&swap_lock, NULL);
    unread_buffer = 0;
}

bool fifo_information::swap_buffer_init(int i)
{
    index = i;
    buffer = new uint8_t[fifo_max_uint][swap_buffer_size]; //缓冲区
    write_I = buffer;                                      //写指针
    read_I = buffer;                                       //读指针
    sem_init(&write_en, 0, fifo_max_uint);                 //可写信号量,初始化时为空，所以可写信号量为最大值
    sem_init(&read_en, 0, 0);                              //可读信号量,初始化时为满，所以可读信号量为0
    pthread_mutex_init(&swap_lock, NULL);
    unread_buffer = 0;
    printf("FIFO NO.%d created!\n", index);
}

bool fifo_information::swap_ifempty()
{
    //return ((!unread_buffer) && (read_I == write_I)); //空：没有数据 且 读写指针相同
    int sval;
    sem_getvalue(&read_en, &sval); //可读信号量为0,则为空
    if (sval == 0)
        return true;
    else
        return false;
}

bool fifo_information::swap_iffull()
{
    //return ((unread_buffer == swap_buffer_size) && (read_I == write_I));
    int sval;
    sem_getvalue(&write_en, &sval);
    if (sval == 0)
        return true; //可写信号量为0，则为满
    else
        return false;
}

bool fifo_information::swap_write_in(uint8_t *source, uint32_t size) //从source将数据存入buffer
{
    static int write_index = 0;
    pthread_mutex_lock(&swap_lock);
    sem_wait(&write_en);
    uint8_t *temp = (uint8_t *)write_I;
    memcpy(temp, &size, sizeof(uint32_t)); //添加上数据大小的头
    memcpy(temp + sizeof(uint32_t), source, size);
    write_index = (write_index + 1) % fifo_max_uint;
    write_I = buffer + write_index;
    sem_post(&read_en);
    pthread_mutex_unlock(&swap_lock);
    return true;
}

bool fifo_information::swap_read_from(uint8_t *dest, uint32_t size) //从buffer把数据读到dest
{
    static int read_index = 0;
    sem_wait(&read_en);
    uint8_t *temp = (uint8_t *)read_I;
    memcpy(dest, temp + sizeof(uint32_t) , size );   //目前没有取那个帧头
    read_index = (read_index + 1) % fifo_max_uint;
    read_I = buffer + read_index;
    sem_post(&write_en);
    return true;
}

bool fifo_information::swap_destory()
{
    delete[] buffer;
    sem_destroy(&write_en);
    sem_destroy(&read_en);
    pthread_mutex_destroy(&swap_lock);
    return true;
}

int fifo_control::fifo_request()
{
    fifo_information new_element;
    int temp = index;
    fifo_pool.push_back(new_element);
    std::vector<fifo_information>::iterator end = (fifo_pool.end() - 1);
    (*end).swap_buffer_init(index);
    ++index;
    ++num_fifo;
    return temp;
}

bool fifo_control::swap_ifempty(int index)
{
    if (index >= fifo_pool.size())
    {
        printf("FIFO:Wrong index！\n");
        return false;
    }
    return fifo_pool[index].swap_ifempty();
}
bool fifo_control::swap_iffull(int index)
{
    if (index >= fifo_pool.size())
    {
        printf("FIFO:Wrong index！\n");
        return false;
    }
    return fifo_pool[index].swap_iffull();
}
bool fifo_control::swap_write_in(int index, uint8_t *source, uint32_t size) //从source将数据存入buffer
{
    if (index >= fifo_pool.size())
    {
        printf("FIFO:Wrong index！\n");
        return false;
    }
    return fifo_pool[index].swap_write_in(source, size);
}
bool fifo_control::swap_read_from(int index, uint8_t *dest, uint32_t size) //从buffer把数据读到dest
{
    if (index >= fifo_pool.size())
    {
        printf("FIFO:Wrong index！\n");
        return false;
    }
    return fifo_pool[index].swap_read_from(dest, size);
}
bool fifo_control::swap_destory(int index) //回收资源
{
    if (index >= fifo_pool.size())
    {
        printf("FIFO:Wrong index！\n");
        return false;
    }
    return fifo_pool[index].swap_destory();
}

bool fifo_control::fifo_destroy()
{
    std::vector<fifo_information>::iterator it = fifo_pool.begin();
    for (; it != fifo_pool.end(); ++it)
    {
        (*it).swap_destory();
    }
    fifo_pool.clear();
    index = 0;
    num_fifo = 0;
}

fifo_control fifo_global; //定义一个用于fifo的全局变量

int swap_buffer_init()
{
    return fifo_global.fifo_request();
}
int swap_ifempty(int index)
{
    return fifo_global.swap_ifempty(index);
}
int swap_iffull(int index)
{
    return fifo_global.swap_iffull(index);
}
int swap_write_in(int index, uint8_t *source, uint32_t size)
{
    return fifo_global.swap_write_in(index, source, size);
}
int swap_read_from(int index, uint8_t *dest, uint32_t size)
{
    return fifo_global.swap_read_from(index, dest, size);
}
int swap_destory()
{
    return fifo_global.fifo_destroy();
}

int swap_header_write_in(int index, uint8_t *source, uint32_t size)
{
    
}

int swap_header_read_from(int index, uint8_t *dest, uint32_t size)
{

}
