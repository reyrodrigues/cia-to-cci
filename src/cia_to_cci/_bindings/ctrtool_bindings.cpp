/**
 * ctrtool_bindings.cpp
 *
 * pybind11 bindings for ctrtool C API.
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../../../native/ctrtool_wrapper/ctrtool_lib.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace py = pybind11;

// Custom exception for ctrtool errors
class CtrToolError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// Wrapper class for CIA extraction
class CIAExtractor {
public:
    void extract_contents(const std::string& cia_path, const std::string& output_dir) {
        char error_msg[1024];
        int result = ctrtool_extract_cia(
            cia_path.c_str(),
            output_dir.c_str(),
            error_msg,
            sizeof(error_msg)
        );
        if (result != 0) {
            throw CtrToolError(error_msg);
        }
    }
};

// Wrapper class for CXI extraction
class CXIExtractor {
public:
    struct CXIInfo {
        std::string title_id;
        std::string product_code;
    };

    void extract_components(
        const std::string& cxi_path,
        const std::string& exheader_path,
        const std::string& exefs_dir,
        const std::string& romfs_path,
        const std::string& logo_path = "",
        const std::string& plain_path = "",
        bool decompress_code = true
    ) {
        char error_msg[1024];
        int result = ctrtool_extract_cxi(
            cxi_path.c_str(),
            exheader_path.empty() ? nullptr : exheader_path.c_str(),
            exefs_dir.empty() ? nullptr : exefs_dir.c_str(),
            romfs_path.empty() ? nullptr : romfs_path.c_str(),
            logo_path.empty() ? nullptr : logo_path.c_str(),
            plain_path.empty() ? nullptr : plain_path.c_str(),
            decompress_code,
            error_msg,
            sizeof(error_msg)
        );
        if (result != 0) {
            throw CtrToolError(error_msg);
        }
    }

    CXIInfo get_info(const std::string& cxi_path) {
        char title_id[64];
        char product_code[64];
        char error_msg[1024];

        int result = ctrtool_get_cxi_info(
            cxi_path.c_str(),
            title_id,
            sizeof(title_id),
            product_code,
            sizeof(product_code),
            error_msg,
            sizeof(error_msg)
        );

        if (result != 0) {
            throw CtrToolError(error_msg);
        }

        return {title_id, product_code};
    }
};

// pybind11 module definition
PYBIND11_MODULE(_ctrtool, m) {
    m.doc() = "Python bindings for ctrtool - CIA/CXI extraction and processing";

    // Register custom exception
    py::register_exception<CtrToolError>(m, "CtrToolError");

    // CIAExtractor class
    py::class_<CIAExtractor>(m, "CIAExtractor")
        .def(py::init<>())
        .def("extract_contents", &CIAExtractor::extract_contents,
             py::arg("cia_path"),
             py::arg("output_dir"),
             "Extract contents from CIA file to directory");

    // CXIExtractor class
    py::class_<CXIExtractor>(m, "CXIExtractor")
        .def(py::init<>())
        .def("extract_components", &CXIExtractor::extract_components,
             py::arg("cxi_path"),
             py::arg("exheader_path") = "",
             py::arg("exefs_dir") = "",
             py::arg("romfs_path") = "",
             py::arg("logo_path") = "",
             py::arg("plain_path") = "",
             py::arg("decompress_code") = true,
             "Extract CXI components (ExeFS, RomFS, etc.)")
        .def("get_info", &CXIExtractor::get_info,
             py::arg("cxi_path"),
             "Get CXI metadata (Title ID, Product Code)");

    // CXIInfo struct
    py::class_<CXIExtractor::CXIInfo>(m, "CXIInfo")
        .def_readonly("title_id", &CXIExtractor::CXIInfo::title_id)
        .def_readonly("product_code", &CXIExtractor::CXIInfo::product_code);
}
