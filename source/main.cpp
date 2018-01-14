#include "blocks.hpp"

extern "C" {
#include "stringutils.h"
}
#include "drawing.h"

FS_Archive sdmcArchive;

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
    pp2d_load_texture_png(TEXTURE_ARROW, "romfs:/arrow.png");

    FSUSER_OpenArchive(&sdmcArchive, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
    slots_list list = slots_list(WORKING_DIR);
    bool wrote = false;

    while(aptMainLoop())
    {
        pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
        list.draw_interface();
        pp2d_end_draw();

        hidScanInput();
        u32 kDown = hidKeysDown();
        u32 kHeld = hidKeysHeld();

        if(kDown & KEY_START) break;

        if(kDown & KEY_RIGHT) list.next_slot_right();
        if(kDown & KEY_LEFT) list.next_slot_left();

        if(kDown & KEY_DDOWN) list.next_backup_down();
        if(kDown & KEY_DUP) list.next_backup_up();

        if((kDown | kHeld) & KEY_TOUCH)
        {
            touchPosition touch;
            hidTouchRead(&touch);
        }
    }

    pp2d_exit();
    romfsExit();
    cfguExit();

    if(wrote)
    {
        APT_HardwareResetAsync();
        for(;;);
    }

    return 0;
}
