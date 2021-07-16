#include <iostream>
#include <map>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include "io.hpp"

#define CONV_FACTOR 0.5

const std::string info_msg =
	"Reading from the keyboard and publishing to /cmd_vel\n\
---------------------------\n\
Moving around:\n\
   u    i    o\n\
   j    k    l\n\
   m    ,    .\n\
\n\
t : up (+z)\n\
b : down (-z)\n\
\n\
Anything else : stop\n\
\n\
CTRL+C followed by ENTER to quit\n\
";

std::map<char, std::array<int, 4>> move_bindings = {
	{'i', {1, 0, 0, 0}},
	{'o', {1, 0, 0, -5}},
	{'j', {0, 0, 0, 5}},
	{'l', {0, 0, 0, -5}},
	{'u', {1, 0, 0, 5}},
	{',', {-1, 0, 0, 0}},
	{'.', {-1, 0, 0, 5}},
	{'m', {-1, 0, 0, -5}},
	{'t', {0, 0, 1, 0}},
	{'b', {0, 0, -1, 0}},
};

geometry_msgs::Twist add(const geometry_msgs::Twist &original_twist, const std::array<int, 4> &move_modifier);

void initialize_twist(geometry_msgs::Twist &vel)
{
	vel.linear.x = 0;
	vel.linear.y = 0;
	vel.linear.z = 0;
	vel.angular.x = 0;
	vel.angular.y = 0;
	vel.angular.z = 0;
}

int main(int argc, char *argv[])
{
	ros::init(argc, argv, "virat_teleop");

	ros::NodeHandle teleop_nh;

	ros::Publisher pub = teleop_nh.advertise<geometry_msgs::Twist>("/cmd_vel", 100);

	geometry_msgs::Twist pub_vel;
	ros::Rate rate(10);

	char opcode = 0;

	initialize_twist(pub_vel);

	std::cout << info_msg << std::endl;

	while (ros::ok())
	{
		opcode = get_invisible_ch();

		if (opcode == 3) // CTRL+C
		{
			return 0;
		}

		if (move_bindings.count(opcode))
		{
			pub_vel = add(pub_vel, move_bindings[opcode]);
		}
		else
		{
			initialize_twist(pub_vel);
		}

		pub.publish(pub_vel);

		ros::spinOnce();
		rate.sleep();
	}
}

geometry_msgs::Twist add(const geometry_msgs::Twist &original_twist, const std::array<int, 4> &move_modifier)
{
	geometry_msgs::Twist result;

	result = original_twist;

	result.linear.x += CONV_FACTOR*move_modifier[0];
	result.linear.y += CONV_FACTOR*move_modifier[1];
	result.linear.z += CONV_FACTOR*move_modifier[2];
	result.angular.z += CONV_FACTOR*move_modifier[3];

	return result;
}