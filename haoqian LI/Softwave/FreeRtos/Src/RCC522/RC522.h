#ifndef _RC522_H
#define _RC522_H

#include "main.h"
#include "stm32f1xx_hal.h"
#include "RFID_API.h"
#include "stdint.h"


#define MYRC522_CS(x)  x?HAL_GPIO_WritePin(RC522_CS_GPIO_Port,RC522_CS_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(RC522_CS_GPIO_Port,RC522_CS_Pin,GPIO_PIN_RESET);
#define MYRC522_RST(x)  x?HAL_GPIO_WritePin(RC522_RST_GPIO_Port,RC522_RST_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(RC522_RST_GPIO_Port,RC522_RST_Pin,GPIO_PIN_RESET);//
uint32_t ReadID(void);
char MFRC522_Reset(void);
char PcdSelect(unsigned char *pSnr);  //ѡ��
char PcdWrite(unsigned char addr,unsigned char *pData);   //д���ݵ�M��
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr); //��֤����
char PcdRead(unsigned char addr,unsigned char *pData);
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData);
char PcdAnticoll(uint8_t *pSnr);
void ClearBitMask(unsigned char reg,unsigned char mask)  ;
void MFRC522_Initializtion(void);
void AntennaOn(uint8_t off) ;
void WriteRawRC(unsigned char Address, unsigned char value)  ;
unsigned char ReadRawRC(unsigned char Address);
void ClearBitMask(unsigned char reg,unsigned char mask)  ;
void SetBitMask(unsigned char reg,unsigned char mask)  ;
char PcdComMF522(uint8_t   Command, 
                 uint8_t *pIn , 
                 uint8_t   InLenByte,
                 uint8_t *pOut , 
                 uint8_t *pOutLenBit);
char PcdRequest(unsigned char req_code,unsigned char *pTagType);
char PcdHalt(void);
								 
								 
								 //MF522������
/////////////////////////////////////////////////////////////////////
#define PCD_IDLE              0x00               //ȡ����ǰ����
#define PCD_AUTHENT           0x0E               //��֤��Կ
#define PCD_RECEIVE           0x08               //��������
#define PCD_TRANSMIT          0x04               //��������
#define PCD_TRANSCEIVE        0x0C               //���Ͳ���������
#define PCD_RESETPHASE        0x0F               //��λ
#define PCD_CALCCRC           0x03               //CRC����

/////////////////////////////////////////////////////////////////////
//Mifare_One��Ƭ������
/////////////////////////////////////////////////////////////////////
#define PICC_REQIDL           0x26               //Ѱ��������δ��������״̬
#define PICC_REQALL           0x52               //Ѱ��������ȫ����
#define PICC_ANTICOLL1        0x93               //����ײ
#define PICC_ANTICOLL2        0x95               //����ײ
#define PICC_AUTHENT1A        0x60               //��֤A��Կ
#define PICC_AUTHENT1B        0x61               //��֤B��Կ
#define PICC_READ             0x30               //����
#define PICC_WRITE            0xA0               //д��
#define PICC_DECREMENT        0xC0               //�ۿ�
#define PICC_INCREMENT        0xC1               //��ֵ
#define PICC_RESTORE          0xC2               //�������ݵ�������
#define PICC_TRANSFER         0xB0               //���滺����������
#define PICC_HALT             0x50               //����
// PAGE 0                										�����״̬
#define     RFU00                 0x00    //����
#define     CommandReg            0x01    //������ֹͣ�����ִ��
#define     ComIEnReg             0x02    //�ж�������ʹ��λ
#define     DivlEnReg             0x03    //�ж�������ʹ��λ
#define     ComIrqReg             0x04    //�����ж������־
#define     DivIrqReg             0x05		//�����ж������־
#define     ErrorReg              0x06    //�����־λ
#define     Status1Reg            0x07    //����״̬��־λ
#define     Status2Reg            0x08    //�������պͷ��ͱ�־λ
#define     FIFODataReg           0x09		//64�ֽڷ��͡�����fifo������
#define     FIFOLevelReg          0x0A		//fifo�������洢�ֽ���
#define     WaterLevelReg         0x0B		//fifo��������籨���ĵȼ�
#define     ControlReg            0x0C		//��ͬ�Ŀ��ƼĴ���
#define     BitFramingReg         0x0D		//����λ��֡�ĵ���
#define     CollReg               0x0E		//RF�ӿ��ϼ�⵽�ĵ�һ��λ��ͻ��λ��λ��
#define     RFU0F                 0x0F		//����λ
// PAGE 1     															����
#define     RFU10                 0x10		//����λ
#define     ModeReg               0x11		//���巢�ͺͽ���ģʽ
#define     TxModeReg             0x12		//���巢����������
#define     RxModeReg             0x13		//���������������
#define     TxControlReg          0x14		//������������1��2���߼�����
#define     TxAutoReg             0x15		//����������������
#define     TxSelReg              0x16		//ѡ�����������ڲ�Դ
#define     RxSelReg              0x17		//ѡ���ڲ���������
#define     RxThresholdReg        0x18		//ѡ��λ����������ֵ
#define     DemodReg              0x19		//��������������
#define     RFU1A                 0x1A		//����λ
#define     RFU1B                 0x1B		//����λ
#define     MifareReg             0x1C		//����ISO mifareģʽ��106kbit/s��ͨ��
#define     RFU1D                 0x1D		//����λ
#define     RFU1E                 0x1E		//����λ
#define     SerialSpeedReg        0x1F		//ѡ��uart�ӿڵ�����
// PAGE 2    																CRG
#define     RFU20                 0x20  	//����λ
#define     CRCResultRegM         0x21//��ʾCRC��������ֵ
#define     CRCResultRegL         0x22//��ʾCRC�������Сֵ
#define     RFU23                 0x23//����λ
#define     ModWidthReg           0x24//����modwidth������
#define     RFU25                 0x25//����λ
#define     RFCfgReg              0x26//���ý�������
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A//�ڲ���ʱ������
#define     TPrescalerReg         0x2B//
#define     TReloadRegH           0x2C//16λ��ʱ����װ��ֵ��λ
#define     TReloadRegL           0x2D//16λ��ʱ����װ��ֵ��λ
#define     TCounterValueRegH     0x2E//��ʾ��ʱ��ʵ�ʶ�ʱֵ��λ
#define     TCounterValueRegL     0x2F//��ʾ��ʱ��ʵ�ʶ�ʱ����λ
// PAGE 3      										//���ԼĴ���
#define     RFU30                 0x30//����λ
#define     TestSel1Reg           0x31//���ò����ź�����
#define     TestSel2Reg           0x32//���ò����ź����ü�PRBS����
#define     TestPinEnReg          0x33//ʹ��D1-D7��������
#define     TestPinValueReg       0x34//����D1-D7��ֵ������������I/O����ʱ
#define     TestBusReg            0x35//��ʾ���ڲ��������ߵ�״̬
#define     AutoTestReg           0x36//���������ź��Բ���
#define     VersionReg            0x37//��ʾ�汾
#define     AnalogTestReg         0x38//����AUX1��AUX2����
#define     TestDAC1Reg           0x39 //����TestDAC1�Ĳ���ֵ
#define     TestDAC2Reg           0x3A   ////����TestDAC2�Ĳ���ֵ
#define     TestADCReg            0x3B   //��ʾADC I��Q��ʵ��ֵ
#define     RFU3C                 0x3C   //����λ
#define     RFU3D                 0x3D  //����λ
#define     RFU3E                 0x3E   //����λ
#define     RFU3F								  0x3F//����λ

#define MAXRLEN 18 

enum 
{
	MI_OK = 0,
    MI_NOTAGERR= 0xff,
    MI_ERR = 0xfe,
    KEY_ERROR = 0xFD
};


#endif

