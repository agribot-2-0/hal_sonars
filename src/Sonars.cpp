/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */
#include "Sonars.h"

Sonars::Sonars(ros::NodeHandle* _nh) : nh(_nh)
{
  vLoadSonars();
  vCreatePublishers();
}

Sonars::~Sonars()
{
}

void Sonars::vLoadSonars()
{
  bool allParametersLoaded = false;
  uint8_t i = 0;
  std::string sonarField, typeField, topicField, idField;
  int id = 0;
  std::string topic, type;
  SonarDevice sonarDevTemp;
  BallufSonar balluf_sonar;
  MotecSonar motec_sonar;
  ROS_INFO_STREAM("[HAL Sonars] Loading sonars");
  // Check all sonars parameters, starting from 0
  while (!allParametersLoaded)
  {
    sonarField = "/sonars_node/sonar" + std::to_string(i);
    idField = sonarField + "_ID";
    topicField = sonarField + "_topic";
    typeField = sonarField + "_type";
    if (nh->hasParam(idField))
    {
      nh->getParam(idField, id);
      sonarDevTemp.ID = id;
      if (nh->hasParam(topicField) && nh->hasParam(typeField))
      {
        nh->getParam(topicField, topic);
        sonarDevTemp.topic = topic;
        nh->getParam(typeField, type);
        if (type == "balluf")
        {
          sonarDevTemp.vSetType(balluf_sonar);
        }
        else if (type == "motec")
        {
          sonarDevTemp.vSetType(motec_sonar);
        }
        else
          break;
        sonarsList.push_back(sonarDevTemp);
        ROS_INFO_STREAM("[HAL Sonars] ID: " << sonarsList[id].ID << " Topic: " << sonarsList[id].topic << " Type: "
                                            << sonarsList[id].type << "\n min range: " << sonarsList[id].min_range
                                            << "max range: " << sonarsList[id].max_range);
      }
      else
      {
        break;
      }
      ++i;
    }
    else
    {
      allParametersLoaded = true;
    }
  }
  if (!allParametersLoaded)
  {
    // Error in configuration - no threshold for this sonar
    ROS_ERROR_STREAM("[HAL Sonars] Error in configuration, last proper sonar ID read: " << id);
    ros::shutdown();
  }
}

void Sonars::vCreatePublishers()
{
  int ID = 0;
  for (auto& it : sonarsList)
  {
    // Create publisher shared ptr to avoid copying, when inserting into map
    std::shared_ptr<ros::Publisher> pub;
    pub = std::make_shared<ros::Publisher>();
    // Set up a publisher
    *pub = nh->advertise<sensor_msgs::Range>(it.topic, 1000);
    // and add Publisher to subscribers map
    std::pair<uint8_t, std::shared_ptr<ros::Publisher>> pubPair(ID, pub);
    sonarsPublishers.insert(pubPair);
    ++ID;
  }
}

void Sonars::vPublishSonarData(SonarMeasurment* sonar_data)
{
  sensor_msgs::Range RangeMsg;
  // Standard for every sensor
  RangeMsg.header.stamp = ros::Time::now();
  RangeMsg.header.frame_id = "sonars_link";
  RangeMsg.radiation_type = RangeMsg.ULTRASOUND;
  // from sonar specification
  RangeMsg.min_range = sonarsList[sonar_data->u8ID].min_range;
  RangeMsg.max_range = sonarsList[sonar_data->u8ID].max_range;
  RangeMsg.range = sonar_data->fRange;
  // Find the publisher
  std::unordered_map<uint8_t, std::shared_ptr<ros::Publisher>>::iterator pub;
  pub = sonarsPublishers.find(sonar_data->u8ID);
  if (pub == sonarsPublishers.end())
  {
    ROS_ERROR_STREAM("[HAL Sonars] Attempted to publish unregistered sonar with ID: " << sonar_data->u8ID);
  }
  else
  {
    pub->second->publish(RangeMsg);
  }
}
