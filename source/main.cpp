#include "blocks.hpp"
#include "buttons.hpp"

#include "drawing.h"

FS_Archive sdmcArchive;
touchPosition touch;

slots_list list;

void draw_instructions(void)
{
    int x_offset = 15;
    int y_offset = 15;
    Area bg_area = Area(x_offset, y_offset, 400-x_offset*2, 240-y_offset*2);
    bg_area.draw();

    y_offset += 25;
    pp2d_draw_text_center(GFX_TOP, y_offset, 1.0, 1.0, COLOR_BLACK, "Instructions");
    y_offset += 40;
    pp2d_draw_text_center(GFX_TOP, y_offset, 0.6, 0.6, COLOR_BLACK, "Press \uE000 to write the backup to the selected slot.");
    y_offset += 20;
    pp2d_draw_text_center(GFX_TOP, y_offset, 0.6, 0.6, COLOR_BLACK, "Press \uE001 to backup the selected slot.");
    y_offset += 20;
    pp2d_draw_text_center(GFX_TOP, y_offset, 0.6, 0.6, COLOR_BLACK, "Press \uE002 to delete the highlighted backup.");
    y_offset += 20;
    pp2d_draw_text_center(GFX_TOP, y_offset, 0.6, 0.6, COLOR_BLACK, "Press \uE003 to toggle the passwords visibility.");
}

int main(int argc, char ** argv)
{
    cfguInit();
    romfsInit();
    pp2d_init();
    consoleDebugInit(debugDevice_SVC);

    pp2d_set_screen_color(GFX_TOP, COLOR_BACKGROUND);
    pp2d_set_screen_color(GFX_BOTTOM, COLOR_BACKGROUND);
    pp2d_load_texture_png(TEXTURE_SAVE, "romfs:/save.png");
    pp2d_load_texture_png(TEXTURE_WRITE, "romfs:/write.png");
    pp2d_load_texture_png(TEXTURE_HIDE, "romfs:/hide.png");
    pp2d_load_texture_png(TEXTURE_DELETE, "romfs:/delete.png");
    pp2d_load_texture_png(TEXTURE_ARROW_UP, "romfs:/arrow_up.png");
    pp2d_load_texture_png(TEXTURE_ARROW_DOWN, "romfs:/arrow_down.png");

    FSUSER_OpenArchive(&sdmcArchive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
    list = slots_list(WORKING_DIR);
    init_buttons();

    while(aptMainLoop())
    {
        hidScanInput();
        hidTouchRead(&touch);

        u32 kDown = hidKeysDown();
        u32 kHeld = hidKeysHeld();

        pp2d_begin_draw(GFX_TOP, GFX_LEFT);
        if(kHeld & KEY_SELECT)
            draw_instructions();
        else
            list.draw_interface();
        pp2d_end_draw();

        if(kHeld & KEY_SELECT)
            continue;

        if(kDown & KEY_START) break;

        if(kDown & KEY_RIGHT) list.next_slot_right();
        if(kDown & KEY_LEFT) list.next_slot_left();

        if(kDown & KEY_DDOWN) list.next_backup_down();
        if(kDown & KEY_DUP) list.next_backup_up();

        if(kDown & KEY_A) list.write_to_selected();
        if(kDown & KEY_B) list.save_from_selected();
        if(kDown & KEY_X) list.delete_selected_backup();
        if(kDown & KEY_Y) list.toggle_password_visibility();

        if((kDown) & KEY_TOUCH)
        {
            for(auto button : buttons)
            {
                if(button.is_pressable() && button.is_pressed() && button.action != NULL)
                    button.action();
            }
        }
    }

    pp2d_exit();
    romfsExit();
    cfguExit();

    APT_HardwareResetAsync();
    for(;;);

    return 0;
}
