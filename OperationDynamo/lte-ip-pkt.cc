#include "FuncHead.h"

using namespace srslte;
using namespace srsue;

#include "Cpp_fun.h"

// buffer for reading from tun/tap interface, must be >= 1500
#define BUFSIZE 2000
#define dst_ip_start 16

extern rlc_um rlc_test[];
extern rlc_um rlc3;
extern int tun_fd;

extern UE_FX ue_test;
/**************************************************************************
* lte_send_ip_3:从tun中读数据并压入队列
**************************************************************************/

void *lte_send_ip_3(void *ptr)
{

	printf("enter--lte_send_ip_3\n");

	/******************************************
	* tun_alloc
	******************************************/
	uint16_t nread;
	uint8_t buffer[200][BUFSIZE] = {0};

	// /*****************************************
	// * read from tun and write to rlc
	// ******************************************/
	byte_buffer_t sdu_bufs[200]; //暂时定为1000次以内读写必然不会重叠

	int k = 0;
	int cycle_times = 1;
	int cycle_now = 0;
	//uint8_t ipClient1[4]={192,168,2,1};
	//uint8_t ipClient2[4]={192,168,2,2};
	//uint8_t ipClient3[4]={192,168,2,3};
	int i = 0;

	uint32_t pdu_sz_test = 1500;
	uint8_t *payload_create = new uint8_t[10240];
	uint8_t *payload_back;

	nread = read(tun_fd, buffer[0], BUFSIZE);
	if (nread <= 0)
	{
		perror("Nothing reading");
		exit(1);
	}
	else
	{
		printf("IP read bytes is %d.\n", nread);
		++k;
	}
	while (1)
	{

		if (k == 200)
		{
			// if(cycle_now == cycle_times)
			// {
			// 	break;
			// }
			k = 1;
			//++cycle_now;
		}
		//data from tun/tap: 想读取sizeof(buffer)个数据,成功则返回读取的实际字节数nread
		memcpy(buffer[k], buffer[0], BUFSIZE);
		//printf("LKLKLK!\n");

		//printf("IP-PKT:::NOW READ %d!!!\n",nread);
		sdu_bufs[k].msg = buffer[k]; //index
		sdu_bufs[k].N_bytes = nread; //size

		//rlc3.write_sdu(&sdu_bufs[k]);
		//匹配目的ip，基站端
		//if((buffer[k][dst_ip_start]==192) && (buffer[k][dst_ip_start+1]==168) && (buffer[k][dst_ip_start+2]==2) && (buffer[k][dst_ip_start+3]==1)){
		rlc_test[i].write_sdu(&sdu_bufs[k]);
		printf("IP-PKT-:::Now %d rlc_in is %d.\n", i, rlc_test[i].n_unread());
		k++;

		// payload_back = ue_test.UE[i].ue_mux_test.pdu_get(payload_create, pdu_sz_test);
		// if (payload_back != NULL)
		// {
		// 	swap_write_in(2, payload_back, pdu_sz_test);
		// }
	}
	delete[] payload_create;
	printf("Ip-pkt is over.\n");
}
