// rosrun rosserial_arduino serial_node.py _port:=/dev/ttyACM0
#define USE_USBCON //dueで通信するときに必要
#include <ros.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Int32MultiArray.h>
#include <Servo.h>

//--------ros設定--------------
ros::NodeHandle nh;
std_msgs::Int32MultiArray pub_data;
ros::Publisher arduino_data("arduino_data", &pub_data);

//---ブラシレスモーター設定---------
Servo brushlessmotor1;
Servo brushlessmotor2;
Servo brushlessmotor3;
Servo brushlessmotor4;

int brushless1_command;
int brushless2_command;
int brushless3_command;
int brushless4_command;

int start_time = millis();

// int vertical_thrust_to_command(float thrust){
// 	int command;
// 	float x1=thrust;
// 	float x2 = thrust * x1;
// 	if(thrust<1.83){
// 		return 0;
// 	}else if(thrust>34.5){
// 		return 1600;
// 	}else{
// 		return 1180.3 + 17.376 * x1 - 0.1581 * x2;
// 	}
// }
//サーボのコマンドを2000に制限
int limit_servo_command_value(float value)
{
	float limit_max = 2000.0;
	if (value >= limit_max)
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
void yawCallback(const std_msgs::Float32MultiArray &command_value)
{
	brushless1_command = limit_servo_command_value(command_value.data[0] + command_value.data[2] / 2.0);
	brushless2_command = limit_servo_command_value(command_value.data[0] - command_value.data[2] / 2.0);
	brushless3_command = limit_servo_command_value(command_value.data[0] + command_value.data[2] / 2.0);
	brushless4_command = limit_servo_command_value(command_value.data[0] - command_value.data[2] / 2.0);
	brushlessmotor1.writeMicroseconds(brushless1_command);
	brushlessmotor2.writeMicroseconds(brushless2_command);
	brushlessmotor3.writeMicroseconds(brushless3_command);
	brushlessmotor4.writeMicroseconds(brushless4_command);
	nh.loginfo(String(brushless1_command).c_str());
}

ros::Subscriber<std_msgs::Float32MultiArray> sub("yaw_command", &yawCallback);
void setup()
{
	pub_data.data_length = 1;
	pub_data.data = (int32_t *)malloc(sizeof(int32_t) * 1);
	nh.initNode();
	nh.advertise(arduino_data);
	nh.subscribe(sub);
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
}

void loop()
{
	if (!nh.connected())
	{ //-----------------------------------------------
		brushless1_command = 1000;
		brushless2_command = 1000;
		brushless3_command = 1000;
		brushless4_command = 1000;
		brushlessmotor1.writeMicroseconds(brushless1_command);
		brushlessmotor2.writeMicroseconds(brushless2_command);
		brushlessmotor3.writeMicroseconds(brushless3_command);
		brushlessmotor4.writeMicroseconds(brushless4_command);
	}
	else
	{
		pub_data.data[0] = analogRead(A0);
		arduino_data.publish(&pub_data);
	}
	nh.spinOnce();
	delayMicroseconds(500);
}
