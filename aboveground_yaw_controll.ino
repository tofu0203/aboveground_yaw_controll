// rosrun rosserial_arduino serial_node.py _port:=/dev/ttyACM0
#define USE_USBCON //dueで通信するときに必要
#include <ros.h>
#include <std_msgs/Float32.h>
#include <Servo.h>

//--------ros設定--------------
ros::NodeHandle node;
std_msgs::Float32 yaw_sub_value;

//---ブラシレスモーター設定---------
Servo brushlessmotor1;
Servo brushlessmotor2;
Servo brushlessmotor3;
Servo brushlessmotor4;

float standard_throttle = 1300.0; //機体の自重分のスロットル
int brushless1_command;
int brushless2_command;
int brushless3_command;
int brushless4_command;

int start_time = millis();

//サーボのコマンドを1100から1900に制限
int limit_servo_command_value(float value)
{
	float limit_max = 1900.0;
	float limit_min = 1100.0;
	if (value <= limit_min)
	{
		return (int)limit_min;
	}
	else if (value >= limit_max)
	{
		return (int)limit_max;
	}
	else
	{
		return (int)value;
	}
}

//コールバック関数
//自重分のスロットル+yaw制御分にリミットをかけモータに出力
void yawCallback(const std_msgs::Float32 &command_value)
{
	if (millis() < start_time + 10000)
	{
		brushless1_command = 1000;
		brushless2_command = 1000;
		brushless3_command = 1000;
		brushless4_command = 1000;
	}
	else
	{
		brushless1_command = limit_servo_command_value(standard_throttle + command_value.data / 2.0);
		brushless2_command = limit_servo_command_value(standard_throttle - command_value.data / 2.0);
		brushless3_command = limit_servo_command_value(standard_throttle + command_value.data / 2.0);
		brushless4_command = limit_servo_command_value(standard_throttle - command_value.data / 2.0);
	}
	brushlessmotor1.writeMicroseconds(brushless1_command);
	brushlessmotor2.writeMicroseconds(brushless2_command);
	brushlessmotor3.writeMicroseconds(brushless3_command);
	brushlessmotor4.writeMicroseconds(brushless4_command);
	node.loginfo(String(brushless1_command).c_str());
}

ros::Subscriber<std_msgs::Float32> sub("yaw_command", &yawCallback);
void setup()
{
	//-----------------------------------
	brushlessmotor1.attach(10);
	brushlessmotor2.attach(11);
	brushlessmotor3.attach(12);
	brushlessmotor4.attach(13);
	brushless1_command = 1000;
	brushless2_command = 1000;
	brushless3_command = 1000;
	brushless4_command = 1000;
	brushlessmotor1.writeMicroseconds(brushless1_command);
	brushlessmotor2.writeMicroseconds(brushless2_command);
	brushlessmotor3.writeMicroseconds(brushless3_command);
	brushlessmotor4.writeMicroseconds(brushless4_command);
	node.getHardware()->setBaud(115200);
	node.initNode();
	node.subscribe(sub);
	node.loginfo("Set Up END!!");
}

void loop()
{
	node.spinOnce();
	delayMicroseconds(500);
}
