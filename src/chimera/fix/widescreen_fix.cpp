// SPDX-License-Identifier: GPL-3.0-only

#include <cstring>
#include <cmath>

#include "../math_trig/math_trig.hpp"
#include "../halo_data/pad.hpp"

#include "../chimera.hpp"
#include "../signature/signature.hpp"
#include "../signature/hook.hpp"
#include "../event/tick.hpp"
#include "../output/output.hpp"

#include "../halo_data/tag.hpp"
#include "../halo_data/resolution.hpp"
#include "widescreen_fix.hpp"

extern "C" {
    void widescreen_input_text() noexcept;
    void widescreen_input_text_undo() noexcept;
    std::int16_t *widescreen_text_input_element;
    std::int16_t widescreen_left_offset_add = 0;
}

namespace Chimera {
    static void on_tick() noexcept;
    static float *scope_width;
    static float aspect_ratio = 4.0f / 3.0f;
    extern float widescreen_width_480p;
    static std::int32_t *console_width;
    static std::int32_t *text_max_x;
    static std::int16_t *f2_motd_x;
    static std::int16_t *f2_heading_x;
    static std::int16_t *f2_motd_body_x1;
    static std::int16_t *f2_motd_body_x2;
    static std::int16_t *f2_rules_1_x1;
    static std::int16_t *f2_rules_1_x2;
    static std::int16_t *f2_rules_2_x1;
    static std::int16_t *f2_rules_2_x2;
    static std::int16_t *f2_rules_3_x1;
    static std::int16_t *f2_rules_3_x2;
    static std::int16_t *f2_rules_4_x1;
    static std::int16_t *f2_rules_4_x2;
    static std::int32_t *f2_rules_4_left_x;
    static std::int32_t *console_output_width;
    static std::int16_t tabs[4];

    static std::uint16_t *tabs_ptr;


    extern "C" void reposition_menu_text_input() noexcept {
        std::int16_t increase = widescreen_left_offset_add;
        widescreen_text_input_element[1] += increase;
        widescreen_text_input_element[3] += increase;
    }

    extern "C" void unreposition_menu_text_input() noexcept {
        std::int16_t increase = widescreen_left_offset_add;
        widescreen_text_input_element[1] -= increase;
        widescreen_text_input_element[3] -= increase;
    }
    
    bool widescreen_fix_enabled() noexcept {
        return WidescreenFixSetting::WIDESCREEN_ON;
    }

    void set_up_widescreen_fix() noexcept {
        tabs_ptr = *reinterpret_cast<std::uint16_t **>(get_chimera().get_signature("widescreen_text_tab_sig").data() + 0x3);

        auto &widescreen_text_max_x_sig = get_chimera().get_signature("widescreen_text_max_x_sig");
        text_max_x = reinterpret_cast<std::int32_t *>(widescreen_text_max_x_sig.data() + 1);

        auto &widescreen_text_f2_text_position_motd_sig = get_chimera().get_signature("widescreen_text_f2_text_position_motd_sig");
        f2_motd_x = reinterpret_cast<std::int16_t *>(widescreen_text_f2_text_position_motd_sig.data() + 0xC);

        auto &widescreen_text_f2_text_position_heading_sig = get_chimera().get_signature("widescreen_text_f2_text_position_heading_sig");
        f2_heading_x = reinterpret_cast<std::int16_t *>(widescreen_text_f2_text_position_heading_sig.data() + 0x5);

        auto &widescreen_text_f2_text_position_motd_body_sig = get_chimera().get_signature("widescreen_text_f2_text_position_motd_body_sig");
        f2_motd_body_x1 = reinterpret_cast<std::int16_t *>(widescreen_text_f2_text_position_motd_body_sig.data() + 0x5);
        f2_motd_body_x2 = reinterpret_cast<std::int16_t *>(widescreen_text_f2_text_position_motd_body_sig.data() + 0x7 + 0x5);

        auto &widescreen_text_f2_text_position_rules_1_sig = get_chimera().get_signature("widescreen_text_f2_text_position_rules_1_sig");
        f2_rules_1_x1 = reinterpret_cast<std::int16_t *>(widescreen_text_f2_text_position_rules_1_sig.data() + 0x5);
        f2_rules_1_x2 = reinterpret_cast<std::int16_t *>(widescreen_text_f2_text_position_rules_1_sig.data() + 0x7 + 0x5);

        auto &widescreen_text_f2_text_position_rules_2_sig = get_chimera().get_signature("widescreen_text_f2_text_position_rules_2_sig");
        f2_rules_2_x1 = reinterpret_cast<std::int16_t *>(widescreen_text_f2_text_position_rules_2_sig.data() + 0x5);
        f2_rules_2_x2 = reinterpret_cast<std::int16_t *>(widescreen_text_f2_text_position_rules_2_sig.data() + 0x7 + 0x5);

        auto &widescreen_text_f2_text_position_rules_3_sig = get_chimera().get_signature("widescreen_text_f2_text_position_rules_3_sig");
        f2_rules_3_x1 = reinterpret_cast<std::int16_t *>(widescreen_text_f2_text_position_rules_3_sig.data() + 0x5);
        f2_rules_3_x2 = reinterpret_cast<std::int16_t *>(widescreen_text_f2_text_position_rules_3_sig.data() + 0x7 + 0x5);

        auto &widescreen_text_f2_text_position_rules_4_sig = get_chimera().get_signature("widescreen_text_f2_text_position_rules_4_sig");
        f2_rules_4_x1 = reinterpret_cast<std::int16_t *>(widescreen_text_f2_text_position_rules_4_sig.data() + 0x5);
        f2_rules_4_x2 = reinterpret_cast<std::int16_t *>(widescreen_text_f2_text_position_rules_4_sig.data() + 0x7 + 0x5);

        auto &widescreen_text_f2_text_position_rules_4_left_x_sig = get_chimera().get_signature("widescreen_text_f2_text_position_rules_4_left_x_sig");
        f2_rules_4_left_x = reinterpret_cast<std::int32_t *>(widescreen_text_f2_text_position_rules_4_left_x_sig.data() + 0x1);

        auto &widescreen_console_tabs_sig = get_chimera().get_signature("widescreen_console_tabs_sig");
        console_output_width = reinterpret_cast<std::int32_t *>(widescreen_console_tabs_sig.data() + 0x3A);
        overwrite(widescreen_console_tabs_sig.data() + 0x51 + 1, reinterpret_cast<std::int16_t *>(tabs));
        overwrite(widescreen_console_tabs_sig.data() + 0x56 + 3, reinterpret_cast<std::int16_t *>(tabs) + 2);

        static Hook input_text;
        auto &widescreen_input_text_sig = get_chimera().get_signature("widescreen_input_text_sig");
        write_jmp_call(reinterpret_cast<void *>(widescreen_input_text_sig.data()), input_text, reinterpret_cast<const void *>(widescreen_input_text), reinterpret_cast<const void *>(widescreen_input_text_undo));

        add_tick_event(on_tick);
        on_tick();
    }

    static void on_tick() noexcept {
        aspect_ratio = static_cast<float>(get_resolution().width) / static_cast<float>(get_resolution().height);
        widescreen_left_offset_add = static_cast<std::int16_t>((aspect_ratio * 480.000f - 640.000f) / 2.0f);

        // Change instructions if we need them to be changed
        widescreen_width_480p = aspect_ratio * 480.0f;

        if(*console_width != static_cast<std::int32_t>(widescreen_width_480p)) {
            overwrite(scope_width, widescreen_width_480p);

            overwrite(console_width, static_cast<std::int32_t>(widescreen_width_480p));
            overwrite(text_max_x, static_cast<std::uint32_t>(widescreen_width_480p));

            overwrite(f2_motd_x, static_cast<std::int16_t>(widescreen_width_480p - 640.0f + 625));
            overwrite(f2_heading_x, static_cast<std::int16_t>((widescreen_width_480p - 640.0f) / 2 + 630));
            overwrite(f2_motd_body_x1, static_cast<std::int16_t>((widescreen_width_480p - 640.0f) / 2 + 105));
            overwrite(f2_motd_body_x2, static_cast<std::int16_t>((widescreen_width_480p - 640.0f) / 2 + 625));
            overwrite(f2_rules_1_x1, static_cast<std::int16_t>((widescreen_width_480p - 640.0f) / 2 + 330));
            overwrite(f2_rules_1_x2, static_cast<std::int16_t>((widescreen_width_480p - 640.0f) / 2 + 630));
            overwrite(f2_rules_2_x1, static_cast<std::int16_t>((widescreen_width_480p - 640.0f) / 2 + 10));
            overwrite(f2_rules_2_x2, static_cast<std::int16_t>((widescreen_width_480p - 640.0f) / 2 + 310));
            overwrite(f2_rules_3_x1, static_cast<std::int16_t>((widescreen_width_480p - 640.0f) / 2 + 330));
            overwrite(f2_rules_3_x2, static_cast<std::int16_t>((widescreen_width_480p - 640.0f) / 2 + 630));
            overwrite(f2_rules_4_x1, static_cast<std::int16_t>((widescreen_width_480p - 640.0f) / 2 + 310));
            overwrite(f2_rules_4_x2, static_cast<std::int16_t>((widescreen_width_480p - 640.0f) / 2 + 450));
            overwrite(f2_rules_4_left_x, static_cast<std::uint32_t>((widescreen_width_480p - 640.0f) / 2 + 10));
            overwrite(console_output_width, static_cast<std::int32_t>(widescreen_width_480p));

            tabs[0] = static_cast<std::int16_t>(0.25f * widescreen_width_480p);
            tabs[1] = static_cast<std::int16_t>(0.50f * widescreen_width_480p);
            tabs[2] = static_cast<std::int16_t>(0.75f * widescreen_width_480p);
        }
    }
}
