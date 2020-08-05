/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */
#include <ros/ros.h>
#include <ros/console.h>
#include <SonarsCanHandler.h>
#include <SonarTypes.h>
#include <Sonars.h>
#include <sensor_msgs/Range.h>

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
  Sonars SonarsHandler(&nh);
  SonarsCanHandler SonarsCommHandler(can_interface.c_str());
  ros::Rate loop_rate(50);
  uint8_t u8Status = 0x00;
  SonarMeasurment Measurement;
  while (ros::ok())
  {
    // stamp the msg
    SonarsCommHandler.vReceiveCanFrames();
    u8Status = SonarsCommHandler.u8ParseCanFrames();
    if ((u8Status & SonarsCommHandler.received_sonars_frame) == SonarsCommHandler.received_sonars_frame)
    {
      while (true)
      {
        SonarsCommHandler.vReturnSonarRange(&Measurement);
        if (Measurement.u8ID == SonarsCommHandler.empty_id)
        {
          break;
        }
        else
        {
          // send proper Range.msg depending on the sonar ID
          SonarsHandler.vPublishSonarData(&Measurement);
        }
      }
    }
    ros::spinOnce();
    loop_rate.sleep();
  }
}
