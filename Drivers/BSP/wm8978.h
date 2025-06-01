/*
 * wm8978.h
 *
 *  Created on: Apr 2, 2025
 *      Author: Atomic
 */

#ifndef BSP_WM8978_H_
#define BSP_WM8978_H_

#include "main.h"

#define WM8978_I2C_HANDLE	hi2c1	/* 选择要使用的硬件 I2C 端口*/
#define WM8978_I2C_ADDRESS	0x34	/* WM8978 I2C从机地址 */

/*等待超时时间*/
#define WM8978_I2C_FLAG_TIMEOUT         ((uint32_t)0x4000)
#define WM8978_I2C_LONG_TIMEOUT         ((uint32_t)(10 * WM8978_I2C_FLAG_TIMEOUT))

/* 定义最大音量 */
#define VOLUME_MAX		63		/* 最大音量 */
#define VOLUME_STEP		1		/* 音量调节步长 */

/* 定义最大MIC增益 */
#define GAIN_MAX		63		/* 最大增益 */
#define GAIN_STEP		1		/* 增益步长 */

/* WM8978 音频输入通道控制选项, 可以选择多路，比如 MIC_LEFT_ON | LINE_ON */
typedef enum
{
	IN_PATH_OFF		= 0x00,	/* 无输入 */
	MIC_LEFT_ON 	= 0x01,	/* LIN,LIP脚，MIC左声道（开发板没用到）*/
	MIC_RIGHT_ON	= 0x02,	/* RIN,RIP脚，MIC右声道（接板载咪头）  */
	LINE_ON			= 0x04, /* L2,R2 立体声输入 */
	AUX_ON			= 0x08,	/* AUXL,AUXR 立体声输入（开发板没用到） */
	DAC_ON			= 0x10,	/* I2S数据DAC (CPU产生音频信号) */
	ADC_ON			= 0x20	/* 输入的音频馈入WM8978内部ADC （I2S录音) */
}IN_PATH_E;

/* WM8978 音频输出通道控制选项, 可以选择多路 */
typedef enum
{
	OUT_PATH_OFF	= 0x00,	/* 无输出 */
	EAR_LEFT_ON 	= 0x01,	/* LOUT1 耳机左声道 */
	EAR_RIGHT_ON	= 0x02,	/* ROUT1 耳机右声道 */
	SPK_ON			= 0x04,	/* LOUT2和ROUT2反相输出单声道,接扬声器 */
	OUT3_4_ON		= 0x08,	/* OUT3 和 OUT4 输出单声道音频*/
}OUT_PATH_E;


/* 供外部引用的函数声明 */
HAL_StatusTypeDef WM8978_Init(void);
HAL_StatusTypeDef WM8978_Reset(void);
void WM8978_CfgAudioIF(uint16_t _usStandard, uint8_t _ucWordLen);
void WM8978_OutMute(uint8_t _ucMute);
void WM8978_PowerDown(void);
void WM8978_CfgAudioPath(uint16_t _InPath, uint16_t _OutPath);
void WM8978_SetMicGain(uint8_t _ucGain);
void WM8978_SetLineGain(uint8_t _ucGain);
void WM8978_SetOUT2Volume(uint8_t _ucVolume);
void WM8978_SetOUT1Volume(uint8_t _ucVolume);
uint8_t WM8978_ReadOUT1Volume(void);
uint8_t WM8978_ReadOUT2Volume(void);
void WM8978_NotchFilter(uint16_t _NFA0, uint16_t _NFA1);
void I2S_READ(uint32_t buffer0,uint32_t buffer1,const uint32_t num);

void WM8978_CtrlGPIO1(uint8_t _ucValue);

#endif /* BSP_WM8978_H_ */
