/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */
#include <ros/ros.h>
#include <ros/console.h>
#include <sensor_msgs/Range.h>
#include <SonarsCanHandler.h>
#include <SonarTypes.h>

int main(int argc, char** argv)
{
  ros::init(argc, argv, "sonars");
  ros::NodeHandle nh("~");
  std::string can_interface;
  if (nh.hasParam("/sonars_node/can_interface"))
  {
    nh.getParam("/sonars_node/can_interface", can_interface);
  }
  else
  {
    ROS_INFO("[Sonars] Missing parameter specifying can_interface, setting the default value: can0");
    can_interface = "can0";
  }
  SonarsCanHandler SonarsCommHandler(can_interface.c_str());
  ros::Rate loop_rate(50);
  sensor_msgs::Range RangeMsg;
  uint8_t u8Status = 0x00;
  SonarMeasurment Measurement;
  // Standard for every sensor
  RangeMsg.header.frame_id = "sonars_link";
  RangeMsg.radiation_type = RangeMsg.ULTRASOUND;
  while (ros::ok())
  {
    // stamp the msg
    RangeMsg.header.stamp = ros::Time::now();
    SonarsCommHandler.vReceiveCanFrames();
    u8Status = SonarsCommHandler.u8ParseCanFrames();
    if ((u8Status & received_sonars_frame) == received_sonars_frame)
    {
      while (true)
      {
        SonarsCommHandler.vReturnSonarRange(&Measurement);
        if (Measurement.u8ID == empty_id)
        {
          ROS_INFO("[Sonars] Sonars queue empty");
          break;
        }
        else
        {
          // send proper Range.msg depending on the sonar ID
          ROS_INFO("[Sonars] Reading sonars");
        }
      }
    }
    ros::spinOnce();
    loop_rate.sleep();
  }
}
