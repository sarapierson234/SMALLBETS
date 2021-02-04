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
#include <scrimmage/plugins/interaction/TerrainGenerator/TerrainMap.h>
#include <scrimmage/plugins/interaction/TerrainGenerator/TerrainGenerator.h>
#include <smallbets/plugins/interaction/OceanParameters/OceanMap.h>
#include <smallbets/plugins/interaction/OceanParameters/OceanParameters.h>


#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/entity/Contact.h>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/math/State.h>
#include <scrimmage/common/CSV.h>
#include <scrimmage/parse/ParseUtils.h>
#include <scrimmage/parse/MissionParse.h>
#include <scrimmage/pubsub/Subscriber.h>
#include <scrimmage/msgs/Terrain.pb.h>
#include <smallbets/msgs/Ocean.pb.h>

#include <scrimmage/pubsub/Message.h>
#include <scrimmage/proto/State.pb.h>

#include <iostream>
#include <limits>

#include <boost/optional.hpp>

#include <iostream>
#include <limits>

using std::cout;
using std::endl;

namespace sc = scrimmage;

REGISTER_PLUGIN(scrimmage::Sensor,
                scrimmage::sensor::OceanParams2csv,
                OceanParams2csv_plugin)

namespace scrimmage {
namespace sensor {

OceanParams2csv::OceanParams2csv() {
}

void OceanParams2csv::init(std::map<std::string, std::string> &params) {
    range_length = sc::get<double>("range_length", params, 50.0);

    auto oceanParam_cb = [&] (auto &msg) {
        paramNEW_ = std::make_shared<scrimmage::interaction::OceanMap>(msg->data);
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

    if (map_ == nullptr){std::cout << "Code is here" << std::endl;} return true;// Wait until we have received a valid terrain map
    if (paramNEW_ == nullptr) return true;//// Wait until we have received a valid ocean params
    std::cout << "Code is there" << std::endl;
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
    for ( int idx_t = 0; idx_t < range_length; idx_t++ ) {
        x_pos[idx_t] = ( source_x + ( rcvr_x - source_x ) * ( idx_t + 1 ) / range_length );
        y_pos[idx_t] = ( source_y + ( rcvr_y - source_y ) * ( idx_t + 1 ) / range_length );
        depth[idx_t] = *(map_->height_at(x_pos[idx_t],y_pos[idx_t]));
    }

    // Write the CSV file to the root log directory
        std::string filename = ".scrimmage/logs/latest/OceanParam.csv";
    // Print to csv file....
        // Create the file
        if (csv.read_csv(filename)) {
            return true;
        }
        if (!csv.open_output(filename)) {
            std::cout << "Couldn't create output file" << endl;
            return false;
        }
        csv.set_column_headers("x1,y1,z1,x2,y2,z2,x,y,z,depth,sound_speed");
        for ( unsigned idx_t = 0; idx_t < range_length; idx_t++ ) {
            csv.append(sc::CSV::Pairs{{"x1", source_x},
                    {"y1", source_y},
                    {"z1", source_z},
                    {"x2", rcvr_x},
                    {"y2", rcvr_y},
                    {"z2", rcvr_z},
                    {"x", x_pos[idx_t]},
                    {"y", y_pos[idx_t]},
                    {"z", depth[idx_t]},
                    {"depth", depth[idx_t]},
                    {"sound_speed", depth[idx_t]}});
        }
        csv.close_output();
    std::cout << "Ocean Params Saved" <<std::endl;
    return true;
}
} // namespace sensor
} // namespace scrimmage
