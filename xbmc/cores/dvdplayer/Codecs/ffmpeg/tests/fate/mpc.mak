FATE_TESTS += fate-mpc7-demux
fate-mpc7-demux: CMD = crc  -i $(SAMPLES)/musepack/inside-mp7.mpc -acodec copy

FATE_TESTS += fate-mpc8-demux
fate-mpc8-demux: CMD = crc  -i $(SAMPLES)/musepack/inside-mp8.mpc -acodec copy

FATE_TESTS += fate-musepack7
fate-musepack7: CMD = pcm -i $(SAMPLES)/musepack/inside-mp7.mpc
fate-musepack7: CMP = oneoff
fate-musepack7: REF = $(SAMPLES)/musepack/inside-mp7.pcm
fate-musepack7: FUZZ = 1
