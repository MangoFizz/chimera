// SPDX-License-Identifier: GPL-3.0-only

#include <cstring>
#include <memory>
#include <command/command.hpp>
#include "../localization/localization.hpp"
#include "../chimera.hpp"
#include "command.hpp"

using namespace Balltze;

namespace Chimera {
    std::vector<std::string> split_arguments(const char *command) noexcept {
        // This is the vector to return.
        std::vector<std::string> arguments;

        // This value will be true if we are inside quotes, during which the word will not separate into arguments.
        bool in_quotes = false;

        // If using a backslash, add the next character to the string regardless of what it is.
        bool escape_character = false;

        // Regardless of if there were any characters, there was an argument.
        bool allow_empty_argument = false;

        // Get the command
        std::size_t command_size = std::strlen(command);

        // Get the argument
        std::string argument;
        for(std::size_t i = 0; i < command_size; i++) {
            if(escape_character) {
                escape_character = false;
            }
            // Escape character - this will be used to include the next character regardless of what it is
            else if(command[i] == '\\') {
                escape_character = true;
                continue;
            }
            // If a whitespace or octotothorpe is in quotations in the argument, then it is considered part of the argument.
            else if(command[i] == '"') {
                in_quotes = !in_quotes;
                allow_empty_argument = true;
                continue;
            }
            else if((command[i] == ' ' || command[i] == '\r' || command[i] == '\n' || command[i] == '#') && !in_quotes) {
                // Add argument if not empty.
                if(argument != "" || allow_empty_argument) {
                    arguments.push_back(argument);
                    argument = "";
                    allow_empty_argument = false;
                }

                // Terminate if beginning a comment.
                if(command[i] == '#') {
                    break;
                }
                continue;
            }
            argument += command[i];
        }

        // Add the last argument.
        if(argument != "" || allow_empty_argument) {
            arguments.push_back(argument);
        }

        return arguments;
    }

    std::string unsplit_arguments(const std::vector<std::string> &arguments) noexcept {
        // This is the string to return.
        std::string unsplit;

        for(std::size_t i = 0; i < arguments.size(); i++) {
            // This is a reference to the argument we're dealing with.
            const std::string &argument = arguments[i];

            // This will be the final string we append to the unsplit string.
            std::string argument_final;

            // Set this to true if we need to surround this argument with quotes.
            bool surround_with_quotes = false;

            // Go through each character and add them one-by-one to argument_final.
            for(const char &c : argument) {
                switch(c) {
                    // Backslashes and quotation marks should be escaped.
                    case '\\':
                    case '"':
                        argument_final += '\\';
                        break;

                    // If we're using spaces or octothorpes, the argument should be surrounded with quotation marks. We could escape those, but this is more readable.
                    case '#':
                    case ' ':
                        surround_with_quotes = true;
                        break;

                    default:
                        break;
                }
                argument_final += c;
            }

            if(surround_with_quotes) {
                argument_final = std::string("\"") + argument_final + "\"";
            }

            unsplit += argument_final;

            // Add the space to separate the next argument.
            if(i + 1 < arguments.size()) {
                unsplit += " ";
            }
        }

        return unsplit;
    }

    static void add_command(const std::string &name, const std::string &category, Balltze::CommandFunction function, bool autosave = false, std::size_t min_args = 0, std::size_t max_args = 0) {
        CommandBuilder builder;
        
        builder.name(name.substr(8))
            .category(category)
            .help(localize((name + "_command_help").c_str()))
            .function(function);
        
        for(std::size_t i = 0; i < max_args; i++) {
            if(i < min_args) {
                builder.param(HSC_DATA_TYPE_SIZE, std::string("arg") + std::to_string(i + 1), false);
            }
            else {
                builder.param(HSC_DATA_TYPE_SIZE, std::string("arg") + std::to_string(i + 1), true);
            }
        }

        builder.autosave(autosave)
            .can_call_from_console()
            .is_public()
            .create(COMMAND_SOURCE_CHIMERA);
    }

    void Chimera::get_all_commands() noexcept {
        #define ADD_COMMAND(name, category, feature, command_fn, autosave, min_args, max_args) \
            extern bool command_fn(int, const char **); \
            static_assert(autosave == false || autosave == true, "autosave value is not a boolean"); \
            add_command(name, category, [](const std::vector<std::string> &args) -> bool { \
                std::vector<const char *> argv; \
                argv.reserve(args.size()); \
                for (const auto &arg : args) { \
                    argv.push_back(arg.c_str()); \
                } \
                return command_fn(args.size(), argv.data()); \
            }, autosave, min_args, max_args);

        // Chimera-specific commands
        // this->p_commands.emplace_back("chimera", localize("chimera_category_core"), "core", localize("chimera_command_help"), Chimera::chimera_command, false, 0, 1);
        // this->p_commands.emplace_back("chimera_signature_info", localize("chimera_category_core"), "core", localize("chimera_signature_info_command_help"), Chimera::signature_info_command, false, 1, 1);
        ADD_COMMAND("chimera_about", "chimera_category_core", "core", about_command, true, 0, 0);
        ADD_COMMAND("chimera_language", "chimera_category_core", "core", language_command, true, 0, 1);
        ADD_COMMAND("chimera_chat_color_help", "chimera_category_custom_chat", "client_custom_chat", chat_color_help_command, true, 0, 1);
        ADD_COMMAND("chimera_chat_block_server_messages", "chimera_category_custom_chat", "client_custom_chat", chat_block_server_messages_command, true, 0, 1);
        ADD_COMMAND("chimera_chat_block_ips", "chimera_category_custom_chat", "client_custom_chat", chat_block_ips_command, true, 0, 1);

        // Debug
        ADD_COMMAND("chimera_budget", "chimera_category_debug", "client", budget_command, true, 0, 1);

        ADD_COMMAND("chimera_devmode", "chimera_category_debug", "core_devmode", devmode_command, true, 0, 1);
        ADD_COMMAND("chimera_load_ui_map", "chimera_category_debug", "client", load_ui_map_command, false, 0, 0);
        ADD_COMMAND("chimera_player_info", "chimera_category_debug", "core", player_info_command, false, 0, 1);
        ADD_COMMAND("chimera_apply_damage", "chimera_category_debug", "core", apply_damage_command, false, 2, 5);
        ADD_COMMAND("chimera_block_damage", "chimera_category_debug", "core", block_damage_command, false, 0, 1);
        ADD_COMMAND("chimera_show_coordinates", "chimera_category_debug", "client", show_coordinates_command, true, 0, 1);
        ADD_COMMAND("chimera_show_fps", "chimera_category_debug", "client", show_fps_command, true, 0, 1);
        ADD_COMMAND("chimera_tps", "chimera_category_debug", "core", tps_command, false, 0, 1);
        ADD_COMMAND("chimera_teleport", "chimera_category_debug", "core", teleport_command, false, 1, 4);
        ADD_COMMAND("chimera_script_command_dump", "chimera_category_debug", "core", script_command_dump_command, false, 0, 0);
        ADD_COMMAND("chimera_send_chat_message", "chimera_category_debug", "client", send_chat_message_command, false, 2, 2);
        ADD_COMMAND("chimera_map_info", "chimera_category_debug", "client", map_info_command, false, 0, 0);

        // Enhancements
        // this->p_commands.emplace_back("chimera_block_all_bullshit", localize("chimera_category_enhancement"), "client", localize("chimera_block_all_bullshit_help"), Chimera::block_all_bullshit_command, false, 0, 0);
        ADD_COMMAND("chimera_block_buffering", "chimera_category_enhancement", "client_disable_buffering", block_buffering_command, true, 0, 1);
        ADD_COMMAND("chimera_block_extra_weapon", "chimera_category_enhancement", "client_block_extra_weapon", block_extra_weapon_command, false, 0, 0);
        ADD_COMMAND("chimera_unblock_all_extra_weapons", "chimera_category_enhancement", "client_block_extra_weapon", unblock_all_extra_weapons_command, false, 0, 0);
        ADD_COMMAND("chimera_set_name", "chimera_category_enhancement", "client", set_name_command, true, 0, 1);
        ADD_COMMAND("chimera_set_color", "chimera_category_enhancement", "client", set_color_command, true, 0, 1);
        ADD_COMMAND("chimera_throttle_fps", "chimera_category_enhancement", "client", throttle_fps_command, true, 0, 1);
        ADD_COMMAND("chimera_fp_reverb", "chimera_category_enhancement", "client_fp_reverb", fp_reverb_command, true, 0, 1);

        // Server
        ADD_COMMAND("chimera_spectate", "chimera_category_server", "client_spectate", spectate_command, false, 1, 1);
        ADD_COMMAND("chimera_spectate_next", "chimera_category_server", "client_spectate", spectate_next_command, false, 0, 0);
        ADD_COMMAND("chimera_spectate_previous", "chimera_category_server", "client_spectate", spectate_previous_command, false, 0, 0);
        ADD_COMMAND("chimera_spam_to_join", "chimera_category_server", "client", spam_to_join_command, true, 0, 1);
        ADD_COMMAND("chimera_spectate_team_only", "chimera_category_server", "client_spectate", spectate_team_only_command, true, 0, 1);
        ADD_COMMAND("chimera_delete_empty_weapons", "chimera_category_server", "core", delete_empty_weapons_command, true, 0, 1);
        ADD_COMMAND("chimera_player_list", "chimera_category_server", "core", player_list_command, false, 0, 0);
        ADD_COMMAND("chimera_block_equipment_rotation", "chimera_category_server", "core_null_rotation", block_equipment_rotation_command, true, 0, 1);
        ADD_COMMAND("chimera_allow_all_passengers", "chimera_category_server", "core_mtv", allow_all_passengers_command, true, 0, 1);
        ADD_COMMAND("chimera_master_server", "chimera_category_server", "core", master_server_command, true, 0, 4);

        // Visuals
        ADD_COMMAND("chimera_af", "chimera_category_visual", "client_af", af_command, true, 0, 1);
        ADD_COMMAND("chimera_block_auto_center", "chimera_category_visual", "client", block_auto_center_command, true, 0, 1);
        ADD_COMMAND("chimera_block_camera_shake", "chimera_category_visual", "client_camera_shake", block_camera_shake_command, true, 0, 1);
        ADD_COMMAND("chimera_block_gametype_indicator", "chimera_category_visual", "client_gametype_indicator", block_gametype_indicator_command, true, 0, 1);
        ADD_COMMAND("chimera_block_gametype_rules", "chimera_category_visual", "client_gametype_rules", block_gametype_rules_command, true, 0, 1);
        ADD_COMMAND("chimera_block_hold_f1", "chimera_category_visual", "client_hold_f1", block_hold_f1_command, true, 0, 1);
        ADD_COMMAND("chimera_block_letterbox", "chimera_category_visual", "client_letterbox", block_letterbox_command, true, 0, 1);
        ADD_COMMAND("chimera_block_loading_screen", "chimera_category_visual", "client_loading_screen", block_loading_screen_command, true, 0, 1);
        ADD_COMMAND("chimera_block_multitexture_overlays", "chimera_category_visual", "client_multitexture_overlays", block_multitexture_overlays_command, true, 0, 1);
        ADD_COMMAND("chimera_block_server_ip", "chimera_category_visual", "client_server_ip", block_server_ip_command, true, 0, 1);
        ADD_COMMAND("chimera_block_zoom_blur", "chimera_category_visual", "client_zoom_blur", block_zoom_blur_command, true, 0, 1);
        ADD_COMMAND("chimera_fov", "chimera_category_visual", "client", fov_command, true, 0, 1);
        ADD_COMMAND("chimera_fov_vehicle", "chimera_category_visual", "client", fov_vehicle_command, true, 0, 1);
        ADD_COMMAND("chimera_fov_cinematic", "chimera_category_visual", "client", fov_cinematic_command, true, 0, 1);
        ADD_COMMAND("chimera_model_detail", "chimera_category_visual", "client_lod", model_detail_command, true, 0, 1);
        ADD_COMMAND("chimera_shrink_empty_weapons", "chimera_category_visual", "client", shrink_empty_weapons_command, true, 0, 1);
        ADD_COMMAND("chimera_split_screen_hud", "chimera_category_visual", "client_split_screen_hud", split_screen_hud_command, true, 0, 1);
        ADD_COMMAND("chimera_uncap_cinematic", "chimera_category_visual", "client_interpolate", uncap_cinematic_command, true, 0, 1);
        ADD_COMMAND("chimera_invert_shader_flags", "chimera_category_visual", "client_custom", invert_shader_flags_command, true, 0, 1);

        // Lua
        ADD_COMMAND("chimera_lua_reload_scripts", "chimera_category_lua", "core", reload_scripts_command, false, 0, 0);

        // Mouse
        ADD_COMMAND("chimera_block_mouse_acceleration", "chimera_category_mouse", "client_mouse_acceleration", block_mouse_acceleration_command, true, 0, 1);
        ADD_COMMAND("chimera_mouse_sensitivity", "chimera_category_mouse", "client_mouse_sensitivity", mouse_sensitivity_command, true, 0, 2);

        // Controller
        ADD_COMMAND("chimera_aim_assist", "chimera_category_controller", "client", aim_assist_command, true, 0, 1);
        ADD_COMMAND("chimera_auto_uncrouch", "chimera_category_controller", "client_auto_uncrouch", auto_uncrouch_command, true, 0, 1);
        ADD_COMMAND("chimera_diagonals", "chimera_category_controller", "client_diagonals", diagonals_command, true, 0, 1);
        ADD_COMMAND("chimera_deadzones", "chimera_category_controller", "client_deadzones", deadzones_command, true, 0, 1);
        ADD_COMMAND("chimera_block_button_quotes", "chimera_category_controller", "client_quote_prompt", block_button_quotes_command, true, 0, 1);

        // Bookmark
        ADD_COMMAND("chimera_bookmark_list", "chimera_category_bookmark", "client", bookmark_list_command, false, 0, 0);
        ADD_COMMAND("chimera_bookmark_add", "chimera_category_bookmark", "client", bookmark_add_command, false, 0, 2);
        ADD_COMMAND("chimera_bookmark_connect", "chimera_category_bookmark", "client", bookmark_connect_command, false, 1, 1);
        ADD_COMMAND("chimera_bookmark_delete", "chimera_category_bookmark", "client", bookmark_delete_command, false, 0, 1);
        ADD_COMMAND("chimera_history_list", "chimera_category_bookmark", "client", history_list_command, false, 0, 0);
        ADD_COMMAND("chimera_history_connect", "chimera_category_bookmark", "client", history_connect_command, false, 1, 1);
    }
}
