#include "FuncHead.h"

#include "Cpp_fun.h"

#define SEND_SIZE 400

using namespace srslte;
using namespace srsue;

extern pthread_barrier_t barrier;
extern pthread_mutex_t pdu_gets;
extern rlc_um rlc_test[];

extern UE_FX ue_test; //map容器

pthread_t id_test[50]; //预留100可以创建个线程

int test_error[10][8][70] = {};

void tongbu() //用于最开始的对齐
{
    printf("Tong bu!!!\n");
    int port = atoi("4567");
    int st = socket(AF_INET, SOCK_DGRAM, 0);
    int sys = 1;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("10.129.4.106"); //目的实际地址
    sendto(st, &sys, sizeof(int), 0, (struct sockaddr *)&addr, sizeof(addr));
}

void handle(union sigval v) //union sigval v
{
    bool flag;
    uint32_t subframe_now = 0;
    int flag_sub;

    subframe_now = ue_test.subframe_now();
    printf("\nNow subframe is NO.%d\n", subframe_now);

    flag_sub = ue_test.subframe_process_now();
    if (flag_sub == -1)
    {
        printf("Invaild subframes!\n");
    }
    else if (flag_sub == 0) //1为切换子帧
    {
    }
    else if (flag_sub == 1) //下行帧
    {
        //usleep(5000);
        flag = lte_send_dpdk(NULL);
    }
    else
    {
        flag = lte_rece(NULL); //上行帧
    }
    //printf("\n\nWork done!\n\n");
    ue_test.subframe_end();
}

void handle2() //union sigval v
{
    bool flag;
    uint32_t subframe_now = 0;
    int flag_sub;

    subframe_now = ue_test.subframe_now();
    printf("\nNow subframe is NO.%d\n", subframe_now);

    flag_sub = ue_test.subframe_process_now();
    if (flag_sub == -1)
    {
        printf("Invaild subframes!\n");
    }
    else if (flag_sub == 0) //1为切换子帧
    {
    }
    else if (flag_sub == 1) //下行帧
    {
        //usleep(5000);
        flag = lte_send_dpdk(NULL);
    }
    else
    {
        flag = lte_rece(NULL); //上行帧
    }
    //printf("\n\nWork done!\n\n");
    ue_test.subframe_end();
}

void *lte_send_dpdk_tt(void *ptr)
{
    //struct crc_check_args_t crc_check_args = *((struct crc_check_args_t *)arg);
    struct send_args send_q = *((struct send_args *)ptr);
    //struct send_args send_q;
    // struct recv_args recv_q;
    //send_q.rnti = 0;
    //send_q.buffer_tx = new uint8_t[10240];
    // recv_q.rnti = 0;
    // recv_q.buffer_rx = new uint8_t[10240];
    while (1)
    {
        lte_send_dpdk((void *)&send_q);
    }
    //delete[] send_q.buffer_tx;
}

void *lte_recv_dpdk_local_tt(void *ptr)
{
    struct recv_args recv_q = *((struct recv_args *)ptr);
    // struct send_args send_q;
    //struct recv_args recv_q;
    // send_q.rnti = 0;
    //send_q.buffer_tx = new uint8_t[10240];
    //recv_q.rnti = 0;
    //recv_q.buffer_rx = new uint8_t[10240];
    while (1)
    {
        lte_recv_dpdk_local((void *)&recv_q);
    }
    //delete[] recv_q.buffer_rx;
}

void *sum_error_number(void *ptr)
{
    FILE *fpt;
    unsigned int sum = 0;
    unsigned int sum_all = 0;
    uint32_t times = 0;
    uint32_t retrans_overflow_f = 0;
    fpt = fopen("data.txt", "w"); //打开文档，写入
    if (fpt == NULL)
    {
        printf("File cannot open! ");
        return NULL;
    }
    while (1)
    {
        if(times == 10000)
        {
            break;
        }
        for (int x = 0; x < 10; ++x)
        {
            for (int y = 0; y < 8; ++y)
            {
                for (int z = 0; z < 70; ++z)
                {
                    sum = sum + test_error[x][y][z];
                }
            }
        }
        retrans_overflow_f = ue_test.UE[0].retrans_overflow_times;
        fprintf(fpt, "Now the number of error block without retrans is %u. Now retrans_overflow is %u.\n", sum,retrans_overflow_f);
        sum_all =sum_all+sum;
        sum = 0;
        ++times;
        sleep(3);
        for(int i=0;i<HARQ_NUM;++i)
        {
            ue_test.UE[0].reset(i);
        } 
    }
    fprintf(fpt, "Program end!:::At last,all the number of error block without retrans is %u.\n", sum_all);
    fclose(fpt);
}

void *lte_rlc_mac(void *ptr)
{
    uint8_t *payload_create = new uint8_t[10240];
    uint8_t *payload_back;
    uint32_t pdu_sz_test = 1500;
    while(1)
    {
        if(ue_test.UE[0].all_qbuff_full())
        {
            usleep(2000);
        }
        else
        {
            payload_back = ue_test.UE[0].ue_mux_test.pdu_get(payload_create, pdu_sz_test);
            ue_test.UE[0].pdu_in(payload_back, pdu_sz_test);
        }
    }
}

void *lte_send_recv(void *ptr)
{
    struct send_args send_q;
    struct recv_args recv_q;
    send_q.rnti = 0;
    send_q.buffer_tx = new uint8_t[10240];
    recv_q.rnti = 0;
    recv_q.buffer_rx = new uint8_t[10240];
    bool flag;
    uint32_t subframe_now = 0;
    int flag_sub;
    ue_test.set_subframe(5);
    pthread_barrier_wait(&barrier);
    pthread_create(&id_test[2], NULL, sum_error_number, NULL);
    sleep(1);
    int times = 0;
    //pthread_create(&id_test[3], NULL, lte_rlc_mac, NULL);
    //sleep(3);
    pthread_create(&id_test[0], NULL, lte_send_dpdk_tt, (void *)&send_q);
    pthread_create(&id_test[1], NULL, lte_recv_dpdk_local_tt, (void *)&recv_q);
   

    pthread_join(id_test[0], NULL);
    pthread_join(id_test[1], NULL);
    pthread_join(id_test[2], NULL);
    //pthread_join(id_test[3], NULL);
    // while (1)
    // {
    // subframe_now = ue_test.subframe_now();
    // printf("\nNow subframe is NO.%d\n", subframe_now);

    // flag_sub = ue_test.subframe_process_now();
    // if (flag_sub == -1)
    // {
    //     printf("Invaild subframes!\n");
    // }
    // else if (flag_sub == 0) //1为切换子帧
    // {
    // }
    // else if (flag_sub == 1) //下行帧
    // {
    //     //usleep(5000);
    //     flag = lte_send_dpdk((void *)&send_q);
    // }
    // else
    // {
    //     flag = lte_recv_dpdk_local((void *)&recv_q); //上行帧
    // }
    // //printf("\n\nWork done!\n\n");
    // ue_test.subframe_end();
    //     if (times < 8)
    //     {
    //         if (lte_send_dpdk((void *)&send_q) == false)
    //         {
    //             sleep(5);
    //         }
    //     }
    //     else
    //     {
    //         if (lte_recv_dpdk_local((void *)&recv_q) == false)
    //         {
    //             sleep(5);
    //         }
    //     }
    //     times = (times + 1) % 16;
    // }
    // struct sigevent evp;
    // struct itimerspec ts;
    // timer_t timer;
    // int ret;

    // memset (&evp, 0, sizeof (evp));
    // evp.sigev_value.sival_ptr = &timer;
    // evp.sigev_notify = SIGEV_THREAD;
    // evp.sigev_notify_function = handle;

    // ret = timer_create(CLOCK_REALTIME, &evp, &timer);
    // if (ret)
    //     perror("timer_create");

    // ts.it_interval.tv_sec = 0;
    // ts.it_interval.tv_nsec =1000000;
    // ts.it_value.tv_sec = 30;
    // ts.it_value.tv_nsec = 0;

    // tongbu();

    // ret = timer_settime(timer, 0, &ts, NULL);
    // if (ret)
    //     perror("timer_settime");

    // while (1)
    // {
    // }

    // printf("Timer over!\n");
    // timer_delete(timer);
    // }
    delete[] send_q.buffer_tx;
    delete[] recv_q.buffer_rx;
}