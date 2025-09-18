// SPDX-License-Identifier: GPL-3.0-only

#ifndef CHIMERA_COMMAND_HPP
#define CHIMERA_COMMAND_HPP

#include <cstring>
#include <vector>
#include <string>

namespace Chimera {
    #define BOOL_TO_STR(boolean) (boolean ? "true" : "false")
    #define STR_TO_BOOL(str) (std::strcmp(str, "1") == 0 || std::strcmp(str, "true") == 0)

    /**
     * Result of a command
     */
    enum CommandResult {
        /** Command was a success. If invoked by console and the command can save, save here. */
        COMMAND_RESULT_SUCCESS = 0,

        /** Command failed. Do not save. */
        COMMAND_RESULT_FAILED_ERROR,

        /** Command not found. Do not save. */
        COMMAND_RESULT_FAILED_ERROR_NOT_FOUND,

        /** Command requires more arguments than given. Do not save. */
        COMMAND_RESULT_FAILED_NOT_ENOUGH_ARGUMENTS,

        /** Command requires fewer arguments than given. Do not save. */
        COMMAND_RESULT_FAILED_TOO_MANY_ARGUMENTS,

        /** Command feature is not available on this version of Halo CE. Do not save. */
        COMMAND_RESULT_FAILED_FEATURE_NOT_AVAILABLE
    };

    /**
     * Split the arguments
     * @param command arguments to split
     * @return        vector of split arguments
     */
    std::vector<std::string> split_arguments(const char *command) noexcept;

    /**
     * Unsplit the arguments
     * @param  arguments arguments to unsplit
     * @return           combined arguments
     */
    std::string unsplit_arguments(const std::vector<std::string> &arguments) noexcept;
}

#endif
