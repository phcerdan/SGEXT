#include "sgcore_common_py.hpp"
#include "spatial_graph_io.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace SG;

void init_spatial_graph_io(py::module &m) {
    auto mio = m.def_submodule("io");
    mio.def("read_graphviz_sg", [](const std::string &input_file) {
        return SG::read_graphviz_sg(input_file);
    });
    mio.def("read_serialized_sg", [](const std::string &input_file) {
        return SG::read_serialized_sg(input_file);
    });
    mio.def("write_graphviz_sg",
            [](const std::string &input_file, GraphType &graph) {
                return SG::write_graphviz_sg(input_file, graph);
            });
    mio.def("write_serialized_sg",
            [](const std::string &input_file, GraphType &graph) {
                return SG::write_serialized_sg(input_file, graph);
            });
}
