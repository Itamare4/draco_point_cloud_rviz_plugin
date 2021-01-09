/* Taken from draco_point_cloud_transport by @paplhjak */

#include <draco_point_cloud_rviz_plugin/DracotoPC2.h>

namespace draco_point_cloud_rviz_plugin
{
    //! Constructor
    DracotoPC2::DracotoPC2(std::unique_ptr<draco::PointCloud> && pc, const draco_point_cloud_transport::CompressedPointCloud2ConstPtr & compressed_PC2)
    {
        pc_=std::move(pc);
        compressed_PC2_ =  compressed_PC2;
    }

    //! Destructor
    //DracotoPC2::~DracotoPC2() {}

    //! Method for converting into sensor_msgs::PointCloud2
    sensor_msgs::PointCloud2 DracotoPC2::DracotoPC2::convert(){

        // number of all attributes of point cloud
        int32_t number_of_attributes = pc_->num_attributes();

        // number of points in pointcloud
        draco::PointIndex::ValueType number_of_points = pc_->num_points();

        std::vector<uint8_t> vec_data;
        vec_data.resize(number_of_points*compressed_PC2_->point_step);




        // for each attribute
        for (uint32_t att_id = 0 ; att_id < number_of_attributes ; att_id++)
        {
            // get attribute
            const draco::PointAttribute* attribute = pc_->attribute(att_id);

            // check if attribute is valid
            if (!attribute->IsValid()){
                // RAISE ERROR - buffer of attribute is empty
                ROS_FATAL_STREAM("In point_cloud_transport::DracotoPC2, attribute of Draco pointcloud is not valid!") ;
            }

            // get offset of attribute in data structure
            uint32_t attribute_offset = compressed_PC2_->fields[att_id].offset;

            // for each point in point cloud
            for (draco::PointIndex::ValueType point_index = 0; point_index < number_of_points; point_index++)
            {
                // get pointer to corresponding memory in PointCloud2 data
                uint8_t *out_data = &vec_data[int(compressed_PC2_->point_step*point_index + attribute_offset)];

                // read value from Draco pointcloud to out_data
                attribute->GetValue(draco::AttributeValueIndex(point_index), out_data);
            }
        }


        // Create PointCloud2 structure to be filled up
        sensor_msgs::PointCloud2 PC2;

        // copy PointCloud2 description (header, width, ...)
        assign_description_of_PointCloud2(PC2, compressed_PC2_);

        // if points were deduplicated, overwrite height and width
        int deduplicate;
        pc_->metadata()->GetEntryInt("deduplicate", &deduplicate);

        if (deduplicate == 1)
        {
            PC2.width=number_of_points;
            PC2.height=1;
        }

        PC2.data = vec_data;

    return std::move(PC2);
    //return PC2;
    }
}