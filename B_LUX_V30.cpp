
#include "B_LUX_V30.h"  

#define TIMEOUT 10000  



B_LUX_V30::B_LUX_V30(uint8_t _sda, uint8_t _scl)
{
	SDA = _sda;
	SCL = _scl;
	pinMode(SCL, OUTPUT);
	pinMode(SDA, OUTPUT);
	val32 = 0;
}

float B_LUX_V30::read()
{
	//---------------------------������
	B_LUX_Start();
	B_LUX_SendByte(B_LUX_SlaveAddress + 0);         //�����豸��ַ+���ź�
	B_LUX_SendByte(0x00);

	B_LUX_Start();
	B_LUX_SendByte(B_LUX_SlaveAddress + 1);         //�����豸��ַ+���ź�
	delay(50);										//��ʱ
	for (uint8_t i = 0; i <= 4; i++)                        //������ȡ6����ַ���ݣ��洢��BUF
	{
		BUF_0[i] = B_LUX_RecvByte();                //BUF[0]�洢0x32��ַ�е�����
		if (i == 4)
		{
			B_LUX_SendACK(1);                       //���һ��������Ҫ��NOACK
		}
		else
		{
			B_LUX_SendACK(0);                       //��ӦACK
		}
	}
	B_LUX_Stop();									//ֹͣ�ź�

	val32 = BUF_0[3];
	val32 <<= 8;
	val32 |= BUF_0[2];
	val32 <<= 8;
	val32 |= BUF_0[1];
	val32 <<= 8;
	val32 |= BUF_0[0];
	//0x00 ~0x03Ϊ����ֵ�洢�Ĵ������洢32λ����ֵ, ����ֵ��ȷ��С����3λ������ֵ = �Ĵ���ֵ / 1000������267856 / 1000 = 267.856(ʵ�ʹ���ֵ)
	return ((float)val32*1.4) / 1000;//*1.4�������ֵ  
}

/*---------------------------------------------------------------------
��������: ��IIC���߷���һ���ֽ�����
����˵��: dat - д�ֽ�
��������: ��
---------------------------------------------------------------------*/
void B_LUX_V30::B_LUX_SendByte(uint8_t dat)
{
	pinMode(SDA, OUTPUT);
	pinMode(SCL, OUTPUT);
	for (uint8_t i = 0; i < 8; i++)         			//8λ������
	{
		if (dat & 0x80)	digitalWrite(SDA, HIGH);
		else			digitalWrite(SDA, LOW);         //�����ݿ� 
		delay(5);            				//��ʱ
		digitalWrite(SCL, HIGH);       		//����ʱ����
		delay(5);             				//��ʱ
		digitalWrite(SCL, LOW);           	//����ʱ����
		delay(5);				      		//��ʱ
		dat <<= 1;              			//�Ƴ����ݵ����λ
	}

	digitalWrite(SDA, HIGH);
	pinMode(SDA, INPUT);
	digitalWrite(SCL, HIGH);                //����ʱ����
	delay(5);								//��ʱ
	digitalRead(SDA);				        //��Ӧ���ź�
	delay(5);								//��ʱ
	digitalWrite(SCL, LOW);                 //����ʱ����
	pinMode(SDA, OUTPUT);
}


/*---------------------------------------------------------------------
��������: ��IIC���߽���һ���ֽ�����
����˵��: ��
��������: �����ֽ�
---------------------------------------------------------------------*/
uint8_t  B_LUX_V30::B_LUX_RecvByte()
{
	uint8_t dat = 0;
	pinMode(SDA, INPUT);
	digitalWrite(SDA, HIGH);                    //ʹ���ڲ�����,׼����ȡ����,
	for (uint8_t i = 0; i < 8; i++)         	        //8λ������
	{
		dat <<= 1;
		digitalWrite(SCL, HIGH);                //����ʱ����
		delay(5);             					//��ʱ
		dat |= digitalRead(SDA);				//������               
		digitalWrite(SCL, LOW);                 //����ʱ����
		delay(5);             					//��ʱ 
	}
	pinMode(SDA, OUTPUT);
	return dat;
}


/*---------------------------------------------------------------------
��������: ����Ӧ���ź�
����˵��: ack - Ӧ���ź�(0:ACK 1:NAK)
��������: ��
---------------------------------------------------------------------*/
void B_LUX_V30::B_LUX_SendACK(uint8_t ack)
{
	if (ack & 0x01)	digitalWrite(SDA, HIGH);		//дӦ���ź�
	else			digitalWrite(SDA, LOW);
	digitalWrite(SCL, HIGH);                        //����ʱ����
	delay(5);										//��ʱ
	digitalWrite(SCL, LOW);                         //����ʱ����
	digitalWrite(SDA, HIGH);
	delay(5);										//��ʱ
}


/*---------------------------------------------------------------------
��������: ��ʼ�ź�
����˵��: ��
��������: ��
---------------------------------------------------------------------*/
void B_LUX_V30::B_LUX_Start()
{
	digitalWrite(SDA, HIGH);                         //����������
	digitalWrite(SCL, HIGH);                         //����ʱ����
	delay(5);										//��ʱ
	digitalWrite(SDA, LOW);                         //�����½���
	delay(5);										//��ʱ
	digitalWrite(SCL, LOW);                         //����ʱ����
}

/*---------------------------------------------------------------------
��������: ֹͣ�ź�
����˵��: ��
��������: ��
---------------------------------------------------------------------*/
void  B_LUX_V30::B_LUX_Stop()
{
	digitalWrite(SDA, LOW);                         //����������
	digitalWrite(SCL, HIGH);                        //����ʱ����
	delay(5);										//��ʱ
	digitalWrite(SDA, HIGH);                        //����������
	delay(5);										//��ʱ
	digitalWrite(SCL, LOW);
	delay(180);
}