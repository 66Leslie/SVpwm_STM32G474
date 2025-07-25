#include "svpwm.h"
#include <stdio.h>

// 全局变量定义
MotorParams motor;
ThreePhaseParams sinware;
SVMParams svm;
float fsin = 0.0f;  // 目标频率设定值
float phi = 0;

/**
 * @brief SVPWM初始化函数
 * @note 使用TIM8替代原来的TIM4，适配您的硬件配置
 */
void SVPWM_Init(void) {
    // 初始化SVPWM参数
    svm.Fp = 10000;                   // PWM频率 10kHz (与TIM8配置匹配)
    svm.Fsrc = 170000000;             // STM32G4系统时钟频率 170MHz
    svm.Fs = 0;                       // 初始输出频率
    svm.Fs_max = 100.0f;              // 最大输出频率 100Hz (足够包含50Hz)
    svm.Acl = 10.0f;                  // 加速度参数 (Hz/s²)
    svm.Ma = 0.5;                     // 初始调制比
    svm.State = 0;                    // 初始状态：停止

    // 使用现有的TIM8配置参数
    svm.PCS = htim8.Init.Prescaler;   // 使用现有预分频器
    svm.ARR = htim8.Init.Period;      // 使用现有自动重装载值

    // 计算相位增量
    svm.Dphi = svm.Fs * PI / svm.Fp;

    // 注意：TIM8的PWM输出由Three_Phase_PWM_Enable()函数控制
    // 这里不重复启动PWM

    // 初始化相位
    phi = 0;
}

/**
 * @brief 设置SVPWM输出频率
 * @param fs 目标频率 (Hz)
 */
void SVPWM_SetFs(float fs) {
    svm.Fs = fs;
    svm.Dphi = svm.Fs * PI / svm.Fp;
}

/**
 * @brief 设置SVPWM载波频率
 * @param fp PWM载波频率 (Hz)
 */
void SVPWM_SetFp(float fp) {
    svm.Fp = fp;
    
    // 重新计算预分频器和自动重装载值
    for (int i = 1; i < 1000; i++) {
        uint16_t ARR = svm.Fsrc / (2 * (i + 1) * svm.Fp) - 1;
        if (ARR < 0x3FF) {
            svm.PCS = i - 1;
            svm.ARR = svm.Fsrc / (2 * (svm.PCS + 1) * svm.Fp) - 1;
            break;
        }
    }
    
    svm.Dphi = svm.Fs * PI / svm.Fp;
    svm.Fs_max = svm.Fp / 100; // Fp / Fs >= 100
    
    // 更新定时器配置
    __HAL_TIM_SET_PRESCALER(&htim8, svm.PCS);
    __HAL_TIM_SET_AUTORELOAD(&htim8, svm.ARR);
}

/**
 * @brief 设置SVPWM调制比
 * @param ma 调制比 (0.0 - 1.0)
 */
void SVPWM_SetMa(float ma) {
    // 限制调制比范围
    if (ma > 1.0f) ma = 1.0f;
    if (ma < 0.0f) ma = 0.0f;

    svm.Ma = ma;

    // 调试信息：显示调制比变化
    static float last_ma = -1.0f;
    if (ma != last_ma) {
        printf("SVPWM Ma changed: %.3f\n", ma);
        last_ma = ma;
    }
}

/**
 * @brief 设置SVPWM目标频率
 * @param target_freq 目标频率 (Hz)
 */
void SVPWM_SetTargetFreq(float target_freq) {
    // 限制频率范围
    if (target_freq > svm.Fs_max) target_freq = svm.Fs_max;
    if (target_freq < 0.0f) target_freq = 0.0f;

    fsin = target_freq;
}

/**
 * @brief 控制SVPWM状态
 * @param state 状态 (0: 停止, 1: 运行)
 */
void SVPWM_State(uint8_t state) {
    if (state != svm.State) {
        svm.State = state;
        if (state) {
            // 启动SVPWM
            HAL_TIM_Base_Start_IT(&htim8);
        } else {
            // 停止SVPWM
            fsin = 0;
            svm.Vsin = 0;
        }
    }

    // 如果状态为停止且频率为0，完全停止PWM输出
    if (state == 0x00) {
        if (svm.Fs == 0) {
            HAL_TIM_Base_Stop_IT(&htim8);
            __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);
            __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 0);
            __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3, 0);
        }
    }
}

/**
 * @brief SVPWM核心算法 - 空间矢量脉宽调制
 * @param phi 当前相位角 (弧度)
 * @param ma 调制比 (0.0 - 1.0)
 * @param tpwm PWM周期计数值
 */
void SVPWM_Update(float phi, float ma, unsigned int tpwm) {
    // 安全检查
    if (ma <= 0.0f || tpwm == 0) {
        __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);
        __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 0);
        __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3, 0);
        return;
    }

    // 1. 确定扇区 (1-6)，确保phi在0-2π范围内
    while (phi >= 2 * PI) phi -= 2 * PI;
    while (phi < 0) phi += 2 * PI;

    int sector = (int)(phi / PIP3) + 1;
    if (sector > 6) sector = 6;
    if (sector < 1) sector = 1;

    // 2. 计算扇区内的角度
    float angles2 = phi - (PIP3 * (sector - 1));
    float angles1 = PIP3 - angles2;

    // 3. 计算基本矢量作用时间
    int Ta = (int)(ma * tpwm * sinf(angles1));
    int Tb = (int)(ma * tpwm * sinf(angles2));

    // 4. 计算零矢量作用时间
    int HT0 = (tpwm - Ta - Tb) / 2;

    // 确保HT0不为负
    if (HT0 < 0) HT0 = 0;

    // 5. 根据扇区计算三相PWM占空比
    uint16_t CCR1, CCR2, CCR3;

    switch (sector) {
        case 1:  // 扇区1: U1(100) -> U2(110) -> U0(000)
            CCR1 = Ta + Tb + HT0;
            CCR2 = Tb + HT0;
            CCR3 = HT0;
            break;
        case 2:  // 扇区2: U3(010) -> U2(110) -> U0(000)
            CCR1 = Ta + HT0;
            CCR2 = Ta + Tb + HT0;
            CCR3 = HT0;
            break;
        case 3:  // 扇区3: U3(010) -> U4(011) -> U0(000)
            CCR1 = HT0;
            CCR2 = Ta + Tb + HT0;
            CCR3 = Tb + HT0;
            break;
        case 4:  // 扇区4: U5(001) -> U4(011) -> U0(000)
            CCR1 = HT0;
            CCR2 = Ta + HT0;
            CCR3 = Ta + Tb + HT0;
            break;
        case 5:  // 扇区5: U5(001) -> U6(101) -> U0(000)
            CCR1 = Tb + HT0;
            CCR2 = HT0;
            CCR3 = Ta + Tb + HT0;
            break;
        case 6:  // 扇区6: U1(100) -> U6(101) -> U0(000)
            CCR1 = Ta + Tb + HT0;
            CCR2 = HT0;
            CCR3 = Ta + HT0;
            break;
        default:
            CCR1 = CCR2 = CCR3 = HT0;
            break;
    }

    // 6. 限制占空比范围
    if (CCR1 > tpwm) CCR1 = tpwm;
    if (CCR2 > tpwm) CCR2 = tpwm;
    if (CCR3 > tpwm) CCR3 = tpwm;

    // 7. 设置PWM占空比
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, CCR1);
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, CCR2);
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3, CCR3);

    // 调试输出 (您添加的)
    //printf("Sector:%d, Ma:%.2f, CCR1:%d,CCR2:%d,CCR3:%d\n", sector, ma, CCR1, CCR2, CCR3);
}

/**
 * @brief SVPWM频率斜坡控制函数
 * @note 在TIM8中断中调用，处理频率变化和相位累加
 */
void SVPWM_FrequencyRampControl(void) {
    // 1. 频率限制保护
    if (fsin > svm.Fs_max) {
        fsin = svm.Fs_max;
    } else if (fsin < -1 * svm.Fs_max) {
        fsin = -1 * svm.Fs_max;
    }

    // 2. 频率斜坡控制算法 (S曲线加减速)
    if (svm.Fs < fsin) {
        // 加速过程
        svm.Tp = 2.0f / svm.Fp;
        svm.Vsin += svm.Acl * svm.Tp;
        svm.Fs += svm.Vsin * svm.Tp + svm.Acl * svm.Tp * svm.Tp / 2;
        if (svm.Fs > fsin) {
            svm.Fs = fsin;
            svm.Vsin = 0;
        }
        svm.Dphi = svm.Fs * PI / svm.Fp;
    } else if (svm.Fs > fsin) {
        // 减速过程
        svm.Tp = 2.0f / svm.Fp;
        svm.Vsin += -svm.Acl * svm.Tp;
        svm.Fs += svm.Vsin * svm.Tp - svm.Acl * svm.Tp * svm.Tp / 2;
        if (svm.Fs < fsin) {
            svm.Fs = fsin;
            svm.Vsin = 0;
        }
        svm.Dphi = svm.Fs * PI / svm.Fp;
    } else {
        // 恒速过程
        svm.Vsin = 0;
        svm.Fs = fsin;
        svm.Dphi = svm.Fs * PI / svm.Fp;
    }

    // 3. 相位累加
    phi += svm.Dphi;

    // 4. 相位归一化 (0 - 2π)
    if (phi > 2 * PI) {
        phi = 0;
    } else if (phi < 0) {
        phi = 2 * PI;
    }

    // 5. 执行SVPWM更新
    SVPWM_Update(phi, svm.Ma, svm.ARR);
}
