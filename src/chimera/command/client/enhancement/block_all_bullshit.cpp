// SPDX-License-Identifier: GPL-3.0-only

#include "../../../chimera.hpp"

namespace Chimera {
    bool Chimera::block_all_bullshit_command(int, const char **) noexcept {
        get_chimera().execute_command("chimera_block_gametype_indicator 1", true);
        get_chimera().execute_command("chimera_block_gametype_rules 1", true);
        get_chimera().execute_command("chimera_block_hold_f1 1", true);
        get_chimera().execute_command("chimera_block_loading_screen 1", true);
        get_chimera().execute_command("chimera_block_mouse_acceleration 1", true);
        return true;
    }
}
