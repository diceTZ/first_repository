#include "pid.h"

#define ABS(x) ((x > 0) ? x : -x)

//@����			��tou_zi
//@��дʱ��	��2019��4��6��
//@�޸�ʱ��	��2019��4��6��
//@������		��initPID_Position_Module
//@��������	����ʼ��λ��ʽpidģ��
//@����1		��*pid	λ��ʽpidģ��ṹ��ָ��
//@����ֵ		����
void initPID_Position_Module(PID_Position_Module *pid)
{
	pid->flag.run = 0;
	pid->flag.integral_way = INTEGRAL_NORMAL;
	pid->flag.differential_way = DIFFERENTIAL_COMPLETE;
	pid->flag.use_predict = 0;
	
	pid->interface.data.target = 0;
	pid->interface.data.present = 0;
	pid->interface.data.present = 0;
	
	pid->parameter.kp = 0;
	pid->parameter.ki = 0;
	pid->parameter.kd = 0;
	pid->parameter.kf = 0;
	pid->parameter.kd_lpf = 0;
	pid->parameter.kd_pre = 0;
	pid->parameter.k_pre = 0;
	
	pid->parameter.target_limit = -1;
	pid->parameter.bias_limit	= -1;
	pid->parameter.bias_dead_zone = -1;
	pid->parameter.bias_for_integral = -1;
	pid->parameter.integral_limit = -1;
	pid->parameter.out_limit = -1;
	
	pid->data.out = 0;
	
	pid->process.bias = 0;
	pid->process.differential_bias = 0;
	pid->process.lpf_differential_bias = 0;
	pid->process.feedforward = 0;
	pid->process.integral_bias = 0;
	pid->process.last_bias = 0;
	pid->process.last_target = 0;
}

//@����			��tou_zi
//@��дʱ��	��2019��4��6��
//@�޸�ʱ��	��2019��4��6��
//@������		��calculatePID_Position_Module
//@��������	������λ��ʽpidģ��
//@����1		��*pid	λ��ʽpidģ��ṹ��ָ��
//@����2		��cycle ��������
//@����ֵ		����
void calculatePID_Position_Module(PID_Position_Module *pid, float cycle)
{	
	if (pid->flag.run == 0)
		return;
////////////////////////////////����ֵ�޷�
	if (pid->parameter.target_limit >= 0)
	{
		if (*(pid->interface.data.target) > pid->parameter.target_limit)
			*(pid->interface.data.target) = pid->parameter.target_limit;
		else if (*(pid->interface.data.target) < -pid->parameter.target_limit)
			*(pid->interface.data.target) = -pid->parameter.target_limit;
	}

////////////////////////////////ǰ������--ǰ��ֱֵ�Ӽ������
	pid->process.feedforward = pid->parameter.kf * *(pid->interface.data.target);
/////////////////////////////////////////////////////////////////////////////////
	
////////////////////////////////Ԥ�����--Ԥ��ֱֵ�Ӽ���ƫ��	
	if (pid->flag.use_predict == 0 || pid->interface.data.predict == 0)
		pid->process.predict = 0;
	else
		pid->process.predict = pid->parameter.k_pre * *pid->interface.data.predict * ABS(*pid->interface.data.predict);
/////////////////////////////////////////////////////////////////////////////////
	
////////////////////////////////ƫ�����	
	//��һ����Ϊ����
	//�ڶ�����Ϊ����
	//��������ΪԤ�⣨һ��Ϊ�ڻ�����,�ɲ����ã�
	float temp_bias = 	*(pid->interface.data.target) 	-  		
											*(pid->interface.data.present) 	- 
											pid->process.predict;
	
	if (pid->parameter.bias_dead_zone >= 0)
		temp_bias = (temp_bias < pid->parameter.bias_dead_zone && temp_bias > -pid->parameter.bias_dead_zone) ? 0 : temp_bias;	//��������ж�--���øù���ʱ���Խ�����-1
	
	if (pid->parameter.bias_limit >= 0)
	{
		temp_bias = (temp_bias > pid->parameter.bias_limit) ? pid->parameter.bias_limit : temp_bias;														//����޷�			--���øù���ʱ���Խ�����-1
		temp_bias = (temp_bias < -pid->parameter.bias_limit) ? -pid->parameter.bias_limit : temp_bias;
	}
	pid->process.bias = temp_bias;																																														//�������
/////////////////////////////////////////////////////////////////////////////////	
	
////////////////////////////////���ֲ���
	if (pid->parameter.ki == 0)
		pid->process.integral_bias = 0;
	
	else
		switch (pid->flag.integral_way)
		{
			case INTEGRAL_NORMAL: 
				//��ͨ����
				pid->process.integral_bias += pid->process.bias * cycle;	
			break;
			
			case INTEGRAL_SEPARATION:
				//���ַ���
				if (pid->process.bias > pid->parameter.bias_for_integral || pid->process.bias < -pid->parameter.bias_for_integral)
					break;
				
				pid->process.integral_bias += pid->process.bias * cycle;	
			break;

			case INTEGRAL_SATURATION:
				//�����ֱ���
				if (pid->process.integral_bias * pid->parameter.ki > pid->parameter.integral_limit)
					pid->process.integral_bias = pid->parameter.integral_limit / pid->parameter.ki; 
				
				else if (pid->process.integral_bias * pid->parameter.ki < -pid->parameter.integral_limit)
					pid->process.integral_bias = -pid->parameter.integral_limit / pid->parameter.ki; 
				
				else
					pid->process.integral_bias += pid->process.bias * cycle;
			break;
			
			case INTEGRAL_SPEED:
				//���ٻ��� -- ��������Ӻ��������㷨
				pid->process.integral_bias += (pid->process.bias + pid->process.last_bias) / (2.0f * cycle);	
			break;
			
			default:
				//Ĭ��Ϊ��ͨ����
				pid->process.integral_bias += pid->process.bias * cycle;	
			break;
		}																													
/////////////////////////////////////////////////////////////////////////////////	
	
////////////////////////////////΢�ֲ���	
	switch	(pid->flag.differential_way)
	{
		case DIFFERENTIAL_COMPLETE:
			//ֱ����΢��
			pid->process.lpf_differential_bias 
				= pid->process.differential_bias 
				= pid->process.bias - pid->process.last_bias;
		break;

		case DIFFERENTIAL_PART:
			//��΢�֣��ٵ�ͨ�˲�
			pid->process.differential_bias = pid->process.bias - pid->process.last_bias;
			pid->process.lpf_differential_bias += pid->parameter.kd_lpf * 3.14f * cycle 
																						* (pid->process.differential_bias - pid->process.lpf_differential_bias);
		break;
		
		case DIFFERENTIAL_PREVIOUS:
			//΢������
			pid->process.lpf_differential_bias 
				= pid->process.differential_bias 
				= (pid->process.bias - pid->process.last_bias) - pid->parameter.kd_pre * (*(pid->interface.data.target) - pid->process.last_target);
		break;
		
		default : 
			//ֱ����΢��
			pid->process.lpf_differential_bias 
				= pid->process.differential_bias 
				= pid->process.bias - pid->process.last_bias;
		break;
	}
/////////////////////////////////////////////////////////////////////////////////	
	
////////////////////////////////����ϳ�
	//�������
	pid->data.out = pid->parameter.kp * pid->process.bias +
									pid->parameter.ki * pid->process.integral_bias +
									pid->parameter.kd * pid->process.lpf_differential_bias / cycle + 	//���Խ�cycleע��
									pid->process.feedforward;
	
	//����޷�
	if (pid->parameter.out_limit >= 0)
	{
		if (pid->data.out > pid->parameter.out_limit)
			pid->data.out = pid->parameter.out_limit;
		
		else if (pid->data.out < -pid->parameter.out_limit)
			pid->data.out = -pid->parameter.out_limit;
	}
	
	//�洢��ȥֵ
	pid->process.last_target = *(pid->interface.data.target);
	pid->process.last_bias = pid->process.bias;
}

