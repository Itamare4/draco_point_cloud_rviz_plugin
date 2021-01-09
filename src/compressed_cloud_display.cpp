/*
 * Copyright (c) 2012, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreSceneManager.h>

#include <tf/transform_listener.h>

#include <rviz/visualization_manager.h>
#include <rviz/properties/color_property.h>
#include <rviz/properties/float_property.h>
#include <rviz/properties/int_property.h>
#include <rviz/default_plugin/point_cloud_transformers.h>
#include <rviz/validate_floats.h>
#include <rviz/frame_manager.h>

#include <ros/package.h>

#include <draco_point_cloud_rviz_plugin/compressed_cloud_display.h>
#include <draco_point_cloud_rviz_plugin/point_cloud_common.h>
#include <draco_point_cloud_rviz_plugin/DracotoPC2.h>

#include "draco/compression/decode.h"
// #include <draco/point_cloud/point_cloud.h>

namespace draco_point_cloud_rviz_plugin
{

CompressedPointCloud2Display::CompressedPointCloud2Display() : point_cloud_common_( new PointCloudCommon( this ))
{
  queue_size_property_ = new rviz::IntProperty( "Queue Size", 10,
                                          "Advanced: set the size of the incoming PointCloud2 message queue. "
                                          " Increasing this is useful if your incoming TF data is delayed significantly "
                                          "from your PointCloud2 data, but it can greatly increase memory usage if the messages are big.",
                                          this, SLOT( updateQueueSize() ));

  // PointCloudCommon sets up a callback queue with a thread for each
  // instance.  Use that for processing incoming messages.
  update_nh_.setCallbackQueue( point_cloud_common_->getCallbackQueue() );
}

void CompressedPointCloud2Display::onInitialize()
{
  MFDClass::onInitialize();
  point_cloud_common_->initialize( context_, scene_node_ );
}

CompressedPointCloud2Display::~CompressedPointCloud2Display()
{
}

void CompressedPointCloud2Display::updateQueueSize()
{
  tf_filter_->setQueueSize( (uint32_t) queue_size_property_->getInt() );
}

void CompressedPointCloud2Display::processMessage( const draco_point_cloud_transport::CompressedPointCloud2ConstPtr& message )
{
  /* Taken from draco_point_cloud_transport/draco_subscriber.cpp by @paplhjak */
  // get size of buffer with compressed data in Bytes
  uint32_t compressed_data_size = message->compressed_data.size();

  draco::DecoderBuffer decode_buffer;
  std::vector <unsigned char> vec_data = (message->compressed_data);

  // Sets the buffer's internal data. Note that no copy of the input data is
  // made so the data owner needs to keep the data valid and unchanged for
  // runtime of the decoder.
  decode_buffer.Init(reinterpret_cast<const char *>(&vec_data[0]), compressed_data_size);
  draco::Decoder decoder;

  // decode buffer into draco point cloud
  std::unique_ptr<draco::PointCloud> decoded_pc =
          decoder.DecodePointCloudFromBuffer(&decode_buffer).value();

  // create and initiate converter object
  DracotoPC2 converter_b(std::move(decoded_pc), message);
  // convert draco point cloud to sensor_msgs::PointCloud2
  sensor_msgs::PointCloud2Ptr cloud( new sensor_msgs::PointCloud2( std::move(converter_b.convert()) ) );

  // Filter any nan values out of the cloud.  Any nan values that make it through to PointCloudBase
  // will get their points put off in lala land, but it means they still do get processed/rendered
  // which can be a big performance hit
  sensor_msgs::PointCloud2Ptr filtered(new sensor_msgs::PointCloud2);
  int32_t xi = rviz::findChannelIndex(cloud, "x");
  int32_t yi = rviz::findChannelIndex(cloud, "y");
  int32_t zi = rviz::findChannelIndex(cloud, "z");

  if (xi == -1 || yi == -1 || zi == -1)
  {
    return;
  }

  const uint32_t xoff = cloud->fields[xi].offset;
  const uint32_t yoff = cloud->fields[yi].offset;
  const uint32_t zoff = cloud->fields[zi].offset;
  const uint32_t point_step = cloud->point_step;
  const size_t point_count = cloud->width * cloud->height;

  if( point_count * point_step != cloud->data.size() )
  {
    std::stringstream ss;
    ss << "Data size (" << cloud->data.size() << " bytes) does not match width (" << cloud->width
       << ") times height (" << cloud->height << ") times point_step (" << point_step << ").  Dropping message.";
    setStatusStd( rviz::StatusProperty::Error, "Message", ss.str() );
    return;
  }

  filtered->data.resize(cloud->data.size());
  uint32_t output_count;
  if (point_count == 0)
  {
    output_count = 0;
  } else {
    uint8_t* output_ptr = &filtered->data.front();
    const uint8_t* ptr = &cloud->data.front(), *ptr_end = &cloud->data.back(), *ptr_init;
    size_t points_to_copy = 0;
    for (; ptr < ptr_end; ptr += point_step)
    {
      float x = *reinterpret_cast<const float*>(ptr + xoff);
      float y = *reinterpret_cast<const float*>(ptr + yoff);
      float z = *reinterpret_cast<const float*>(ptr + zoff);
      if (rviz::validateFloats(x) && rviz::validateFloats(y) && rviz::validateFloats(z))
      {
        if (points_to_copy == 0)
        {
          // Only memorize where to start copying from
          ptr_init = ptr;
          points_to_copy = 1;
        }
        else
        {
          ++points_to_copy;
        }
      }
      else
      {
        if (points_to_copy)
        {
          // Copy all the points that need to be copied
          memcpy(output_ptr, ptr_init, point_step*points_to_copy);
          output_ptr += point_step*points_to_copy;
          points_to_copy = 0;
        }
      }
    }
    // Don't forget to flush what needs to be copied
    if (points_to_copy)
    {
      memcpy(output_ptr, ptr_init, point_step*points_to_copy);
      output_ptr += point_step*points_to_copy;
    }
    output_count = (output_ptr - &filtered->data.front()) / point_step;
  }

  filtered->header = cloud->header;
  filtered->fields = cloud->fields;
  filtered->data.resize(output_count * point_step);
  filtered->height = 1;
  filtered->width = output_count;
  filtered->is_bigendian = cloud->is_bigendian;
  filtered->point_step = point_step;
  filtered->row_step = output_count;

  point_cloud_common_->addMessage( filtered );
}


void CompressedPointCloud2Display::update( float wall_dt, float ros_dt )
{
  point_cloud_common_->update( wall_dt, ros_dt );
}

void CompressedPointCloud2Display::reset()
{
  MFDClass::reset();
  point_cloud_common_->reset();
}

}

#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS(draco_point_cloud_rviz_plugin::CompressedPointCloud2Display,rviz::Display )