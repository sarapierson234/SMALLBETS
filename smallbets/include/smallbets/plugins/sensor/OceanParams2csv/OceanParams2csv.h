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

#ifndef INCLUDE_SMALLBETS_PLUGINS_SENSOR_OCEANPARAMS2CSV_OCEANPARAMS2CSV_H_
#define INCLUDE_SMALLBETS_PLUGINS_SENSOR_OCEANPARAMS2CSV_OCEANPARAMS2CSV_H_

#include <scrimmage/sensor/Sensor.h>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/entity/Contact.h>
#include <scrimmage/pubsub/Publisher.h>
#include <scrimmage/plugins/interaction/TerrainGenerator/TerrainMap.h>
#include <smallbets/plugins/interaction/OceanParameters/OceanMap.h>

#include <random>
#include <vector>
#include <map>
#include <string>

namespace scrimmage {
namespace sensor {
class OceanParams2csv : public scrimmage::Sensor {
 public:
    OceanParams2csv();
    void init(std::map<std::string, std::string> &params) override;
    bool step() override;

 protected:
    std::map<std::string, std::string> params_;
    double source_x;
    double source_y;
    double source_z;
    double rcvr_x;
    double rcvr_y;
    double rcvr_z;
    std::vector<double> x_pos;
    std::vector<double> y_pos;
    std::vector<double> depth;
    double range_length = 20; //Number of range bins
    std::shared_ptr<scrimmage::interaction::OceanMap> paramNEW_ = nullptr;
    std::shared_ptr<scrimmage::interaction::TerrainMap> map_ = nullptr;
 private:
};
} // namespace sensor
} // namespace scrimmage
#endif // INCLUDE_SMALLBETS_PLUGINS_SENSOR_OCEANPARAMS2CSV_OCEANPARAMS2CSV_H_
