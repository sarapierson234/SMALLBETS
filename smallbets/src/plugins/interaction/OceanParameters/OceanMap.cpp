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
#include <smallbets/plugins/interaction/OceanParameters/OceanMap.h>
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

OceanMap::OceanMap(const smallbets::msgs::Ocean &Ocean) :
        x_length_(Ocean.x_length()), y_length_(Ocean.y_length()),
        z_length_(Ocean.z_length()), x_resolution_(Ocean.x_resolution()),
        y_resolution_(Ocean.y_resolution()), z_resolution_(Ocean.z_resolution()),
        grid_(std::vector<double>(x_length_*y_length_*z_length_)) {

    // Populate the grid
    for (int r = 0; r < Ocean.map().row_size(); ++r) {
            grid_[r] = Ocean.map().row(r);
    }
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
    double zc = 500; //m depth of minimum sound speed
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

smallbets::msgs::Ocean OceanMap::proto() {
    smallbets::msgs::Ocean Ocean;
    Ocean.set_x_length(x_length_);
    Ocean.set_y_length(y_length_);
    Ocean.set_z_length(z_length_);
    Ocean.set_x_resolution(x_resolution_);
    Ocean.set_y_resolution(y_resolution_);
    Ocean.set_z_resolution(z_resolution_);
    for (unsigned int r = 0; r < y_length_; ++r) {
        for (unsigned int c = 0; c < x_length_; ++c) {
            for (unsigned int d = 0; d < z_length_; ++d) {
                int idx = (r*x_length_*z_length_)+(c*z_length_)+d;
                Ocean.mutable_map()->add_row(grid_[idx]);
                //std::cout << "(Depth,Speed) : (" << (d * z_resolution_) << ", " << grid_[idx] << ")" << std::endl;
            }
        }
    }
    return Ocean;
}

//Method to get parameters at XYZ location
double OceanMap::param_at(const double &x, const double &y,const double &z) {
// get nearest neighbor (Add on later interp around value)
    std::vector<int> idx_dist(x_length_*y_length_*z_length_);
    std::vector<double> dist(x_length_*y_length_*z_length_);
    for (unsigned int r = 0; r < y_length_; ++r) {
        for (unsigned int c = 0; c < x_length_; ++c) {
            for (unsigned int d = 0; d < z_length_; ++d) {
                int idx_cnt = (r*x_length_*z_length_)+(c*z_length_)+d;
                double x2 = c * x_resolution_ - (x_length_ * x_resolution_ * 0.5);
                double y2 = r * y_resolution_ - (y_length_ * y_resolution_ * 0.5);
                double z2 = d * z_resolution_;
                double xSqr = (x - x2) * (x - x2);
                double ySqr = (y - y2) * (y - y2);
                double zSqr = ((-z) - z2) * ((-z) - z2);
                dist[idx_cnt] = sqrt(xSqr + ySqr + zSqr);
                idx_dist[idx_cnt] = idx_cnt;
            }
        }
    }
    double dist_min = (std::min_element(dist.begin(), dist.end())) - dist.begin();
    int idx_min = idx_dist[dist_min];

    if (dist_min >= 0) {
        return grid_[idx_min];
    }
    std::cout << "Error: Sound Speed Data out of Range" << std::endl;
    return 9999;
    //grid_[(row*x_length_*z_length_)+(col*z_length_)+depth] = C_z_;
}
} // namespace interaction
} // namespace scrimmage
