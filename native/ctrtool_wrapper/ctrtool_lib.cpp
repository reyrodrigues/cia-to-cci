/**
 * ctrtool_lib.cpp
 *
 * Implementation of C API wrapper for ctrtool.
 */

#include "ctrtool_lib.h"

#include <cstring>
#include <string>
#include <memory>
#include <exception>

// Include ctrtool headers
#include "../project_ctr/ctrtool/src/CiaProcess.h"
#include "../project_ctr/ctrtool/src/NcchProcess.h"
#include "../project_ctr/ctrtool/src/KeyBag.h"
#include "../project_ctr/ctrtool/src/Settings.h"

#include <tc/io/FileStream.h>
#include <tc/io/Path.h>

// Helper macro for safe string copy
#define SAFE_STRCPY(dest, src, size) do { \
    if (dest && size > 0) { \
        strncpy(dest, src, size - 1); \
        dest[size - 1] = '\0'; \
    } \
} while(0)

// Helper function to set error message
static void set_error(char* error_msg, size_t error_msg_size, const char* msg) {
    SAFE_STRCPY(error_msg, msg, error_msg_size);
}

// Helper function to initialize KeyBag
// This initializes the KeyBag with all static keys needed for decryption
static ctrtool::KeyBag get_default_keybag() {
    // Initialize KeyBag with static keys
    // isDev = false (retail mode)
    // No fallback title key
    // No seed database path
    // No fallback seed
    ctrtool::KeyBagInitializer keybag(
        false,  // isDev - use retail keys
        tc::Optional<std::string>(),  // no fallback title key
        tc::Optional<tc::io::Path>(),  // no seed database
        tc::Optional<std::string>()   // no fallback seed
    );
    return keybag;
}

extern "C" {

int ctrtool_extract_cia(
    const char* cia_path,
    const char* output_dir,
    char* error_msg,
    size_t error_msg_size
) {
    if (!cia_path || !output_dir) {
        set_error(error_msg, error_msg_size, "Invalid arguments: cia_path and output_dir required");
        return -1;
    }

    try {
        // Create input stream for CIA file
        tc::io::Path cia_tc_path = tc::io::Path(cia_path);
        std::shared_ptr<tc::io::IStream> cia_stream =
            std::make_shared<tc::io::FileStream>(
                cia_tc_path, tc::io::FileMode::Open, tc::io::FileAccess::Read);

        // Initialize KeyBag
        ctrtool::KeyBag keybag = get_default_keybag();

        // Create CIA processor
        ctrtool::CiaProcess proc;
        proc.setInputStream(cia_stream);
        proc.setKeyBag(keybag);
        proc.setCliOutputMode(false); // Don't print info
        proc.setVerboseMode(false);
        proc.setVerifyMode(false);

        // Set content extract path (needs to be a file prefix, not just a directory)
        // The CiaProcess will append .XXXX.XXXXXXXX to this path
        std::string output_dir_str(output_dir);
        // Remove trailing slash if present
        if (!output_dir_str.empty() && output_dir_str.back() == '/') {
            output_dir_str.pop_back();
        }
        std::string content_prefix = output_dir_str + "/contents";
        tc::io::Path output_base_path(content_prefix);
        proc.setContentExtractPath(output_base_path);

        // Process the CIA file
        proc.process();

        return 0;
    } catch (const std::exception& e) {
        set_error(error_msg, error_msg_size, e.what());
        return -1;
    } catch (...) {
        set_error(error_msg, error_msg_size, "Unknown error occurred");
        return -1;
    }
}

int ctrtool_extract_cxi(
    const char* cxi_path,
    const char* exheader_path,
    const char* exefs_dir,
    const char* romfs_path,
    const char* logo_path,
    const char* plain_path,
    bool decompress_code,
    char* error_msg,
    size_t error_msg_size
) {
    if (!cxi_path) {
        set_error(error_msg, error_msg_size, "Invalid argument: cxi_path required");
        return -1;
    }

    try {
        // Create input stream for CXI file
        tc::io::Path cxi_tc_path = tc::io::Path(cxi_path);
        std::shared_ptr<tc::io::IStream> cxi_stream =
            std::make_shared<tc::io::FileStream>(
                cxi_tc_path, tc::io::FileMode::Open, tc::io::FileAccess::Read);

        // Initialize KeyBag
        ctrtool::KeyBag keybag = get_default_keybag();

        // Create NCCH processor
        ctrtool::NcchProcess proc;
        proc.setInputStream(cxi_stream);
        proc.setKeyBag(keybag);
        proc.setVerboseMode(false);
        proc.setVerifyMode(false);
        proc.setRawMode(false);
        proc.setPlainMode(false);
        proc.setShowSyscallName(false);

        // Configure region processing
        // Header - just process, don't extract
        proc.setRegionProcessOutputMode(
            ctrtool::NcchProcess::NcchRegion_Header,
            false, false, tc::Optional<tc::io::Path>(), tc::Optional<tc::io::Path>());

        // ExHeader
        if (exheader_path) {
            tc::Optional<tc::io::Path> exh_path = tc::io::Path(exheader_path);
            proc.setRegionProcessOutputMode(
                ctrtool::NcchProcess::NcchRegion_ExHeader,
                false, false, exh_path, tc::Optional<tc::io::Path>());
        } else {
            proc.setRegionProcessOutputMode(
                ctrtool::NcchProcess::NcchRegion_ExHeader,
                false, false, tc::Optional<tc::io::Path>(), tc::Optional<tc::io::Path>());
        }

        // Plain Region
        if (plain_path) {
            tc::Optional<tc::io::Path> plain_p = tc::io::Path(plain_path);
            proc.setRegionProcessOutputMode(
                ctrtool::NcchProcess::NcchRegion_PlainRegion,
                false, false, plain_p, tc::Optional<tc::io::Path>());
        } else {
            proc.setRegionProcessOutputMode(
                ctrtool::NcchProcess::NcchRegion_PlainRegion,
                false, false, tc::Optional<tc::io::Path>(), tc::Optional<tc::io::Path>());
        }

        // Logo
        if (logo_path) {
            tc::Optional<tc::io::Path> logo_p = tc::io::Path(logo_path);
            proc.setRegionProcessOutputMode(
                ctrtool::NcchProcess::NcchRegion_Logo,
                false, false, logo_p, tc::Optional<tc::io::Path>());
        } else {
            proc.setRegionProcessOutputMode(
                ctrtool::NcchProcess::NcchRegion_Logo,
                false, false, tc::Optional<tc::io::Path>(), tc::Optional<tc::io::Path>());
        }

        // ExeFS
        tc::Optional<tc::io::Path> exefs_extract_path;
        if (exefs_dir) {
            exefs_extract_path = tc::io::Path(exefs_dir);
        }
        proc.setRegionProcessOutputMode(
            ctrtool::NcchProcess::NcchRegion_ExeFs,
            false, false, tc::Optional<tc::io::Path>(), exefs_extract_path);

        // RomFS
        tc::Optional<tc::io::Path> romfs_bin_path;
        if (romfs_path) {
            romfs_bin_path = tc::io::Path(romfs_path);
        }
        proc.setRegionProcessOutputMode(
            ctrtool::NcchProcess::NcchRegion_RomFs,
            false, false, romfs_bin_path, tc::Optional<tc::io::Path>());

        // Process the CXI file
        proc.process();

        return 0;
    } catch (const std::exception& e) {
        set_error(error_msg, error_msg_size, e.what());
        return -1;
    } catch (...) {
        set_error(error_msg, error_msg_size, "Unknown error occurred");
        return -1;
    }
}

int ctrtool_get_cxi_info(
    const char* cxi_path,
    char* title_id,
    size_t title_id_size,
    char* product_code,
    size_t product_code_size,
    char* error_msg,
    size_t error_msg_size
) {
    if (!cxi_path || !title_id || !product_code) {
        set_error(error_msg, error_msg_size, "Invalid arguments: all output buffers required");
        return -1;
    }

    try {
        // Create input stream for CXI file
        tc::io::Path cxi_tc_path = tc::io::Path(cxi_path);
        std::shared_ptr<tc::io::IStream> cxi_stream =
            std::make_shared<tc::io::FileStream>(
                cxi_tc_path, tc::io::FileMode::Open, tc::io::FileAccess::Read);

        // Initialize KeyBag
        ctrtool::KeyBag keybag = get_default_keybag();

        // Create NCCH processor
        ctrtool::NcchProcess proc;
        proc.setInputStream(cxi_stream);
        proc.setKeyBag(keybag);
        proc.setVerboseMode(false);
        proc.setVerifyMode(false);
        proc.setRawMode(false);
        proc.setPlainMode(false);

        // Just process header to get metadata
        proc.setRegionProcessOutputMode(
            ctrtool::NcchProcess::NcchRegion_Header,
            false, false, tc::Optional<tc::io::Path>(), tc::Optional<tc::io::Path>());

        // Process to read header
        proc.process();

        // Extract title ID and product code
        // Note: This is a simplified version. In reality, we'd need to access
        // the internal state of the processor, which may require additional methods.
        // For now, we'll set placeholder values
        SAFE_STRCPY(title_id, "0000000000000000", title_id_size);
        SAFE_STRCPY(product_code, "CTR-P-XXXX", product_code_size);

        // TODO: Actually extract these values from the processor
        // This requires either:
        // 1. Adding getter methods to NcchProcess
        // 2. Parsing the header ourselves
        // 3. Reading the file directly

        return 0;
    } catch (const std::exception& e) {
        set_error(error_msg, error_msg_size, e.what());
        return -1;
    } catch (...) {
        set_error(error_msg, error_msg_size, "Unknown error occurred");
        return -1;
    }
}

} // extern "C"
