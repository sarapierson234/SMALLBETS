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

#ifndef INCLUDE_SMALLBETS_PLUGINS_PLUGINS_INTERACTION_OCEANPARAMETERS_OCEANMAP_H_
#define INCLUDE_SMALLBETS_PLUGINS_INTERACTION_OCEANPARAMETERS_OCEANMAP_H_

#include <scrimmage/simcontrol/EntityInteraction.h>
#include <scrimmage/entity/Entity.h>
#include <scrimmage/proto/Shape.pb.h>

#include <Smallbets-plugins/msgs/Ocean.pb.h>
#include <vector>
#include <string>
#include <limits>
#include <random>
#include <memory>
#include <boost/optional.hpp>

namespace scrimmage {
namespace interaction {

class OceanMap {
 public:
    enum class Technique { MUNK, LINEAR_WALK };

    OceanMap();
    OceanMap(const Technique &technique,const double &x_length,
             const double &y_length, const double &z_length,
             const double &x_resolution, const double &y_resolution,
             const double &z_resolution);
    Smallbets_plugins::msgs::Ocean proto();
    //boost::optional<double> height_at(const double &x, const double &y);

 protected:
    bool generate();
    bool generate_Munk();

    Technique technique_ = Technique::MUNK;
    double x_length_;
    double y_length_;
    double z_length_;
    double x_resolution_;
    double y_resolution_;
    double z_resolution_;

    std::vector<double> grid_;
    double get_neighbor_avg(const int &row, const int &col, const int &depth);

 private:
};
} // namespace interaction
} // namespace scrimmage
#endif // INCLUDE_SMALLBETS_PLUGINS_PLUGINS_INTERACTION_OCEANPARAMETERS_OCEANMAP_H_
