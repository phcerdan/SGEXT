/* Copyright (C) 2019 Pablo Hernandez-Cerdan
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "graph_locator_fixtures.hpp"
#include "vtkDataSet.h"

namespace testing {
void print_vtk_points(vtkAbstractPointLocator * locator)
{
    auto npoints = locator->GetDataSet()->GetNumberOfPoints();
    for(vtkIdType index = 0; index < npoints; ++index){
        auto point = locator->GetDataSet()->GetPoint(index);
        std::cout << index << ": " << point[0] << ", " << point[1] << " , " << point[2] << std::endl;
    }
};
}

void MatchingGraphsFixture::CreateG0()
{
    using boost::add_edge;
    using boost::add_vertex;

    /*   g0)
     *       |
     *       |
     *      / \
     *     /   \
     *    /     \
     */
    this->g0 = GraphType(4);
    g0[0].pos = p0;
    g0[1].pos = u2;
    g0[2].pos = dr3;
    g0[3].pos = dl3;

    /*   |
     *   |
     */
    SG::SpatialEdge se_p0u2;
    se_p0u2.edge_points.insert(std::end(se_p0u2.edge_points), {u1});
    add_edge(0, 1, se_p0u2, g0);

    /*   \
     *    \
     *     \
     */
    SG::SpatialEdge se_p0dr3;
    se_p0dr3.edge_points.insert(std::end(se_p0dr3.edge_points), {dr1, dr2});
    add_edge(0, 2, se_p0dr3, g0);

    /*    /
     *   /
     *  /
     */
    SG::SpatialEdge se_p0dl3;
    se_p0dl3.edge_points.insert(std::end(se_p0dl3.edge_points), {dl1, dl2});
    add_edge(0, 3, se_p0dl3, g0);
};

void MatchingGraphsFixture::CreateG1()
{
    using boost::add_edge;
    using boost::add_vertex;
    /*   g1)
     *       |
     *       |
     *      / \
     *     /___\
     *    /     \__
     */
    // g1 has 2 extra nodes, and one vertex moved (dr3_1)

    this->g1 = GraphType(6);

    g1[0].pos = p0;
    g1[1].pos = u2;
    g1[2].pos = dr3_1;
    g1[3].pos = dl3;

    g1[4].pos = dr2;
    g1[5].pos = dl2;

    /*   |
     *   |
     */
    SG::SpatialEdge se_p0u2;
    se_p0u2.edge_points.insert(std::end(se_p0u2.edge_points), {u1});
    add_edge(0, 1, se_p0u2, g1);

    /*  \
     *   \
     */
    SG::SpatialEdge se_p0dr2;
    se_p0dr2.edge_points.insert(std::end(se_p0dr2.edge_points), {dr1});
    add_edge(0, 4, se_p0dr2, g1);

    /*  \__
    */
    SG::SpatialEdge se_dr2dr3_1;
    se_dr2dr3_1.edge_points.insert(std::end(se_dr2dr3_1.edge_points), {dr3});
    add_edge(4, 2, se_dr2dr3_1, g1);

    /*    /
     *   /
     */
    SG::SpatialEdge se_p0dl2;
    se_p0dl2.edge_points.insert(std::end(se_p0dl2.edge_points), {dl1});
    add_edge(0, 5, se_p0dl2, g1);

    /*    /
    */
    SG::SpatialEdge se_dl2_dl3;
    add_edge(5, 3, se_dl2_dl3, g1);

    /*  ___
    */
    SG::SpatialEdge se_dl2_dr2;
    se_dl2_dr2.edge_points.insert(std::end(se_dl2_dr2.edge_points),
            {dl2_dr2_1, dl2_dr2_2, dl2_dr2_3});
    add_edge(5, 4, se_dl2_dr2, g1);
};

void MatchingGraphsFixture::CreateGR()
{
    using boost::add_edge;
    using boost::add_vertex;
    /*   gR)
     *       |
     *       |
     *      / \
     *     /   \
     *    /     \__
     */

    this->gR = GraphType(4);

    gR[0].pos = p0;
    gR[1].pos = u2;
    gR[2].pos = dr3_1;
    gR[3].pos = dl3;

    /*   |
     *   |
     */
    SG::SpatialEdge se_p0u2;
    se_p0u2.edge_points.insert(std::end(se_p0u2.edge_points), {u1});
    add_edge(0, 1, se_p0u2, gR);

    /*  \
     *   \
     *    \__
     */
    SG::SpatialEdge se_p0dr3_1;
    se_p0dr3_1.edge_points.insert(std::end(se_p0dr3_1.edge_points),
            {dr1, dr2, dr3});
    add_edge(0, 2, se_p0dr3_1, gR);

    /*    /
     *   /
     *  /
     */
    SG::SpatialEdge se_p0dl3;
    se_p0dl3.edge_points.insert(std::end(se_p0dl3.edge_points), {dl1, dl2});
    add_edge(0, 3, se_p0dl3, gR);
};

void MatchingGraphsFixture::SetUp()
{
    this->CreateG0();
    this->CreateG1();
    this->CreateGR();
};

