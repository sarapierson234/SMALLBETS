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
#include <Smallbets-plugins/plugins/interaction/OceanParameters/OceanMap.h>
#include <scrimmage/proto/ProtoConversions.h>

#include <boost/algorithm/clamp.hpp>

namespace sc = scrimmage;
namespace sp = scrimmage_proto;

using std::cout;
using std::endl;

using boost::algorithm::clamp;

namespace scrimmage {
namespace interaction {

OceanMap::OceanMap() {}

OceanMap::OceanMap(const Technique &technique,const double &x_length,
             const double &y_length, const double &z_length,
             const double &x_resolution, const double &y_resolution,
             const double &z_resolution) :
        technique_(technique),
        x_length_(x_length),
        y_length_(y_length),
        z_length_(z_length),
        x_resolution_(x_resolution),
        y_resolution_(y_resolution),
        z_resolution_(z_resolution),
        grid_(std::vector<double>(x_length_*y_length_*z_length_)) {
    generate();
}


bool OceanMap::generate() {
    if (technique_ == Technique::MUNK) {
        return generate_Munk();
    } return false;
}

bool OceanMap::generate_Munk() {
    // Force the altitude center to be at the midpoint between z_max and z_min
    double C_z_ = 1500; //Sound Speed
    double z = 0;
    // MUNK Profile C(z) = 1500 * (1 + E * (zt - 1 + E^-zt)); zt = (2 * (z - zc))/zc;
    double ep = 0.00737;
    double zc = 1300; //m depth of minimum sound speed
    for (unsigned int row = 0; row < y_length_; ++row) {
        for (unsigned int col = 0; col < x_length_; ++col) {
            for (unsigned int depth = 0; depth < z_length_; ++depth) {
                z = depth * z_resolution_;
                C_z_ = 1500 * (1 + ep * ((2 * (z - zc))/zc - 1 + exp((-(2 * (z - zc))/zc))));
                grid_[(row*x_length_*z_length_)+(col*z_length_)+depth] = C_z_;
                //grid_[(row*x_length_*z_length_)+(col*z_length_)+depth].is_set = true;
            }
        }
    }
    return true;
}

double OceanMap::get_neighbor_avg(const int &row, const int &col, const int &depth) {
    unsigned int neighbors = 0;
    double speed_sum = 0;

    if (row > 0) {
        speed_sum += grid_[depth+row+col];
        ++neighbors;
    }
    if (col > 0) {
        speed_sum += grid_[depth+row+col];
        ++neighbors;
    }
    if (row > 0 && col > 0) {
        speed_sum += grid_[depth+row+col];
        ++neighbors;
    }

    return (neighbors == 0) ? 0 : speed_sum / neighbors;
}

Smallbets_plugins::msgs::Ocean OceanMap::proto() {
    Smallbets_plugins::msgs::Ocean Ocean;
    Ocean.set_x_length(x_length_);
    Ocean.set_y_length(y_length_);
    Ocean.set_z_length(z_length_);
    Ocean.set_x_resolution(x_resolution_);
    Ocean.set_y_resolution(y_resolution_);
    Ocean.set_z_resolution(z_resolution_);
    std::cout << "You Are here" << std::endl;
    for (unsigned int r = 0; r < y_length_; ++r) {
        for (unsigned int c = 0; c < x_length_; ++c) {
            for (unsigned int d = 0; d < z_length_; ++d) {
                int idx = (r*x_length_*z_length_)+(c*z_length_)+d;
                Ocean.mutable_map()->add_row(grid_[idx]);
                std::cout << "(Depth,Speed) : (" << (d * z_resolution_) << ", " << grid_[idx] << ")" << std::endl;
            }
        }
    }
    return Ocean;
}

//Method to get Sound speed (depth)
/*boost::optional<double> OceanMap::height_at(const double &x, const double &y) {
    int row = std::floor((y + y_length_/2.0 - center_(1)) / y_resolution_);
    int col = std::floor((x + x_length_/2.0 - center_(0)) / x_resolution_);
    if (row < 0 || row >= static_cast<int>(num_y_rows_) ||
        col < 0 || col >= static_cast<int>(num_x_cols_) ||
        not grid_[row][col].is_set) {
        return boost::optional<double>{};
    }
    return grid_[row][col].height;
}*/

} // namespace interaction
} // namespace scrimmage
