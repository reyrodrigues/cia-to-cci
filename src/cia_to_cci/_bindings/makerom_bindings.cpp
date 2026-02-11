/**
 * makerom_bindings.cpp
 *
 * pybind11 bindings for makerom C API.
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../../../native/makerom_wrapper/makerom_lib.h"

#include <stdexcept>
#include <string>
#include <cstdio>

namespace py = pybind11;

// Custom exception for makerom errors
class MakeRomError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// Wrapper class for CXI building
class CXIBuilder {
public:
    void build_cxi(
        const std::string& output_path,
        const std::string& rsf_path,
        const std::string& exheader_path,
        const std::string& code_path,
        const std::string& icon_path,
        const std::string& banner_path,
        const std::string& romfs_path = "",
        const std::string& logo_path = "",
        const std::string& plain_path = ""
    ) {
        char error_msg[1024];
        int result = makerom_build_cxi(
            output_path.c_str(),
            rsf_path.c_str(),
            exheader_path.c_str(),
            code_path.c_str(),
            icon_path.c_str(),
            banner_path.c_str(),
            romfs_path.empty() ? nullptr : romfs_path.c_str(),
            logo_path.empty() ? nullptr : logo_path.c_str(),
            plain_path.empty() ? nullptr : plain_path.c_str(),
            error_msg,
            sizeof(error_msg)
        );
        if (result != 0) {
            throw MakeRomError(error_msg);
        }
    }
};

// Wrapper class for CCI building
class CCIBuilder {
public:
    void build_cci(
        const std::string& output_path,
        const std::string& content_path,
        int content_index = 0,
        int content_id = 0
    ) {
        char error_msg[1024];
        int result = makerom_build_cci(
            output_path.c_str(),
            content_path.c_str(),
            content_index,
            content_id,
            error_msg,
            sizeof(error_msg)
        );
        if (result != 0) {
            throw MakeRomError(error_msg);
        }
    }
};

// pybind11 module definition
PYBIND11_MODULE(_makerom, m) {
    m.doc() = "Python bindings for makerom - CXI/CCI building";

    // Register custom exception
    py::register_exception<MakeRomError>(m, "MakeRomError");

    // CXIBuilder class
    py::class_<CXIBuilder>(m, "CXIBuilder")
        .def(py::init<>())
        .def("build_cxi", &CXIBuilder::build_cxi,
             py::arg("output_path"),
             py::arg("rsf_path"),
             py::arg("exheader_path"),
             py::arg("code_path"),
             py::arg("icon_path"),
             py::arg("banner_path"),
             py::arg("romfs_path") = "",
             py::arg("logo_path") = "",
             py::arg("plain_path") = "",
             "Build CXI file from components");

    // CCIBuilder class
    py::class_<CCIBuilder>(m, "CCIBuilder")
        .def(py::init<>())
        .def("build_cci", &CCIBuilder::build_cci,
             py::arg("output_path"),
             py::arg("content_path"),
             py::arg("content_index") = 0,
             py::arg("content_id") = 0,
             "Build CCI file from CXI content");
}
