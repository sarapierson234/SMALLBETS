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

#ifndef INCLUDE_MY_SCRIMMAGE_PLUGINS_PLUGINS_INTERACTION_OCEANPARAMETERS_OCEANP$
#define INCLUDE_MY_SCRIMMAGE_PLUGINS_PLUGINS_INTERACTION_OCEANPARAMETERS_OCEANP$

#include <scrimmage/simcontrol/EntityInteraction.h>
#include <scrimmage/entity/Entity.h>

#include <map>
#include <list>
#include <string>

namespace scrimmage {
namespace interaction {

class OceanParameters : public scrimmage::EntityInteraction {
 public:
    OceanParameters();
    bool init(std::map<std::string, std::string> &mission_params,
              std::map<std::string, std::string> &plugin_params) override;
    bool step_entity_interaction(std::list<scrimmage::EntityPtr> &ents,
                                 double t, double dt) override;
};

protected:
// below are the publishing lines in the TG.h
//TerrainMap map_;
//bool terrain_published_ = false;
//scrimmage::PublisherPtr terrain_pub_;
//RandomPtr random_;
//now here is what I think needs to be included in our code--is it right?
        OceanParameters map_;
        bool oceanparameters_published_ = false;
        scrimmage::PublisherPtr oceanparameters_pub_;
        RandomPtr random_;
 private:
};
} // namespace interaction
} // namespace scrimmage
#endif // INCLUDE_MY_SCRIMMAGE_PLUGINS_PLUGINS_INTERACTION_OCEANPARAMETERS_OCEA$


