OBJS = crc.o bit.o cbsegm.o debug.o turbocoder.o vector.o tc_interl_umts.o tc_interl_lte.o rm_turbo.o MQAMMod.o phy_common.o sequence.o turbodecoder.o turbodecoder_gen.o turbodecoder_sse.o QAM_NDemod.o CalPilotSymb.o CalSymb_mmse.o ChannelEstimator.o RxAdptLink.o ChannelEstimator_LS.o R_Estimator.o vector_simd.o crc_cbsegm.o crc_mod.o packing.o chest_calsym.o derm_crc.o crc_check.o TaskScheduler.o main.o thread_pool.o lte-ue-main.o lte-ip-pkt.o  lte-udp.o lte-rece.o lte-tun.o mux.o sendandrecv.o demux.o pdu.o pdu_queue.o rlc_um.o  buffer_pool.o log_stdout.o qbuff.o proc_phr.o proc_bsr.o swap_buffer.o  
DEST = main
LIBS = -lm -lpthread -lmkl_rt -msse4.1 -fopenmp -lboost_system -lboost_filesystem -lboost_thread -lboost_date_time -lrt

CPPFLAGS = -g -Wall -I . -msse4.1 -O3 -mavx  -I ./enb_mac/hdr -I ./enb_mac/src/commom -I ./liblte/hdr
main: ${OBJS}
	g++ -o ${DEST} ${OBJS} ${LIBS}
clean:
	rm -f ${DEST} ${OBJS}
