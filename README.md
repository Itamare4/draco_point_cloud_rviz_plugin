DRACO PointCloud2 RViz Plugin
------------------------

![](https://www.rootiebot.com/github/draco_point_cloud_rviz_plugin.gif)

Plugin for the ROS (Robot Operating System) visualizer RViz, to display draco PointCloud2 messages.
This plugin adds a new display type, named `PointCloud2Draco`.

This plugin is an duplication of the built-in PointCloud2 plugin, and likewise it receives messages of type `draco_point_cloud_transport/CompressedPointCloud2`.
### Credit to Jakub Paplham (@paplhjak) and Draco Google Team ###

This package is nothing but a wrapper over those two packages which makes the visualization on RViz seamless without the need of running an additional node to run the conversion on the host side.

### TODOs ###
* Port to ROS2
