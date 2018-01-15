#include "buttons.hpp"

std::vector<Button> buttons;

Area::Area(int x, int y, int width, int height)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}
void Area::draw(void)
{
    pp2d_draw_rectangle(this->x, this->y, this->width, this->height, COLOR_BLACK);
    pp2d_draw_rectangle(this->x+2, this->y+2, this->width-4, this->height-4, COLOR_FOREGROUD);
}

Button::Button(int x, int y, int width, int height, bool (*is_pressable)(void), void (*action)(void))
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->is_pressable = is_pressable;
    this->action = action;
    this->selected = false;
    this->image_id = 0;
};
Button::Button(int x, int y, int width, int height, bool (*is_pressable)(void), void (*action)(void), size_t image_id)
{
   this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->is_pressable = is_pressable;
    this->action = action;
    this->selected = false;
    this->image_id = image_id;
}


bool Button::is_pressed(void)
{
    return touch.px >= x && touch. px <= x+width && touch.py >= y && touch.py <= y+height;
}
void Button::draw(void)
{
    pp2d_draw_rectangle(this->x, this->y, this->width, this->height, COLOR_BLACK);
    pp2d_draw_rectangle(this->x+2, this->y+2, this->width-4, this->height-4, !this->is_pressable() ? COLOR_UNSELECTABLE : (this->is_pressed() || this->selected ? COLOR_CURSOR : COLOR_FOREGROUD));
    if(this->image_id != 0)
    {
        pp2d_draw_texture_blend(this->image_id, this->x, this->y, COLOR_BLACK);
    }
}

// Wrappers
bool selected_new_backup()
{
    return list.selected_new_backup();
}
bool selected_other_backup()
{
    return !list.selected_new_backup();
}
bool can_go_down()
{
    return list.can_go_down(); 
}

void go_up()
{
    list.next_backup_up();
}
void go_down()
{
    list.next_backup_down();
}

void delete_selected_backup()
{
    list.delete_selected_backup();
}

bool exists_slot_1(void)
{
    return list.slot_exists(0);
}
bool exists_slot_2(void)
{
    return list.slot_exists(1);
}
bool exists_slot_3(void)
{
    return list.slot_exists(2);
}

void select_slot_1()
{
    list.select_slot(0);
}
void select_slot_2()
{
    list.select_slot(1);
}
void select_slot_3()
{
    list.select_slot(2);
}

void write_to_slot_1(void)
{
    list.write_to(0);
}
void write_to_slot_2(void)
{
    list.write_to(1);
}
void write_to_slot_3(void)
{
    list.write_to(2);
}

void save_from_slot_1(void)
{
    list.save_from(0);
}
void save_from_slot_2(void)
{
    list.save_from(1);
}
void save_from_slot_3(void)
{
    list.save_from(2);
}
//-----------------------------------------------------

void init_buttons(void)
{
    DEBUG("initing buttons!\n");
    bool (*exists_slot[3])(void) = {
        exists_slot_1,
        exists_slot_2,
        exists_slot_3,
    };
    void (*select_slot[3])(void) = {
        select_slot_1,
        select_slot_2,
        select_slot_3,
    };
    void (*write_to_slot[3])(void) = {
        write_to_slot_1,
        write_to_slot_2,
        write_to_slot_3,
    };
    void (*save_from_slot[3])(void) = {
        save_from_slot_1,
        save_from_slot_2,
        save_from_slot_3,
    };

    int y_offset = 15;
    int x_offset = 15;
    for(int i = 0; i < CFG_WIFI_SLOTS; i++)
    {
        int x = x_offset + i*100;
        buttons.push_back(Button(x+27, y_offset, 63, 56, exists_slot[i], select_slot[i]));
        buttons.push_back(Button(x, y_offset, 29, 29, exists_slot[i], save_from_slot[i], TEXTURE_SAVE));
        buttons.push_back(Button(x, y_offset+27, 29, 29, selected_other_backup, write_to_slot[i], TEXTURE_WRITE));
    }

    int buttons_width = 35;
    x_offset = 320-x_offset-buttons_width-4;
    y_offset += 66;
    buttons.push_back(Button(x_offset, y_offset, buttons_width+4, 48, selected_other_backup, go_up, TEXTURE_ARROW_UP));
    y_offset += 48;
    buttons.push_back(Button(x_offset, y_offset-2, buttons_width+4, 48+4, selected_other_backup, delete_selected_backup, TEXTURE_DELETE));
    y_offset += 48;
    buttons.push_back(Button(x_offset, y_offset, buttons_width+4, 48, can_go_down, go_down, TEXTURE_ARROW_DOWN));

    DEBUG("done!\n");
}