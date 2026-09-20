#include "recorder.h"
#include "motion.h"
#include  "stdio.h"

#define REC_MAX 10
static uint8_t record_data[REC_MAX];
static uint8_t record_count = 0;
static uint8_t playback_index = 0;
static uint8_t playback_active = 0;
static uint8_t playback_done = 1;

void Recorder_Init(void) {
    record_count = 0;
    playback_active = 0;
    playback_done = 1;
}

void Recorder_StartRecord(void) {
    record_count = 0;
	    printf("[REC] Start recording\r\n");
}

void Recorder_StopRecord(void) {
    // 什么都不做
}

void Recorder_RecordAngle(uint8_t angle) {
    if (record_count < REC_MAX) {
        record_data[record_count++] = angle;
			 printf("[REC] Recorded angle: %d (total %d)\r\n", angle, record_count);
    }
}

void Recorder_StartPlayback(void) {
    if (record_count == 0) return;
    playback_index = 0;
    playback_active = 1;
    playback_done = 0;
printf("[REC] Playback started, %d frames\r\n", record_count);	
}

void Recorder_StopPlayback(void) {
    playback_active = 0;
    playback_done = 1;
}

uint8_t Recorder_IsPlaybackDone(void) {
    return playback_done;
}

uint8_t Recorder_GetCount(void) {
    return record_count;
}

void Recorder_UpdatePlayback(void) {
    if (!playback_active) return;
    if (Motion_IsIdle() && playback_index < record_count) {
        printf("[REC] Playing frame %d: %d deg\r\n", playback_index, record_data[playback_index]);
        Motion_SetTarget(record_data[playback_index++]);
    }
    if (playback_index >= record_count && Motion_IsIdle()) {
        playback_active = 0;
        playback_done = 1;
    }
}
