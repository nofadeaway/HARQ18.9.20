#include "FuncHead.h"

#include "Cpp_fun.h"

#define PAYLOAD_SIZE 400

using namespace srslte;
using namespace srsue;

extern mac_dummy_timers timers_test;

extern UE_FX ue_test; //map容器

extern pthread_barrier_t barrier;

extern int test_error[10][8][200]; //记录失包，帧号 x HARQ PID x number
/**************************************************************************
* ipsend:从tun中读数据并压入队列
**************************************************************************/

bool lte_rece(void *ptr)
{
	struct timeval tti_timeout = {0, 900000}; //用于测试，recv超时

	int port_add = 0;
	if (ptr != NULL)
	{
		port_add = *((int *)ptr);
		printf("Recv:send The port offset is %d\n", port_add);
	}
	else
	{
		printf("Recv:No port offset inport.\n");
	}

	uint16_t rnti = port_add;
	//printf("enter--lte_rece\n");

	int st = socket(AF_INET, SOCK_DGRAM, 0);
	if (st == -1)
	{
		printf("open socket failed ! error message : %s\n", strerror(errno));
		exit(1);
	}
	int port = atoi("8808"); //接受数据的端口
	port = port + port_add;
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(st, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		printf("bind IP failed ! error message : %s\n", strerror(errno));
		exit(1);
	}

	struct sockaddr_in client_addr;
	socklen_t addrlen = sizeof(client_addr);

	int rece_size = 300, k = 0;
	; //修改为随机啊！！！！！！！！！！！
	uint8_t rece_payload[1000][PAYLOAD_SIZE] = {0};

	/****************************/
	//ACK接受
	int st_a = socket(AF_INET, SOCK_DGRAM, 0);
	if (st_a == -1)
	{
		printf("ACK:open socket failed ! error message : %s\n", strerror(errno));
		exit(1);
	}
	int port_a = atoi("5500");
	port_a = port_a + port_add;

	struct sockaddr_in addr_a;

	addr_a.sin_family = AF_INET;
	addr_a.sin_port = htons(port_a);
	addr_a.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(st_a, (struct sockaddr *)&addr_a, sizeof(addr_a)) == -1)
	{
		printf("ACK_receive:bind IP failed ! error message : %s\n", strerror(errno));
		exit(1);
	}

	setsockopt(st_a, SOL_SOCKET, SO_RCVTIMEO, (char *)&tti_timeout, sizeof(struct timeval)); //FX:8.24

	uint32_t subframe_now = 0;
	subframe_now = ue_test.subframe_now();

	/****************************/
	//FX   接受ACK
	char temp[100];
	A_ACK ack_reply;
	//ack_reply.ack_0=true;
	memset(temp, 0, sizeof(temp));

	if (recv(st_a, temp, sizeof(ack_reply), 0) == -1)
	{
		printf("ACK:recvfrom failed ! error message : %s\n", strerror(errno));
	}
	else
	{
		memcpy(&ack_reply, temp, sizeof(ack_reply));
		ue_test.UE[rnti].set_ack_come(subframe_now, &ack_reply.ack_0);
		char str1[10] = "true", str2[10] = "false";
		printf("/******lte-Recv:");
		printf("RNTI:%d::: No.%d ACK received is %s\n", rnti, ack_reply.ACK_pid, (ack_reply.ack_0) ? str1 : str2);
	}

	close(st);
	close(st_a);
}

bool lte_recv_dpdk_local(void *ptr)
{
	uint16_t rnti;
	uint8_t *payload_store;
	bool flag = true;
	bool ack_test[3] = {true, true, true};
	struct recv_args recv_arg; //FX:7.10
	struct pdu_header header = {};
	int rand_n = 20;

	//uint32_t subframe_now = ue_test.subframe_now();
	if (ptr != NULL)
	{
		recv_arg = *((struct recv_args *)ptr);
		rnti = recv_arg.rnti;
		payload_store = recv_arg.buffer_rx;
	}
	else
	{
		printf("lte_recv_dpdk_local: no args in.\n");
		return false;
	}
	//srand((unsigned)time(NULL));
	while (1)
	{
		swap_read_from(2, payload_store, 1500 + sizeof(struct pdu_header));
		memcpy(&header, payload_store, sizeof(struct pdu_header));
		ue_test.UE[rnti].mac_demux_test.process_pdu(payload_store+sizeof(struct pdu_header),1500);
		printf("\nPDU received.The subframe/HARQ pid/number::%u/%u/%u.\n", header.sub_frame, header.harq_pid, header.number);
		srand((unsigned)time(NULL));
		rand_n = (rand() % 100);
		if ((rand_n % 10) == 0) //误包率
		{
			test_error[0][header.harq_pid][header.number] = 1;
			ue_test.UE[rnti].ACK[header.harq_pid] = false;
			printf("The subframe/HARQ pid/number::%u/%u/%u ACK is false!!!\n", header.sub_frame, header.harq_pid, header.number);
		}
		else
		{
			test_error[0][header.harq_pid][header.number] = 0;
			ue_test.UE[rnti].ACK[header.harq_pid] = true;
		}
		ue_test.UE[rnti].reset(header.harq_pid);
	}

	//ue_test.UE[rnti].set_ack_come(subframe_now, ack_test);

	//ue_test.UE[rnti].mac_demux_test.process_pdu(payload_store+sizeof(struct pdu_header),1500);

	//ue_test.subframe_end();
	return flag;
}
