/* Copyright (C) 2020 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
