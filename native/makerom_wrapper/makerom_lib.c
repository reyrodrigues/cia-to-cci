/**
 * makerom_lib.c
 *
 * Implementation of C API wrapper for makerom.
 */

#include "makerom_lib.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Include makerom headers
#include "../project_ctr/makerom/src/lib.h"
#include "../project_ctr/makerom/src/user_settings.h"
#include "../project_ctr/makerom/src/ncch_build.h"
#include "../project_ctr/makerom/src/ncsd_build.h"
#include "../project_ctr/makerom/src/cia_build.h"
#include "../project_ctr/makerom/src/rsf_settings.h"
#include "../project_ctr/makerom/src/keyset.h"

// For CIA_MAX_CONTENT constant
#define CIA_MAX_CONTENT 65535

// Forward declare functions from user_settings.c and keyset.c
extern void SetDefaults(user_settings *set);
extern int SetKeys(keys_struct *keys);

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
) {
    if (!output_path || !rsf_path || !exheader_path || !code_path || !icon_path || !banner_path) {
        set_error(error_msg, error_msg_size, "Invalid arguments: required paths missing");
        return -1;
    }

    // Allocate user settings
    user_settings *set = calloc(1, sizeof(user_settings));
    if (set == NULL) {
        set_error(error_msg, error_msg_size, "Not enough memory");
        return -1;
    }

    int result = -1;

    // Initialize settings
    init_UserSettings(set);
    initRand();

    // Initialize keys (required before building)
    InitKeys(&set->common.keys);

    // Set output format to CXI
    set->common.outFormat = CXI;

    // Set output filename
    set->common.outFileName = strdup(output_path);
    set->common.outFileName_mallocd = true;  // Flag that we allocated this

    // Set RSF path
    set->common.rsfPath = strdup(rsf_path);

    // Enable NCCH building
    set->ncch.buildNcch0 = true;

    // Set component paths (these are pointers in the struct, not char arrays)
    set->ncch.exheaderPath = strdup(exheader_path);
    set->ncch.codePath = strdup(code_path);
    set->ncch.iconPath = strdup(icon_path);
    set->ncch.bannerPath = strdup(banner_path);

    // Set optional paths if provided
    if (romfs_path) {
        set->ncch.romfsPath = strdup(romfs_path);
    }
    if (logo_path) {
        set->ncch.logoPath = strdup(logo_path);
    }
    if (plain_path) {
        set->ncch.plainRegionPath = strdup(plain_path);
    }

    // Get RSF settings
    if (GetRsfSettings(set) < 0) {
        set_error(error_msg, error_msg_size, "Failed to parse RSF file");
        goto cleanup;
    }

    // Build NCCH/CXI
    result = build_NCCH(set);
    if (result < 0) {
        set_error(error_msg, error_msg_size, "Failed to build CXI");
        goto cleanup;
    }

    // Verify buffer was allocated
    if (!set->common.workingFile.buffer || set->common.workingFile.size == 0) {
        set_error(error_msg, error_msg_size, "NCCH build did not produce output buffer");
        result = -1;
        goto cleanup;
    }

    // Write output file
    FILE *ncch_out = fopen(set->common.outFileName, "wb");
    if (!ncch_out) {
        set_error(error_msg, error_msg_size, "Failed to create output file");
        result = -1;
        goto cleanup;
    }

    WriteBuffer(set->common.workingFile.buffer, set->common.workingFile.size, 0, ncch_out);
    fclose(ncch_out);
    result = 0;

cleanup:
    // Free allocated memory
    // Note: free_UserSettings() already frees the set pointer internally
    free_UserSettings(set);

    return result;
}

int makerom_build_cci(
    const char* output_path,
    const char* content_path,
    int content_index,
    int content_id,
    char* error_msg,
    size_t error_msg_size
) {
    if (!output_path || !content_path) {
        set_error(error_msg, error_msg_size, "Invalid arguments: output_path and content_path required");
        return -1;
    }

    // Allocate user settings
    user_settings *set = calloc(1, sizeof(user_settings));
    if (set == NULL) {
        set_error(error_msg, error_msg_size, "Not enough memory");
        return -1;
    }

    int result = -1;

    // Initialize settings
    init_UserSettings(set);

    // Manually allocate contentPath array (normally done by ParseArgs)
    set->common.contentPath = calloc(CIA_MAX_CONTENT, sizeof(char*));
    if (set->common.contentPath == NULL) {
        set_error(error_msg, error_msg_size, "Not enough memory for contentPath");
        free(set);
        return -1;
    }

    // Initialize random number generator
    initRand();

    // Initialize keys
    InitKeys(&set->common.keys);

    // Set defaults (normally done in ParseArgs) - CRITICAL!
    SetDefaults(set);

    // Load cryptographic keys (normally done in ParseArgs after SetDefaults)
    if (SetKeys(&set->common.keys) != 0) {
        set_error(error_msg, error_msg_size, "Failed to load cryptographic keys");
        goto cleanup;
    }

    // Indicate we're not building NCCH from scratch but importing content
    set->ncch.buildNcch0 = false;

    // Initialize RSF settings
    if (GetRsfSettings(set) < 0) {
        set_error(error_msg, error_msg_size, "Failed to get RSF settings");
        goto cleanup;
    }

    // Set output format to CCI
    set->common.outFormat = CCI;

    // Set output filename
    set->common.outFileName = strdup(output_path);
    set->common.outFileName_mallocd = true;

    // Set content path
    set->common.contentPath[content_index] = strdup(content_path);

    // Set working file type
    set->common.workingFileType = infile_ncch;

    // Read the CXI content
    if (!AssertFile(set->common.contentPath[0])) {
        set_error(error_msg, error_msg_size, "Failed to open content file");
        goto cleanup;
    }

    u64 fileSize = GetFileSize64(set->common.contentPath[0]);
    FILE *ncch0 = fopen(set->common.contentPath[0], "rb");
    if (!ncch0) {
        set_error(error_msg, error_msg_size, "Failed to open content file");
        goto cleanup;
    }

    set->common.workingFile.size = fileSize;
    set->common.workingFile.buffer = malloc(fileSize);
    if (!set->common.workingFile.buffer) {
        set_error(error_msg, error_msg_size, "Not enough memory for content");
        fclose(ncch0);
        goto cleanup;
    }

    ReadFile64(set->common.workingFile.buffer, set->common.workingFile.size, 0, ncch0);
    fclose(ncch0);

    // Set content size for content 0 (needed by ImportNcchForCci)
    set->common.contentSize[content_index] = fileSize;

    // Build CCI
    result = build_CCI(set);
    if (result < 0) {
        set_error(error_msg, error_msg_size, "Failed to build CCI");
        goto cleanup;
    }

    result = 0;

cleanup:
    // Free allocated memory
    // Note: free_UserSettings() already frees the set pointer internally
    free_UserSettings(set);

    return result;
}
