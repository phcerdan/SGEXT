#include "sgcore_common.hpp"
#include "spatial_graph_io.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace SG;

void init_spatial_graph_io(py::module &m) {
    m.def("read_graphviz_sg", [](const std::string &input_file) {
        return SG::read_graphviz_sg(input_file);
    });
    m.def("read_serialized_sg", [](const std::string &input_file) {
        return SG::read_serialized_sg(input_file);
    });
    m.def("write_graphviz_sg",
          [](const std::string &input_file, GraphType &graph) {
              return SG::write_graphviz_sg(input_file, graph);
          });
    m.def("write_serialized_sg",
          [](const std::string &input_file, GraphType &graph) {
              return SG::write_serialized_sg(input_file, graph);
          });
}
