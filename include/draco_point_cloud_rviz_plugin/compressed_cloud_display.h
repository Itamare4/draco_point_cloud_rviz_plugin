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

#ifndef COMPRESSED_POINTCLOUD2_DISPLAY_H
#define COMPRESSED_POINTCLOUD2_DISPLAY_H

#include <boost/circular_buffer.hpp>

#include <sensor_msgs/PointCloud2.h>
#include <draco_point_cloud_transport/CompressedPointCloud2.h>
#include <rviz/message_filter_display.h>

namespace Ogre
{
class SceneNode;
}

namespace rviz
{
class ColorProperty;
class FloatProperty;
class IntProperty;
}

namespace draco_point_cloud_rviz_plugin
{

class PointCloudCommon;

class CompressedPointCloud2Display: public rviz::MessageFilterDisplay<draco_point_cloud_transport::CompressedPointCloud2>
{
  Q_OBJECT
  public:
    CompressedPointCloud2Display();
    ~CompressedPointCloud2Display();

    virtual void reset();

    virtual void update( float wall_dt, float ros_dt );

  private Q_SLOTS:
    void updateQueueSize();

  protected:
    /** @brief Do initialization. Overridden from MessageFilterDisplay. */
    virtual void onInitialize();

    /** @brief Process a single message.  Overridden from MessageFilterDisplay. */
    virtual void processMessage( const draco_point_cloud_transport::CompressedPointCloud2ConstPtr& message );

    rviz::IntProperty* queue_size_property_;

    std::shared_ptr<PointCloudCommon> point_cloud_common_;
};

}

#endif
