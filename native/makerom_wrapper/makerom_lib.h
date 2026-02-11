/**
 * makerom_lib.h
 *
 * C API wrapper for makerom functionality.
 * Provides simplified interface for building CXI/CCI files.
 */

#ifndef MAKEROM_LIB_H
#define MAKEROM_LIB_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Build a CXI file from components.
 *
 * @param output_path Output path for CXI file
 * @param rsf_path Path to RSF configuration file
 * @param exheader_path Path to extended header
 * @param code_path Path to code.bin
 * @param icon_path Path to icon.bin
 * @param banner_path Path to banner.bin
 * @param romfs_path Path to RomFS binary (can be NULL)
 * @param logo_path Path to logo (can be NULL)
 * @param plain_path Path to plain region (can be NULL)
 * @param error_msg Buffer for error message (if any)
 * @param error_msg_size Size of error_msg buffer
 * @return 0 on success, -1 on error
 */
int makerom_build_cxi(
    const char* output_path,
    const char* rsf_path,
    const char* exheader_path,
    const char* code_path,
    const char* icon_path,
    const char* banner_path,
    const char* romfs_path,
    const char* logo_path,
    const char* plain_path,
    char* error_msg,
    size_t error_msg_size
);

/**
 * Build a CCI file from a CXI.
 *
 * @param output_path Output path for CCI file
 * @param content_path Path to CXI content
 * @param content_index Content index (usually 0)
 * @param content_id Content ID (usually 0)
 * @param error_msg Buffer for error message (if any)
 * @param error_msg_size Size of error_msg buffer
 * @return 0 on success, -1 on error
 */
int makerom_build_cci(
    const char* output_path,
    const char* content_path,
    int content_index,
    int content_id,
    char* error_msg,
    size_t error_msg_size
);

#ifdef __cplusplus
}
#endif

#endif // MAKEROM_LIB_H
