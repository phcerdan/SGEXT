/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SG_WRITE_VTU_FILE_HPP
#define SG_WRITE_VTU_FILE_HPP

#include "system.hpp"

namespace SG {
void write_vtu_file(const System & sys, const std::string &file_name);
} // namespace SG
#endif
