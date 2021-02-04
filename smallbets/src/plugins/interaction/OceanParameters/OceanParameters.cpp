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
 * MODIFIED BY:
 * @author Sara Pierson <spierson@gatech.edu>
 * @date Novermber 2020
 * @section DESCRIPTION
 * This plugin will get information from the terrain generator to create a sudo realistic/munk ocean profile
%This environmental information will be published to scrimmage memory (a global variable?) and then a sensor model will subscribe to the 
%published information. The sensor will get the info, then write it out in a txt/csv file so that it can be given to BELLHOP for reduction/
%sound analysis  *
 */

#include <smallbets/plugins/interaction/OceanParameters/OceanParameters.h>

#include <scrimmage/common/Utilities.h>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/math/State.h>
#include <scrimmage/parse/ParseUtils.h>
#include <scrimmage/common/Random.h>
#include <scrimmage/proto/Shape.pb.h>
#include <scrimmage/pubsub/Publisher.h>

#include <memory>
#include <limits>
#include <iostream>

using std::cout;
using std::endl;

namespace sc = scrimmage;
namespace sp = scrimmage_proto;

REGISTER_PLUGIN(scrimmage::EntityInteraction,
                scrimmage::interaction::OceanParameters,
                OceanParameters_plugin)

namespace scrimmage {
namespace interaction {

OceanParameters::OceanParameters() {
}

bool OceanParameters::init(std::map<std::string, std::string> &mission_params,
                            std::map<std::string, std::string> &plugin_params) {
    double x_length = sc::get<double>("x_length", plugin_params, 100.0);
    double y_length = sc::get<double>("y_length", plugin_params, 100.0);
    double z_length = sc::get<double>("z_length", plugin_params, 50.0);
    double x_resolution = sc::get<double>("x_resolution", plugin_params, 1.0);
    double y_resolution = sc::get<double>("y_resolution", plugin_params, 1.0);
    double z_resolution = sc::get<double>("z_resolution", plugin_params, 1.0);

    std::string technique_str = sc::get("technique", plugin_params, "Munk");
    OceanMap::Technique technique;
    if (technique_str == "Munk") {
        technique = OceanMap::Technique::MUNK;
    } else {
        cout << "WARNING: Invalid Ocean Parameter technique" << endl;
        technique = OceanMap::Technique::MUNK;
    }

    map_ = OceanMap(technique, x_length, y_length, z_length,
                        x_resolution, y_resolution, z_resolution);
    
    ocean_pub_ = advertise("GlobalNetwork", "Ocean");

    return true;
}


bool OceanParameters::step_entity_interaction(std::list<sc::EntityPtr> &ents,
                                                  double t, double dt) {
    if (not ocean_published_) {
        ocean_published_ = true;
        // Publish the Ocean protobuf message
        auto msg = std::make_shared<sc::Message<smallbets::msgs::Ocean>>();
        msg->data = map_.proto();
        ocean_pub_->publish(msg);
    }
    return true;
}
} // namespace interaction
} // namespace scrimmage
