#include "Communication.hpp"

#include "MathUtil.hpp"
#include "PowerManager.hpp"
#include "can.h"

namespace Communication
{
struct RxData
{
    uint8_t enableDCDC : 1;     //表示占用1位（uint8_t的最低位
    uint8_t systemRestart : 1;  //表示占用1位（uint8_t的第1位
    uint8_t resv0 : 6;          //表示占用1位（uint8_t的第2~7位
    uint16_t feedbackRefereePowerLimit;
    uint16_t feedbackRefereeEnergyBuffer;   //缓冲能量
    uint8_t resv1[3];   //保留字节3字节
} __attribute__((packed));     //确保编译器不对结构体进行内存对齐，数据大小严格等于成员大小之和。

static RxData rxData;

struct TxData   //8+32+16+8=64bit
{
    uint8_t errorCode;
    float chassisPower; //底盘的功率：vA*IReferee - vA*iA
    uint16_t chassisPowerLimit;
    uint8_t capEnergy;  //vB平方÷MAX_CAP_VOLTAGE平方
} __attribute__((packed));

static TxData txData;

/** txData存入can的Mailbox寄存器，这样再被发送出去
 * rxData收到后位于CAN_RX_FIFO0，之后再被去除进行操作 **/

void init()
{
    static_assert(sizeof(RxData) == 8, "RxData size error");    //静态断言（编译期间断言）：第一个参数表达式不成立则产生一个编译错误
    static_assert(sizeof(TxData) == 8, "TxData size error");

    CAN_FilterTypeDef filter = {
        0x061 << 5,             // filterID HI:1100_0010_000x_xxxx
        0,                      // filterID LO
        0,                      // filterMask HI-->表示需要完全匹配ID
        0,                      // filterMask LO
        CAN_RX_FIFO0,           // FIFO assignment-->使用FIFO0存储接收数据
        CAN_FILTER_FIFO0,       // filterBank number
        CAN_FILTERMODE_IDLIST,  // filter mode-->ID列表模式/ID掩码模式
        CAN_FILTERSCALE_16BIT,  // filter size
        CAN_FILTER_ENABLE,      // ENABLE or DISABLE
        0                       // Slave start bank, STM32F334 only has master
    };
    HAL_CAN_ConfigFilter(&hcan, &filter);
    __HAL_CAN_ENABLE_IT(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);    //配置 CAN 滤波器，使其只接收 ID 为 0x061 的标准帧
    HAL_CAN_Start(&hcan);
}

//标准数据帧:
//| SOF | 标识符（ID 11 位） | RTR | 控制字段 | 数据字段 | CRC | ACK | EOF |
//|  1  |         11        |  1  |    6    |   0~64   | 15  |  2  |  7  |

static CAN_TxHeaderTypeDef txHeader = { //Can报头。定义在stm32f3xx_hal_can.h
    0x051,         // StdId:0000_0101_0001
    0,             // ExtId:未使用
    CAN_ID_STD,    // IDE-->表示使用标准帧(stm32f3xx_hal_can.h
    CAN_RTR_DATA,  // RTR
    8,             // DLC-->表示携带8个字节数据（64bit
    DISABLE,       // TransmitGlobalTime-->不启用时间戳
};

void feedbackPowerData()
{
    txData.errorCode         = PowerManager::Status::status.errorCode | ((uint8_t)!PowerManager::Status::status.outputEnabled) << 7;
    txData.chassisPower      = PowerManager::Status::status.chassisPower;
    txData.chassisPowerLimit = PowerManager::Status::status.chassisPowerLimit;
    txData.capEnergy         = PowerManager::Status::status.capEnergy > 1.0f ? 255 : PowerManager::Status::status.capEnergy * 255;
    // HAL_CAN_AddTxMessage(&hcan, &txHeader, (uint8_t *)&txData, &txMailbox);
    if ((READ_REG(hcan.Instance->TSR) & CAN_TSR_TME0) != 0U)
        SET_BIT(hcan.Instance->TSR, CAN_TSR_ABRQ0);
    hcan.Instance->sTxMailBox[CAN_TX_MAILBOX0].TIR  = ((txHeader.StdId << CAN_TI0R_STID_Pos) | txHeader.RTR);
    hcan.Instance->sTxMailBox[CAN_TX_MAILBOX0].TDTR = (txHeader.DLC);
    WRITE_REG(hcan.Instance->sTxMailBox[CAN_TX_MAILBOX0].TDHR,
              ((uint32_t)(((uint8_t *)&txData)[7]) << CAN_TDH0R_DATA7_Pos) | ((uint32_t)(((uint8_t *)&txData)[6]) << CAN_TDH0R_DATA6_Pos) |
                  ((uint32_t)(((uint8_t *)&txData)[5]) << CAN_TDH0R_DATA5_Pos) | ((uint32_t)(((uint8_t *)&txData)[4]) << CAN_TDH0R_DATA4_Pos));
    WRITE_REG(hcan.Instance->sTxMailBox[CAN_TX_MAILBOX0].TDLR,
              ((uint32_t)(((uint8_t *)&txData)[3]) << CAN_TDL0R_DATA3_Pos) | ((uint32_t)(((uint8_t *)&txData)[2]) << CAN_TDL0R_DATA2_Pos) |
                  ((uint32_t)(((uint8_t *)&txData)[1]) << CAN_TDL0R_DATA1_Pos) | ((uint32_t)(((uint8_t *)&txData)[0]) << CAN_TDL0R_DATA0_Pos));
    SET_BIT(hcan.Instance->sTxMailBox[CAN_TX_MAILBOX0].TIR, CAN_TI0R_TXRQ);
}

CAN_RxHeaderTypeDef rxHeader;

extern "C"
{
    void CAN_RX0_IRQHandler(void)
    {
        if ((hcan.Instance->RF0R & CAN_RF0R_FMP0) != 0U)
        {
            while (HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &rxHeader, (uint8_t *)&rxData) == HAL_OK)
                if ((rxHeader.StdId == 0x061) && (rxHeader.DLC == 8) && (rxHeader.IDE == CAN_ID_STD))
                {
                    static bool lastEnableDCDC = true;

                    if (rxData.systemRestart)
                        PowerManager::systemRestart();

                    PowerManager::ControlData::controlData.refereePowerLimit = M_CLAMP(rxData.feedbackRefereePowerLimit, 30.0f, 250.0f);
                    PowerManager::ControlData::controlData.energyRemain      = M_CLAMP(rxData.feedbackRefereeEnergyBuffer, 0.0f, 300.0f);

                    if (rxData.enableDCDC == false || rxData.feedbackRefereePowerLimit > 250.0f)
                        PowerManager::ControlData::controlData.enableOutput = false;
                    else if (lastEnableDCDC == false)
                        PowerManager::ControlData::controlData.enableOutput = true;
                    lastEnableDCDC = rxData.enableDCDC;

                    PowerManager::updateEnergy();
                }
        }
    }
}

}  // namespace Communication