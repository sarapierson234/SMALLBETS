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
MODIFIED BY:
 * @author Sara Pierson <spierson@gatech.edu>
 * @date Novermber 2020
 * @section DESCRIPTION
 * This plugin will get information from the terrain generator to create a sudo realistic/munk ocean profile
%This environmental information will be published to scrimmage memory (a global variable?) and then a sensor model will subscribe to the 
%published information. The sensor will get the info, then write it out in a txt/csv file so that it can be given to BELLHOP for reduction/
%sound analysis  *
 */

#include <my-scrimmage-plugins/plugins/interaction/OceanParameters/OceanParameters.h>

#include <scrimmage/common/Utilities.h>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/plugin_manager/RegisterPlugin.h>
#include <scrimmage/math/State.h>
//from looking at other code as an example I included the following, delete the ones that may be unecessary
//On second thought/look I think that "pubsub" is ROS specific (the example I am looking at is ROSCompass.cpp)
//but I do need to figure out how to make a publisher/subcriber in here so that I can publish the environmental info 
// to the general scrimmage memory and then subscribe to it from the sensor plugin
#include <scrimmag/parse/ParseUtils.h>
#include <scrimmage/pubsub/Message.h>
#include <scrimmage/pubsub/Publisher.h>
#include <scrimmage/pubsub/Subscriber.h>


#include <memory>
#include <limits>
#include <iostream>

using std::cout;
using std::endl;

namespace sc = scrimmage;

//
REGISTER_PLUGIN(scrimmage::EntityInteraction,
                scrimmage::interaction::OceanParameters,
                OceanParameters_plugin)

namespace scrimmage {
namespace interaction {

OceanParameters::OceanParameters() {
}

//the docs just say that init is implemented like in all other files
//but what does the init actually do? Just initialize variables?
//(is it right to call them variables or do the initialized pieces here
// go by another name?)
//what does std::map<std::string mean?
bool OceanParameters::init(std::map<std::string, std::string> &mission_params,
                               std::map<std::string, std::string> &plugin_params) {
    return true;
}

//this takes a list of scrimmage entities as inputs, so I  
//assume that I put the GroundTerrain generaator here
bool OceanParameters::step_entity_interaction(std::list<sc::EntityPtr> &ents,
                                                  double t, double dt) {
    if (ents.empty()) {
        return true;
    }

    return true;
}
} // namespace interaction
} // namespace scrimmage
