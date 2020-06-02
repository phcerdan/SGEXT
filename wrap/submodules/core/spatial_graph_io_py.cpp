#include "pybind11_common.h"

#include "spatial_graph_io.hpp"

namespace py = pybind11;
using namespace SG;

void init_spatial_graph_io(py::module &m) {
    auto mio = m.def_submodule("io");
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
}
