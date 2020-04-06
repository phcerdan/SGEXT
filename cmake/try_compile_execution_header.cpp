/* ********************************************************************
 * Copyright (C) 2020 Pablo Hernandez-Cerdan.
 *
 * This file is part of SGEXT: http://github.com/phcerdan/sgext.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * *******************************************************************/

/* Use this source file to find out if the compiler has access to 
 * the stl parallel algorithms.
 * At the time of writing, gcc and others rely on intel-tbb (TBB), and need to link
 * with it.
 *
 * The try_compile of this file from CMake could file because no TBB_LIBRARIES
 * are found, and/or because this c++17 feature is not implemented for your
 * compiler version. GCC implements it since 9.1
 */

#include <execution>
#include <vector>
#include <algorithm>

int main()
{
    const std::vector<int> X(10);
    auto F = X;
    size_t counter = 0;
    auto func = [&counter](const int &a) {
        counter++;
        return a + counter;
    };

    auto policy = std::execution::par_unseq;
    std::transform(policy, std::begin(X), std::end(X), std::begin(F), func);
}
