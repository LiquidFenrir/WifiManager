#include "blocks.hpp"
#include "buttons.hpp"

#include "drawing.h"

FS_Archive sdmcArchive;
touchPosition touch;

slots_list list;

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

        pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
        list.draw_interface();
        pp2d_end_draw();

        u32 kDown = hidKeysDown();

        if(kDown & KEY_START) break;

        if(kDown & KEY_RIGHT) list.next_slot_right();
        if(kDown & KEY_LEFT) list.next_slot_left();

        if(kDown & KEY_DDOWN) list.next_backup_down();
        if(kDown & KEY_DUP) list.next_backup_up();

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
