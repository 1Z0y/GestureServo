#ifndef __RECORDER_H
#define __RECORDER_H
#include "main.h"
#define REC_MAX 10   // 添加这一行，最大录制帧数
void Recorder_Init(void);
void Recorder_StartRecord(void);
void Recorder_StopRecord(void);
void Recorder_RecordAngle(uint8_t angle);
void Recorder_StartPlayback(void);
void Recorder_StopPlayback(void);
uint8_t Recorder_IsPlaybackDone(void);
uint8_t Recorder_GetCount(void);
void Recorder_UpdatePlayback(void); // 在定时循环中调用
#endif
