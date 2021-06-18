#include "pybind11_common.h"

#include "spatial_graph_io.hpp"

namespace py = pybind11;
using namespace SG;

void init_spatial_graph_io(py::module &m) {
    auto mio = m.def_submodule("io");
    mio.doc() =
R"(Submodule dealing with reading and writing graphs and related data structures.)";

    mio.def("read_graphviz_sg", [](const std::string &input_file) {
        return SG::read_graphviz_sg(input_file);
    }, py::arg("filename"));

    mio.def("read_serialized_sg", [](const std::string &input_file) {
        return SG::read_serialized_sg(input_file);
    }, py::arg("filename"));

    mio.def("write_graphviz_sg",
            [](const std::string &output_file, GraphType &graph) {
                return SG::write_graphviz_sg(output_file, graph);
    }, py::arg("filename"), py::arg("graph"));

    mio.def("write_serialized_sg",
            [](const std::string &output_file, GraphType &graph) {
                return SG::write_serialized_sg(output_file, graph);
    }, py::arg("filename"), py::arg("graph"));

    /*******************************************/

    const std::string read_write_vertex_to_label_map_common_docs =
            R"(
Read/Write a CSV-like file containing a one-line header and two comma-separated values
with vertex_id and label.
Example:
# vertex_id,  label
4, 0
5, 1
12, 2

This map can be used in @ref voxelize_graph.
)";
    const std::string read_vertex_to_label_map_parameters_docs =
            R"(
Parameters:
----------
input_file: String/Path
 input filename where the map is stored.
)";
    mio.def("read_vertex_to_label_map", &read_vertex_to_label_map,
          (read_write_vertex_to_label_map_common_docs +
           read_vertex_to_label_map_parameters_docs)
                  .c_str(),
          py::arg("input_file"));

    const std::string write_vertex_to_label_map_parameters_docs =
            R"(
Parameters:
----------
vertex_to_label_map: Dict {int->int}
 Map/Dict containing vertex to labels.

output_file: String/Path
 Filename to write down the vertex_to_label_map
)";
    mio.def("write_vertex_to_label_map", &write_vertex_to_label_map,
          (read_write_vertex_to_label_map_common_docs +
           write_vertex_to_label_map_parameters_docs)
                  .c_str(),
          py::arg("vertex_to_label_map"), py::arg("output_file"));

    /*******************************************/

    const std::string read_write_edge_to_label_map_common_docs =
            R"(

Read/Write a CSV-like file containing a one-line header and two
comma-separated values with edge_id and label.

The column holding the edge has a '-' separating source and target.

Example:
# edge_source-target, label
4-12, 0
23-43, 1
12-2, 2
)";
    const std::string read_edge_to_label_map_parameters_docs =
            R"(
Parameters:
----------
input_file: String/Path
 input filename where the map is stored.
)";
    mio.def("read_edge_to_label_map", &read_edge_to_label_map,
          (read_write_edge_to_label_map_common_docs +
           read_edge_to_label_map_parameters_docs)
                  .c_str(),
          py::arg("input_file"));

    const std::string write_edge_to_label_map_parameters_docs =
            R"(
Parameters:
----------
edge_to_label_map: Dict {GraphType::edge_descriptor->int}
 Map/Dict containing edge to labels.

output_file: String/Path
 Filename to write down the edge_to_label_map
)";
    mio.def("write_edge_to_label_map", &write_edge_to_label_map,
          (read_write_edge_to_label_map_common_docs +
           write_edge_to_label_map_parameters_docs)
                  .c_str(),
          py::arg("edge_to_label_map"), py::arg("output_file"));
}
