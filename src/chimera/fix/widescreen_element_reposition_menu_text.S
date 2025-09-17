;# SPDX-License-Identifier: GPL-3.0-only

.intel_syntax noprefix

.text

.globl _reposition_gametype_indicator_asm
_reposition_gametype_indicator_asm:
    push eax
    call _reposition_gametype_indicator
    pop eax
    ret

.globl _reposition_f2_background_asm
_reposition_f2_background_asm:
    push ecx
    call _reposition_f2_background
    add esp, 4
    ret
