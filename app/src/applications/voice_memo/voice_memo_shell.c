/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <zephyr/shell/shell.h>
#include "managers/zsw_recording_manager.h"

static int cmd_voice_memo_start(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    int ret = zsw_recording_manager_start();
    if (ret == 0) {
        shell_print(sh, "Recording started");
    } else {
        shell_print(sh, "Failed to start recording: %d", ret);
    }
    return ret;
}

static int cmd_voice_memo_stop(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    int ret = zsw_recording_manager_stop();
    if (ret == 0) {
        shell_print(sh, "Recording stopped");
    } else {
        shell_print(sh, "Failed to stop recording: %d", ret);
    }
    return ret;
}

static int cmd_voice_memo_list(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    zsw_recording_entry_t entries[ZSW_RECORDING_MAX_FILES];
    int count = zsw_recording_manager_list(entries, ARRAY_SIZE(entries));

    if (count < 0) {
        shell_print(sh, "Error listing recordings: %d", count);
        return count;
    }

    shell_print(sh, "Recordings: %d", count);
    for (int i = 0; i < count; i++) {
        uint32_t secs = (entries[i].duration_ms + 999) / 1000;
        shell_print(sh, "  %s  %u:%02u  %u bytes",
                    entries[i].filename,
                    secs / 60, secs % 60,
                    entries[i].size_bytes);
    }

    uint32_t free_bytes = 0;
    if (zsw_recording_manager_get_free_space(&free_bytes) == 0) {
        shell_print(sh, "Free space: %u KB", free_bytes / 1024);
    }
    return 0;
}

static int cmd_voice_memo_delete(const struct shell *sh, size_t argc, char **argv)
{
    if (argc < 2) {
        shell_print(sh, "Usage: voice_memo delete <filename>");
        return -EINVAL;
    }
    int ret = zsw_recording_manager_delete(argv[1]);
    shell_print(sh, ret == 0 ? "Deleted" : "Delete failed: %d", ret);
    return ret;
}

static int cmd_voice_memo_status(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    bool recording = zsw_recording_manager_is_recording();
    shell_print(sh, "Recording: %s", recording ? "yes" : "no");

    int count = zsw_recording_manager_get_count();
    shell_print(sh, "Total recordings: %d", count);

    uint32_t free_bytes = 0;
    int ret = zsw_recording_manager_get_free_space(&free_bytes);
    if (ret < 0) {
        shell_print(sh, "Free space: unknown");
    } else {
        uint32_t secs = free_bytes / 1024 / 4;
        shell_print(sh, "Free space: %u KB (%u min at 32kbps)",
                    free_bytes / 1024, secs / 60);
    }
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_voice_memo,
                               SHELL_CMD(start, NULL, "Start recording", cmd_voice_memo_start),
                               SHELL_CMD(stop, NULL, "Stop recording", cmd_voice_memo_stop),
                               SHELL_CMD(list, NULL, "List recordings", cmd_voice_memo_list),
                               SHELL_CMD_ARG(delete, NULL, "Delete recording", cmd_voice_memo_delete, 2, 0),
                               SHELL_CMD(status, NULL, "Show recording status", cmd_voice_memo_status),
                               SHELL_SUBCMD_SET_END
                              );
SHELL_CMD_REGISTER(voice_memo, &sub_voice_memo, "Voice memo commands", NULL);
