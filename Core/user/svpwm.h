#ifndef __SVPWM_H__
#define __SVPWM_H__

#include "main.h"
#include "tim.h"
#include <math.h>

// 数学常数定义
#define PI 3.14159265358979323846f
#define PIP3 (PI / 3.0f)  // π/3
#define SQRT3 1.732050807568877f  // √3

// SVPWM参数结构体
typedef struct {
    float Fp;        // PWM频率 (Hz)
    uint32_t Fsrc;   // 系统时钟频率 (Hz)
    float Fs;        // 当前输出频率 (Hz)
    float Fs_max;    // 最大输出频率 (Hz)
    float Acl;       // 加速度参数
    float Ma;        // 调制比 (0.0 - 1.0)
    
    uint16_t PCS;    // 预分频器值
    uint16_t ARR;    // 自动重装载值
    float Dphi;      // 相位增量
    float Tp;        // PWM周期
    float Vsin;      // 频率变化速度
    uint8_t State;   // SVPWM状态 (0: 停止, 1: 运行)
} SVMParams;

// 电机参数结构体
typedef struct {
    float frequency;  // 电机频率
    float amplitude;  // 电机幅值
    float phase;      // 电机相位
} MotorParams;

// 三相参数结构体
typedef struct {
    float Va;  // A相电压
    float Vb;  // B相电压
    float Vc;  // C相电压
} ThreePhaseParams;

// 全局变量声明
extern MotorParams motor;
extern ThreePhaseParams sinware;
extern SVMParams svm;
extern float fsin;

// SVPWM函数声明
void SVPWM_Init(void);
void SVPWM_SetFs(float fs);
void SVPWM_SetFp(float fp);
void SVPWM_SetMa(float ma);
void SVPWM_SetTargetFreq(float target_freq);  // 设置目标频率
void SVPWM_State(uint8_t state);
void SVPWM_Update(float phi, float ma, unsigned int tpwm);
void SVPWM_FrequencyRampControl(void);  // 频率斜坡控制

#endif /* __SVPWM_H__ */
