#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <math.h>
#include <time.h>
#include <complex.h>
#include "mkl.h"
#include "thread_pool.h"
#include "srslte/fec/cbsegm.h"
#include "TaskScheduler.h"
#include "config.h"

#include "Cpp_fun.h"

#define fifo_max_uint 20 



const int threadNum_tx = 1; // 发送端线程数
const int threadNum_rx = 1; // 接收端线程数
const int upper_layer  = 1; // 上层线程数

/* 声明互斥锁 */
pthread_mutex_t mutex1_tx;
pthread_mutex_t mutex2_tx;
pthread_mutex_t mutex1_rx;
pthread_mutex_t mutex2_rx;
pthread_mutex_t mutex3_rx;

/* 声明信号量 */
sem_t tx_can_be_destroyed;
sem_t rx_can_be_destroyed;
sem_t tx_buff_can_be_destroyed;
sem_t rx_buff_can_be_destroyed;
sem_t tx_prepared;
sem_t rx_prepared;
sem_t tx_buff_prepared;
sem_t rx_buff_prepared;
sem_t cache_tx;
sem_t cache_rx;
sem_t buffisnotEmpty;

// /* 用于交互的全局变量 */
// uint8_t (*write_I)[swap_buffer_size];  //写指针
// uint8_t (*read_I)[swap_buffer_size];   //读指针
// uint8_t (*buffer)[swap_buffer_size];   //缓冲区
// //uint8_t *fifo_ptr[fifo_max_uint]; //指向每一块存储区的指针数组
// sem_t  write_en;   //可写信号量
// sem_t  read_en;    //可读信号量
// pthread_mutex_t swap_lock; 
// uint32_t unread_buffer;

int main()
{
	/* 初始化交互缓冲内容 */
	int fifo_mac_phy = swap_buffer_init();
	printf("fifo_mac_phy is %d.\n",fifo_mac_phy);
	int fifo_phy_mac = swap_buffer_init();
	printf("fifo_phy_mac is %d.\n",fifo_phy_mac);
	int fifo_rlc_mac = swap_buffer_init();
	printf("fifo_rlc_mac is %d.\n",fifo_rlc_mac);
	sleep(5);
	/* 初始化互斥锁 */
	pthread_mutex_init(&mutex1_tx, NULL);
	pthread_mutex_init(&mutex2_tx, NULL);
	pthread_mutex_init(&mutex1_rx, NULL);
	pthread_mutex_init(&mutex2_rx, NULL);
	pthread_mutex_init(&mutex3_rx, NULL);

	/* 初始化信号量 */
	sem_init(&tx_can_be_destroyed, 0, 0);
	sem_init(&rx_can_be_destroyed, 0, 0);
	sem_init(&tx_buff_can_be_destroyed, 0, 0);
	sem_init(&rx_buff_can_be_destroyed, 0, 0);
	sem_init(&tx_prepared, 0, 0);
	sem_init(&rx_prepared, 0, 0);
	sem_init(&tx_buff_prepared, 0, 0);
	sem_init(&rx_buff_prepared, 0, 0);
	sem_init(&cache_tx, 0, 0);
	sem_init(&cache_rx, 0, 0);
	sem_init(&buffisnotEmpty, 0, 0);

	/* 初始化线程池 */
	pool_init(0, 1, 0);
	printf("creat pool 0...\n");
	pool_init(1, 1, 1);
	printf("creat pool 1...\n");
	pool_init(2, threadNum_tx, 2);
	printf("creat pool 2...\n");
	pool_init(2 + threadNum_tx, threadNum_rx, 3);
	printf("creat pool 3...\n");
	pool_init(2 + threadNum_tx + threadNum_rx, 1, 4);
	printf("creat pool 4...\n");
	pool_init(3 + threadNum_tx + threadNum_rx, 1, 5);
	printf("creat pool 5...\n");
	pool_init(3 + threadNum_tx + threadNum_rx + upper_layer, 1, 6);
	printf("creat pool 6...\n");

	/* 添加发送端主任务 */
	pool_add_task(TaskScheduler_tx, NULL, 0);
	printf("add Tx TaskScheduler to pool 0...\n");
	/* 添加接收端主任务 */
	pool_add_task(TaskScheduler_rx, NULL, 1);
	printf("add Rx TaskScheduler to pool 1...\n");
	/* 添加发送端缓存任务 */
	pool_add_task(Tx_buff, NULL, 4);
	printf("add Tx Buff to pool 4...\n");
	/* 添加发送端缓存任务 */
	pool_add_task(Rx_buff, NULL, 5);
	printf("add Rx Buff to pool 5...\n");
	/* 添加 */
	pool_add_task(lte_upper_main, NULL, 6);
	printf("add Upper_task to pool 6...\n");

	/* 等待信号销毁线程 */
	sem_wait(&tx_can_be_destroyed);
	pool_destroy(0);
	sem_wait(&rx_can_be_destroyed);
	pool_destroy(1);
	sem_wait(&tx_buff_can_be_destroyed);
	pool_destroy(4);
	sem_wait(&rx_buff_can_be_destroyed);
	pool_destroy(5);

	/* 销毁信号量*/
	sem_destroy(&tx_can_be_destroyed);
	sem_destroy(&rx_can_be_destroyed);
	sem_destroy(&tx_buff_can_be_destroyed);
	sem_destroy(&rx_buff_can_be_destroyed);
	sem_destroy(&tx_prepared);
	sem_destroy(&rx_prepared);
	sem_destroy(&tx_buff_prepared);
	sem_destroy(&rx_buff_prepared);
	sem_destroy(&cache_tx);
	sem_destroy(&cache_rx);
	sem_destroy(&buffisnotEmpty);

	swap_destory();

	return 0;
}
