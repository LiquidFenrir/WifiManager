#pragma once

#include "basic.hpp"
#include "blocks.hpp"
#include "drawing.h"

enum ButtonsID {
    BUTTON_SLOT_1,
    BUTTON_SLOT_1_SAVE,
    BUTTON_SLOT_1_WRITE,

    BUTTON_SLOT_2,
    BUTTON_SLOT_2_SAVE,
    BUTTON_SLOT_2_WRITE,

    BUTTON_SLOT_3,
    BUTTON_SLOT_3_SAVE,
    BUTTON_SLOT_3_WRITE,

    BUTTON_ARROW_UP,
    BUTTON_DELETE,
    BUTTON_ARROW_DOWN,

    BUTTONS_AMOUNT,
};

class Area
{
    public:
        Area(int, int, int, int);
        void draw(void);

    private:
        int x, y, width, height;
};

class Button
{
    public:
        Button();
        Button(int, int, int, int, bool (*)(void), void (*)(void));
        Button(int, int, int, int, bool (*)(void), void (*)(void), size_t);

        bool (*is_pressable)(void);
        void (*action)(void);
        void draw(void);
        bool is_pressed(void);
        bool selected;

    private:
        int x, y, width, height;
        size_t image_id;
};

extern std::vector<Button> buttons;
void init_buttons(void);
