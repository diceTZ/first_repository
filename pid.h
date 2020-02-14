//@����			��tou_zi
//@��дʱ��	��2019��4��6��
//@�޸�ʱ��	��2019��4��6��
//@�ļ���		��pid.h
//@����			��pidģ��⣬��ʵ��pid�ļ���
#ifndef _PID_H
#define _PID_H

#ifndef u8
#define u8 unsigned char
#endif

#define INTEGRAL_NORMAL				0			//��ͨ����
#define INTEGRAL_SEPARATION		1			//���ַ���
#define INTEGRAL_SATURATION 	2			//�����ͻ���
#define INTEGRAL_SPEED				3			//���ٻ���	

#define DIFFERENTIAL_COMPLETE	0			//��ȫ΢��
#define DIFFERENTIAL_PART			1			//����ȫ΢��
#define DIFFERENTIAL_PREVIOUS 2			//΢������

typedef struct PID_Position_Flag
{
	u8 run;
	u8 integral_way;
	u8 differential_way;
	u8 use_predict;
}PID_Position_Flag;

typedef struct PID_Position_DInterface
{
	float *target;
	float *present;
	float *predict;
}PID_Position_DInterface;

typedef struct PID_Position_Interface
{
	PID_Position_DInterface data;
}PID_Position_Interface;

typedef struct PID_Position_Parameter
{
	float kp;								//����ϵ��
	float ki;								//����ϵ��
	float kd;								//΢��ϵ��
	float kf;								//ǰ��ϵ��
	float kd_lpf;						//����ȫ΢��ϵ��
	float kd_pre;						//΢������ϵ��
	float k_pre;						//Ԥ��ϵ��
	
	float target_limit;				//Ŀ��ֵ�޷�
	float bias_limit;					//����޷�
	float bias_dead_zone;			//С�����ֵ��������PID����
	float bias_for_integral;	//��ʼ���ֵ����	--	���ڻ��ַ���
	float integral_limit;			//�����޷�				--	���ڿ����ֱ���
	float out_limit;					//����޷�
}PID_Position_Parameter;

typedef struct PID_Position_Data
{	
	float out;
}PID_Position_Data;

typedef struct PID_Position_Process
{
	float bias;
	float integral_bias;
	float differential_bias;
	float lpf_differential_bias;
	
	float feedforward;
	float predict;
	
	float last_target;
	float last_bias;
}PID_Position_Process;

typedef struct PID_Position_Module
{
	PID_Position_Flag		flag;
	PID_Position_Interface 	interface;
	PID_Position_Parameter	parameter;
	PID_Position_Data 		data;
	PID_Position_Process 	process;
}PID_Position_Module;

void initPID_Position_Module(PID_Position_Module *pid);
void calculatePID_Position_Module(PID_Position_Module *pid, float cycle);
#endif


