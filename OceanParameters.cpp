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

#include <my-scrimmage-plugins/plugins/interaction/OceanParameters/OceanParameters.h>

#include <scrimmage/common/Utilities.h>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/math/State.h>

#include <memory>
#include <limits>
#include <iostream>

using std::cout;
using std::endl;

namespace sc = scrimmage;

REGISTER_PLUGIN(scrimmage::EntityInteraction,
                scrimmage::interaction::OceanParameters,
                OceanParameters_plugin)

namespace scrimmage {
namespace interaction {

OceanParameters::OceanParameters() {
}

bool OceanParameters::init(std::map<std::string, std::string> &mission_params,
                               std::map<std::string, std::string> &plugin_params) {
    //I assume we need these here to make the data cube a legit cube
    double x_length = sc::get<double>("x_length", plugin_params, 500.0);
    double y_length = sc::get<double>("y_length", plugin_params, 500.0);
    double z_length = sc::get<double>("z_length", plugin_params, 500.0);
    double x_resolution = sc::get<double>("x_resolution", plugin_params, 1.0);
    double y_resolution = sc::get<double>("y_resolution", plugin_params, 1.0);
    double z_resolution = sc::get<double>("z_resolution", plugin_params, 1.0);
    double z_min = sc::get<double>("z_min", plugin_params, -std::numeric_limits<double>::infinity());
    double z_max = sc::get<double>("z_max", plugin_params, +std::numeric_limits<double>::infinity());
    double z_std = sc::get<double>("z_std", plugin_params, 1.0); 
    return true;
}
  auto callback =[&] (scrimmage::MessagePtr<<sci::Terrain> msg) {
        cout << "Data Map" << map_.proto->coords() << endl;
        cout << "Received a boundary info message!" << endl;
subscribe<sci::Terrain>("GlobalNetwork", "Terrain", callback)
//the documentation said to place this below the init portion of the code,
//if you want to subscribe to something. And then the subscription is below
//it obviously. When I run the code I dont get any messages getting written out though
  //I dont see the "Data Map" string showing up. I am also not quite sure what the 
  //format of the data is that I am subscribing to, so I am a little confused how to 
  //1. even see/visualize/conceptualize the Terrain Data and
  //2. how to manipulate it

//now that we have subscribed above, we need to manipulate the bathymetry data
//and create a datacube 

//data manipulation here
//after we are done with that we move on to the following

//I put the following publishing line below because in the TG.cpp a similar
//line was also above the step-entity-bool line
    oceanparameters_pub_ = advertise("GlobalNetwork", "Ocean Parameters");

    return true;
    
  
bool OceanParameters::step_entity_interaction(std::list<sc::EntityPtr> &ents,
                                                  double t, double dt) {
    if (ents.empty()) {
        return true;
    }

    return true;
}
} // namespace interaction
} // namespace scrimmage
