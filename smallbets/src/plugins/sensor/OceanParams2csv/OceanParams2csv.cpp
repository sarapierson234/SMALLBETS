/*!
 * @file
 *
 * @section LICENSE
 *
 * Copyright (C) 2017 by the Georgia Tech Research Institute (GTRI)
 *
 * This file is part of SCRIMMAGE.
 *
 *   SCRIMMAGE is free software: you can redistribute it and/or modify it under
 *   the terms of the GNU Lesser General Public License as published by the
 *   Free Software Foundation, either version 3 of the License, or (at your
 *   option) any later version.
 *
 *   SCRIMMAGE is distributed in the hope that it will be useful, but WITHOUT
 *   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 *   License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with SCRIMMAGE.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Kevin DeMarco <kevin.demarco@gtri.gatech.edu>
 * @author Eric Squires <eric.squires@gtri.gatech.edu>
 * @date 31 July 2017
 * @version 0.1.0
 * @brief Brief file description.
 * @section DESCRIPTION
 * A Long description goes here.
 *
 */

#include <smallbets/plugins/sensor/OceanParams2csv/OceanParams2csv.h>
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/entity/Contact.h>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/pubsub/Message.h>
#include <scrimmage/math/State.h>
#include <scrimmage/common/CSV.h>
#include <scrimmage/parse/ParseUtils.h>
#include <scrimmage/parse/MissionParse.h>
#include <scrimmage/proto/Shape.pb.h>
#include <scrimmage/math/Quaternion.h>
#include <scrimmage/pubsub/Subscriber.h>
#include <smallbets/msgs/Ocean.pb.h>

#include <boost/optional.hpp>

#include <iostream>
#include <limits>

using std::cout;
using std::endl;

namespace sc = scrimmage;
namespace sp = scrimmage_proto;

REGISTER_PLUGIN(scrimmage::Sensor,
                scrimmage::sensor::OceanParams2csv,
                OceanParams2csv_plugin)

namespace scrimmage {
namespace sensor {

OceanParams2csv::OceanParams2csv() {
}

void OceanParams2csv::init(std::map<std::string, std::string> &params) {
    range_length = sc::get<double>("range_length", params, 50);
    depth_length = sc::get<double>("depth_length", params, 50);
    x_pos.resize(range_length);//Initialize Vector Length
    y_pos.resize(range_length);//Initialize Vector Length
    depth.resize(range_length);//Initialize Vector Length
    z_pos.resize(depth_length);//Initialize Vector Length
    c_z.resize(depth_length);//Initialize Vector Length
    auto oceanParam_cb = [&] (auto &msg) {
        paramNew_ = std::make_shared<scrimmage::interaction::OceanMap>(msg->data);
    };
    subscribe<smallbets::msgs::Ocean>("GlobalNetwork", "Ocean", oceanParam_cb);
    
    auto terrain_map_cb = [&] (auto &msg) {
        map_ = std::make_shared<scrimmage::interaction::TerrainMap>(msg->data);
    };
    subscribe<scrimmage_msgs::Terrain>("GlobalNetwork", "Terrain", terrain_map_cb);
}

bool OceanParams2csv::step() {
     // Make a copy of the current state
    scrimmage::CSV csv;
    auto msg = std::make_shared<Message<ContactMap>>();//Create contact map of other UUVs

    if (map_ == nullptr) return true;
    if (paramNew_ == nullptr) return true;//// Wait until we have received a valid ocean params
    // Need (x1,y1,z1) [position of parent]
    source_x = parent_->state_truth()->pos()(0);
    source_y = parent_->state_truth()->pos()(1);
    source_z = parent_->state_truth()->pos()(2);

    // Create noisy versions of all contacts  //If multiple contacts this probably breaks
    for (auto &kv: (*parent_->contacts())) {
        // Ignore own position
        if (kv.first == parent_->id().id()) {
            continue;
        }
        Contact &cnt = kv.second;

        // (x2,y2,z2) [Postion of UUV2]
        rcvr_x = cnt.state()->pos()(0);
        rcvr_y = cnt.state()->pos()(1);
        rcvr_z = cnt.state()->pos()(2);
    }
    //
    // (x1 + (x2-x1)t, y1 + (y2 - y1)*t)
    // t 1/n [n number of points to output between vehicles]
    // then write csv file to grab the ocean params and depth at each (x_t, y_t, z_all)
    boost::optional<double> height;
  
    for ( int idx_t = 0; idx_t < range_length; idx_t++ ) {
        x_pos[idx_t] = ( source_x + ( rcvr_x - source_x ) * ( idx_t + 1 ) / range_length );
        y_pos[idx_t] = ( source_y + ( rcvr_y - source_y ) * ( idx_t + 1 ) / range_length );
        height = map_->height_at(x_pos[idx_t],y_pos[idx_t]);
        depth[idx_t] = *height;
    }
    double z_max = *(std::min_element(depth.begin(), depth.end()));
    //std::cout << "Z Max is: " << z_max << std::endl;
    //for ( int idx_t = 0; idx_t < range_length; idx_t++ ) {    
        for ( int idx_z = 0; idx_z < depth_length; idx_z++) {
            z_pos[idx_z] = (z_max) * idx_z / (depth_length-1);
            double c_tmp = paramNew_->param_at(x_pos[0],y_pos[0],z_pos[idx_z]);
            c_z[idx_z] = c_tmp;
        }
    //}
    // Write the CSV file to the root log directory
        std::string filename = parent_->mp()->log_dir() + "/" + "OceanParam.csv";
    // Print to csv file....
        // Create the file
        if (!csv.open_output(filename)) {
            std::cout << "Couldn't create output file" << endl;
            return false;
        }
        csv.set_column_headers("x1,y1,z1,x2,y2,z2,x_slice,y_slice,depth_slice,z,c_z");
        if (range_length > depth_length){
            unsigned idx_z;
            for ( unsigned idx_t = 0; idx_t < range_length; idx_t++ ) {
                if (idx_t > depth_length) {
                    idx_z = depth_length;
                } else {idx_z = idx_t;}
                csv.append(sc::CSV::Pairs{{"x1", source_x},
                    {"y1", source_y},
                    {"z1", source_z},
                    {"x2", rcvr_x},
                    {"y2", rcvr_y},
                    {"z2", rcvr_z},
                    {"x_slice", x_pos[idx_t]},
                    {"y_slice", y_pos[idx_t]},
                    {"depth_slice", depth[idx_t]},
                    {"z", z_pos[idx_z]},
                    {"c_z", c_z[idx_z]}});
            }
        } else {
            unsigned idx_t;
            for ( unsigned idx_z = 0; idx_z < depth_length; idx_z++ ) {
                if (idx_z > range_length) {
                    idx_t = range_length;
                } else {idx_t = idx_z;}
                csv.append(sc::CSV::Pairs{{"x1", source_x},
                    {"y1", source_y},
                    {"z1", source_z},
                    {"x2", rcvr_x},
                    {"y2", rcvr_y},
                    {"z2", rcvr_z},
                    {"x_slice", x_pos[idx_t]},
                    {"y_slice", y_pos[idx_t]},
                    {"depth_slice", depth[idx_t]},
                    {"z", z_pos[idx_z]},
                    {"c_z", c_z[idx_z]}});
            }
        }

        csv.close_output();
    std::cout << "Ocean Params Saved" <<std::endl;
    return true;
}
} // namespace sensor
} // namespace scrimmage
