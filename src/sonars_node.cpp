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
  nh.getParam("can_interface", can_interface);
  SonarsCanHandler SonarsCommHandler(can_interface.c_str());

  ros::Rate loop_rate(50);
  sensor_msgs::Range RangeMsg;
  uint8_t u8Status = 0x00;
  uint8_t u8SonarId = empty_id;
  float fSonarRange = out_of_range_value;
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
        SonarsCommHandler.vReturnSonarRange(&u8SonarId, &fSonarRange);
        if (u8SonarId == empty_id)
        {
          ROS_INFO("[Sonars] Sonars queue empty");
          break;
        }
        else
        {
          ROS_INFO("[Sonars] Reading sonars");
        }
      }
    }
    ros::spinOnce();
    loop_rate.sleep();
  }
}
