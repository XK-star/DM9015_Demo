#include "DM9015.h"
#include "BSP_Data.h"

CIRCLE_BUFF_t Que_DMfold = {0, 0, {0}};
CIRCLE_BUFF_t Que_DM_Tx  = {0, 0, {0}};

volatile Encoder GMYawEncoder = {0, 0, 0, 0, 0, 0, 0, 0, 0};
static int8_t frame_temp_DM[30];
	static int8_t DM_DMA_BUFF[200] = {0}; //DMAȡֵ������


/**���������ظ�(8byte) ��ʽ 1   ֡ͷ       0x3E
                            2   �����ֽ�   0xA0 0xA2 0xA3 0xA4
                            3   ID         0x01~0X04
                            4   ���ݳ���   0x02
                            5   У��λ     1-3λУ���
                           6.7  ���������� 0~4095
                            8   У��λ     5-6λУ���               **/

void GMfold_Res_Task()
{
	int16_t sum=0;
	static int16_t Ecoder;
		while(bufferlen(&Que_DMfold) != 0)
	{
		//Ѱ��֡ͷAD
		if(bufferPop(&Que_DMfold,frame_temp_DM) == 0x3E)  
		{
			buffer_multiPop(&Que_DMfold, &frame_temp_DM[1],6);
			sum=frame_temp_DM[5]+frame_temp_DM[6];
			if(bufferPop(&Que_DMfold,&frame_temp_DM[7]) ==sum&0xff )	
			{
				GMYawEncoder .last_raw_value =GMYawEncoder.raw_value;
				GMYawEncoder.raw_value=((int16_t)(frame_temp_DM[1] << 8 | frame_temp_DM[0]));
				EncoderProcess(&GMYawEncoder);
			}
		}
	}		
}

void EncoderProcess(Encoder *v)//ûд��
{
	int i = 0;
	int32_t temp_sum = 0;
	v->diff = v->raw_value - v->last_raw_value;
	if (v->diff < -3200)//���εı������Ƕ�ƫ��̫��
	{
			v->round_cnt++;
			v->ecd_raw_rate = v->diff + 4095;
		}
	else if (v->diff > 3200)
	{
			v->round_cnt--;
			v->ecd_raw_rate = v->diff - 4095;
		}
	else
	{
			v->ecd_raw_rate = v->diff;
		}
	//����õ������ı��������ֵ
	v->ecd_value = v->raw_value + v->round_cnt * 4095;
	//����õ��Ƕ�ֵ����Χ���������
	v->ecd_angle = (float)(v->raw_value - v->ecd_bias) * 360.0f / 4095 + v->round_cnt * 360;
	v->rate_buf[v->buf_count++] = v->ecd_raw_rate;
	if (v->buf_count == RATE_BUF_SIZE)
	{
			v->buf_count = 0;
	}	
	//�����ٶ�ƽ��ֵ
	for (i = 0; i < RATE_BUF_SIZE; i++)
	{
			temp_sum += v->rate_buf[i];
	}
	v->filter_rate = (int32_t)(temp_sum / RATE_BUF_SIZE);
}


/*************�������ƣ��������������Ķ�תŤ��*****************/ 
/******************int16_t POWER -850~+850***********************/
void DM_SendPower(int16_t power)
{
	int8_t Data[50];
  int16_t sum=0;	

	Data[0] = 0x3E;//ͷ�ֽ�
	Data[1] = 0xA0;//�����ֽ� Ťת����
	Data[2] = 0x01;//ID
	Data[3] = 0x02;//���ݳ���
	Data[4] = 0xE1;//1-4�ֽ�У���
	
	Data[5] = (int8_t)(power);
	Data[6] = (int8_t)(power>>8);//����
	
	for(uint8_t i=5;i<7;i++)		//���
	{
		sum += Data[i];
	}
	Data[7] = sum&0xFF;//д��У��λ

	for(int i = 0;i<8;i++)
	{
		bufferPush(&Que_DM_Tx, Data[i]);//����д�����
	}
	
	//


	uint8_t Bufferl_len = bufferlen(&Que_DM_Tx);//�������ݳ���
	buffer_multiPop(&Que_DM_Tx, DM_DMA_BUFF, Bufferl_len);//�Ӷ�����ȡ����������
	HAL_UART_Transmit_DMA(&huart2, DM_DMA_BUFF, Bufferl_len);//����DMA����  ���ڴ���huartx

}

/**************�ٶȱջ����ƣ�����ת�����ٶ�************************************/ 
/**************int32_t Angular Speed ʵ���ٶȱ�0.01dps/LSB*****************/
void DM_SendAngularSpeed(int32_t angularspeed)
{
	int8_t Data[50];
  int16_t sum=0;	

	Data[0] = 0x3E;//ͷ�ֽ�
	Data[1] = 0xA2;//�����ֽ� ���ٶ�
	Data[2] = 0x01;//ID
	Data[3] = 0x04;//���ݳ���
	Data[4] = 0xE5;//1-4�ֽ�У���
	
	Data[5] = (int8_t)(angularspeed);
	Data[6] = (int8_t)(angularspeed>>8);
	Data[7] = (int8_t)(angularspeed>>16);
	Data[8] = (int8_t)(angularspeed>>24);//����

	for(uint8_t i=5;i<9;i++)		//���
	{
		sum += Data[i];
	}
	Data[9] = sum&0xFF;//д��У��λ

	for(int i = 0;i<10;i++)
	{
		bufferPush(&Que_DM_Tx, Data[i]);//����д�����
	}
	
	//


	uint8_t Bufferl_len = bufferlen(&Que_DM_Tx);//�������ݳ���
	buffer_multiPop(&Que_DM_Tx, DM_DMA_BUFF, Bufferl_len);//�Ӷ�����ȡ����������
	HAL_UART_Transmit_DMA(&huart2, DM_DMA_BUFF, Bufferl_len);//����DMA����  ���ڴ���huartx

}

/***************λ�ñջ�����1������ת���Ƕ�************************************/ 
/**************int32_t Angle ʵ�ʽǶȱ�0.01 degree/LSB*****************/
void DM_SendAngle(int64_t angle)
{
	int8_t Data[50];
  int16_t sum=0;	

	Data[0] = 0x3E;//ͷ�ֽ�
	Data[1] = 0xA3;//�����ֽ� �Ƕ�
	Data[2] = 0x01;//ID
	Data[3] = 0x08;//���ݳ���
	Data[4] = 0xEA;//1-4�ֽ�У���
	
	Data[5] = (int8_t)(angle);
	Data[6] = (int8_t)(angle>>8);
	Data[7] = (int8_t)(angle>>16);
	Data[8] = (int8_t)(angle>>24);
	Data[9] = (int8_t)(angle>>32);
	Data[10] = (int8_t)(angle>>40);
	Data[11] = (int8_t)(angle>>48);
	Data[12] = (int8_t)(angle>>56);//����
	
	for(uint8_t i=5;i<13;i++)		//���
	{
		sum += Data[i];
	}
	Data[13] = sum&0xFF;//д��У��λ

	for(int i = 0;i<14;i++)
	{
		bufferPush(&Que_DM_Tx, Data[i]);//����д�����
	}
	
	//


	uint8_t Bufferl_len = bufferlen(&Que_DM_Tx);//�������ݳ���
	buffer_multiPop(&Que_DM_Tx, DM_DMA_BUFF, Bufferl_len);//�Ӷ�����ȡ����������
	HAL_UART_Transmit_DMA(&huart2, DM_DMA_BUFF, Bufferl_len);//����DMA����  ���ڴ���huartx

}	

/*****************λ�ñջ�����2�����ͽǶȺ�ת�����ٶ�*****************/
void DM_SendPosition(int64_t angle,int32_t angularspeed)
{
	int8_t Data[50];
  int16_t sum=0;	

	Data[0] = 0x3E;//ͷ�ֽ�
	Data[1] = 0xA4;//�����ֽ� ���ٶȺͽǶ�
	Data[2] = 0x01;//ID
	Data[3] = 0x0C;//���ݳ���
	Data[4] = 0xEF;//1-4�ֽ�У���
	
	Data[5] = (int8_t)(angle);;
	Data[6] = (int8_t)(angle>>8);
	Data[7] = (int8_t)(angle>>16);
	Data[8] = (int8_t)(angle>>24);
	Data[9] = (int8_t)(angle>>32);
	Data[10] = (int8_t)(angle>>40);
	Data[11] = (int8_t)(angle>>48);
	Data[12] = (int8_t)(angle>>56);
	Data[13] = (int8_t)(angularspeed);
	Data[14] = (int8_t)(angularspeed>>8);
	Data[15] = (int8_t)(angularspeed>>16);
	Data[16] = (int8_t)(angularspeed>>24);//����

	for(uint8_t i=5;i<17;i++)		//���
	{
		sum += Data[i];
	}
	Data[17] = sum&0xFF;//д��У��λ

	for(int i = 0;i<18;i++)
	{
		bufferPush(&Que_DM_Tx, Data[i]);//����д�����
	}
	
	//


	uint8_t Bufferl_len = bufferlen(&Que_DM_Tx);//�������ݳ���
	buffer_multiPop(&Que_DM_Tx, DM_DMA_BUFF, Bufferl_len);//�Ӷ�����ȡ����������
	HAL_UART_Transmit_DMA(&huart2, DM_DMA_BUFF, Bufferl_len);//����DMA����  ���ڴ���huartx

}