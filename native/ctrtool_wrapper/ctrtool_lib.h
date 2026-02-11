/**
 * ctrtool_lib.h
 *
 * C API wrapper for ctrtool functionality.
 * Provides simplified interface for CIA/CXI extraction and processing.
 */

#ifndef CTRTOOL_LIB_H
#define CTRTOOL_LIB_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Extract contents from a CIA file.
 *
 * @param cia_path Path to input CIA file
 * @param output_dir Directory to extract contents to
 * @param error_msg Buffer for error message (if any)
 * @param error_msg_size Size of error_msg buffer
 * @return 0 on success, -1 on error
 */
int ctrtool_extract_cia(
    const char* cia_path,
    const char* output_dir,
    char* error_msg,
    size_t error_msg_size
);

/**
 * Extract components from a CXI/NCCH file.
 *
 * @param cxi_path Path to input CXI file
 * @param exheader_path Output path for extended header (can be NULL)
 * @param exefs_dir Output directory for ExeFS components (can be NULL)
 * @param romfs_path Output path for RomFS binary (can be NULL)
 * @param logo_path Output path for logo (can be NULL)
 * @param plain_path Output path for plain region (can be NULL)
 * @param decompress_code Whether to decompress code.bin
 * @param error_msg Buffer for error message (if any)
 * @param error_msg_size Size of error_msg buffer
 * @return 0 on success, -1 on error
 */
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
);

/**
 * Get metadata from a CXI/NCCH file.
 *
 * @param cxi_path Path to CXI file
 * @param title_id Buffer for title ID (16 hex chars + null terminator)
 * @param title_id_size Size of title_id buffer (should be at least 17)
 * @param product_code Buffer for product code
 * @param product_code_size Size of product_code buffer (should be at least 17)
 * @param error_msg Buffer for error message (if any)
 * @param error_msg_size Size of error_msg buffer
 * @return 0 on success, -1 on error
 */
int ctrtool_get_cxi_info(
    const char* cxi_path,
    char* title_id,
    size_t title_id_size,
    char* product_code,
    size_t product_code_size,
    char* error_msg,
    size_t error_msg_size
);

#ifdef __cplusplus
}
#endif

#endif // CTRTOOL_LIB_H
