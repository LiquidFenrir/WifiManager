#include "blocks.hpp"
#include <sstream>
#include <iomanip>
#include <ctime>
#include "buttons.hpp"

extern "C" {
#include "file.h"
#include "checksum.h"
#include "keyboard.h"
}
#include "drawing.h"

wifi_slot::wifi_slot()
{
    id = -1;
    path = "";
    name = "";
    password = "";
    exists = false;
    editable_security = false;
}

wifi_slot::wifi_slot(int id)
{
    this->id = id;
    this->read_slot();
}
wifi_slot::wifi_slot(std::string path)
{
    this->id = -1;
    this->path = path;
    this->read_slot();
}

void wifi_slot::fix_slot_crc(void)
{
    this->slot_data.crc_checksum = crc_16((u8*)(&this->slot_data)+4, 0x410);
}

Result wifi_slot::read_slot(void)
{
    Result ret = 0;
    this->exists = false;
    if(id == -1)
    {
        Handle handle;

        if(R_FAILED(ret = openFile(this->path.c_str(), sdmcArchive, &handle, false))) return ret;
        FSFILE_Read(handle, NULL, 0, &this->slot_data, sizeof(this->slot_data));
        FSFILE_Close(handle);

        ret = 0;
    }
    else
        ret = CFG_GetConfigInfoBlk8(sizeof(this->slot_data), CFG_WIFI_BLKID+this->id, (u8*)&this->slot_data);

    this->exists = this->slot_data.version;
    this->editable_security = this->slot_data.editable_security;
    this->type = (wifi_multissid_type)this->slot_data.multissid_type;

    if(this->exists)
    {
        this->name = std::string(slot_data.main_network.SSID, this->slot_data.main_network.SSID_length);
        this->password = std::string(slot_data.main_network.password_text, 0x40);
        this->encryption = (wifi_encryption_type)this->slot_data.main_network.encryption_type;

        if(!this->editable_security)
        {
            this->name = std::string(this->slot_data.multissid_networks[0].SSID, this->slot_data.main_network.SSID_length);
            this->password = std::string(this->slot_data.multissid_networks[0].password_text);
            this->encryption = (wifi_encryption_type)this->slot_data.multissid_networks[0].encryption_type;
        }

        if(this->password.empty() && \
          (this->encryption == WIFI_ENCRYPTION_WEP_64 || \
           this->encryption == WIFI_ENCRYPTION_WEP_128 || \
           this->encryption == WIFI_ENCRYPTION_WEP_152))
        {
            this->password = std::string((char*)this->slot_data.multissid_networks[0].password_key, 0x20);
        }
        if(this->password.empty())
        {
            this->password = "(not available)";
        }
    }

    return ret;
}

Result wifi_slot::write_slot(void)
{
    this->fix_slot_crc();
    if(id == -1)
    {
        Handle handle;

        openFile(this->path.c_str(), sdmcArchive, &handle, true);
        FSFILE_Write(handle, NULL, 0, &this->slot_data, sizeof(this->slot_data), 0);
        FSFILE_Close(handle);

        return 0;
    }
    else
        return CFG_SetConfigInfoBlk8(sizeof(this->slot_data), CFG_WIFI_BLKID+this->id, (u8*)&this->slot_data);
}

Result wifi_slot::delete_slot(void)
{
    return deleteFile(this->path.c_str(), sdmcArchive);
}
void wifi_slot::copy_slot(wifi_slot other_slot)
{
    this->slot_data = other_slot.get_data();
    this->write_slot();
    this->read_slot();
    if(this->id != -1)
        CFG_UpdateConfigSavegame();
}
wifi_slot_s wifi_slot::get_data(void)
{
    return this->slot_data;
}
void wifi_slot::draw_info(bool to_the_right, bool hide_password)
{
    pp2d_draw_on(GFX_TOP, GFX_LEFT);
    int x_offset = 15 + to_the_right*190;
    int y_offset = 15;
    Area slot_area = Area(x_offset, y_offset, 180, 210);
    slot_area.draw();

    int text_x = x_offset+9;
    int text_y = y_offset+9;
    float text_scale_x = 0.5;
    float text_scale_y = 0.5;
    if(this->id == -1)
        pp2d_draw_text(text_x, text_y, text_scale_x, text_scale_y, COLOR_BLACK, "Saved Slot");
    else
        pp2d_draw_textf(text_x, text_y, text_scale_x, text_scale_y, COLOR_BLACK, "Slot %i", this->id+1);
    text_y += 20;

    if(this->editable_security)
        pp2d_draw_text(text_x, text_y, text_scale_x, text_scale_y, COLOR_BLACK, "Type: Normal");
    else
        pp2d_draw_textf(text_x, text_y, text_scale_x, text_scale_y, COLOR_BLACK, "Type: %s", this->type ? "WPS" : "AOSS");

    text_y += 20;
    pp2d_draw_textf(text_x, text_y, text_scale_x, text_scale_y, COLOR_BLACK, "SSID:\n%s", this->name.c_str());

    text_y += 40;
    std::string encryption_type_string[WIFI_ENCRYPTION_AMOUNT] = {
        "Not secured",
        "WEP 40-bit",
        "WEP 104-bit",
        "WEP 128-bit",
        "WPA TKIP",
        "WPA2 TKIP",
        "WPA AES",
        "WPA2 AES",
    };
    pp2d_draw_textf(text_x, text_y, text_scale_x, text_scale_y, COLOR_BLACK, "Security: %s", encryption_type_string[this->encryption].c_str());

    text_y += 20;
    if(hide_password)
        pp2d_draw_text(text_x, text_y, text_scale_x, text_scale_y, COLOR_BLACK, "Password:\n(hidden)");
    else
        pp2d_draw_textf(text_x, text_y, text_scale_x, text_scale_y, COLOR_BLACK, "Password:\n%s", this->password.c_str());
}

slots_list::slots_list()
{
    scroll = 0;
    selected_slot = 0;
    selected_backup = 0;
    passwords_hidden = false;
}
slots_list::slots_list(std::string main_path)
{
    scroll = 0;
    selected_slot = 0;
    selected_backup = 0;
    passwords_hidden = false;

    for(int i = 0; i < CFG_WIFI_SLOTS; i++)
        slots[i] = wifi_slot(i);

    Handle dir_handle;
    Result res = FSUSER_OpenDirectory(&dir_handle, sdmcArchive, fsMakePath(PATH_ASCII, main_path.c_str()));
    if(R_FAILED(res)) return;

    u32 entries_count = 0;
    do {
        FS_DirectoryEntry dir_entry;
        res = FSDIR_Read(dir_handle, &entries_count, 1, &dir_entry);
        if(R_FAILED(res) || entries_count == 0)
            break;

        if((dir_entry.attributes & FS_ATTRIBUTE_DIRECTORY) || strcmp(dir_entry.shortExt, "BIN"))
            continue;

        char char_name[0x107] = {0};
        utf16_to_utf8((u8*)char_name, dir_entry.name, 0x106);

        char char_path[0x107] = {0};
        strcat(char_path, main_path.c_str());
        strcat(char_path, "/");
        strcat(char_path, char_name);

        std::string path = std::string(char_path);
        backups.push_back(wifi_slot(path));
    } while(entries_count);

    FSDIR_Close(dir_handle);
    std::sort(backups.begin(), backups.end());
}

static void draw_single_slot(int id, bool selected)
{
    buttons[BUTTON_SLOT_1 + id*3].selected = selected;
    buttons[BUTTON_SLOT_1 + id*3].draw();
    buttons[BUTTON_SLOT_1_SAVE + id*3].draw();
    buttons[BUTTON_SLOT_1_WRITE + id*3].draw();

    int text_x = 53+id*100;
    int text_y = 19;
    float text_scale_x = 0.8;
    float text_scale_y = 0.8;
    pp2d_draw_text(text_x, text_y, text_scale_x, text_scale_y, COLOR_BLACK, "Slot");
    text_x += ((pp2d_get_text_width("Slot", text_scale_x, text_scale_y) - 6) /2) - 2;
    text_y += pp2d_get_text_height("Slot", text_scale_x, text_scale_y);
    pp2d_draw_textf(text_x, text_y, text_scale_x, text_scale_y, COLOR_BLACK, "%i", id+1);
}

void slots_list::draw_top(void)
{
    pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
    for(int i = 0; i < CFG_WIFI_SLOTS; i++)
        draw_single_slot(i, (unsigned int)i == this->selected_slot);
}
void slots_list::draw_list(void)
{
    pp2d_draw_on(GFX_BOTTOM, GFX_LEFT);
    int x_offset = 15;
    int y_offset = 81;
    int buttons_width = 36;
    int bg_width = 320-x_offset*2-buttons_width-2;
    int bg_height = 240-y_offset-x_offset;

    Area background = Area(x_offset, y_offset, bg_width, bg_height);
    background.draw();

    buttons[BUTTON_ARROW_UP].draw();
    buttons[BUTTON_DELETE].draw();
    buttons[BUTTON_ARROW_DOWN].draw();

    const unsigned int names_per_screen = 7;
    if(this->backups.size() >= names_per_screen)
    {
        if(this->selected_backup == this->scroll-1 || this->backups.size()-this->scroll+1 < names_per_screen)
            this->scroll--;
        else if(this->backups.size()-this->scroll >= names_per_screen)
        {
            if(this->selected_backup-this->scroll >= names_per_screen)
                this->scroll++;
        }
    }

    int text_x = x_offset + 4;
    for(unsigned int i = this->scroll; i < this->scroll+names_per_screen; i++)
    {
        if(i > this->backups.size())
            break;

        int text_y = y_offset + 2 + 2 + (i-this->scroll) * 20;
        wifi_slot current_slot;
        if(i == 0)
        {
            current_slot.name = "New...";
        }
        else
        {
            current_slot = this->backups[i-1];
        }

        if(i == this->selected_backup)
            pp2d_draw_rectangle(x_offset+2, text_y-2, bg_width-4, pp2d_get_text_height(current_slot.name.c_str(), 0.6, 0.6)+2, COLOR_CURSOR);
        pp2d_draw_text(text_x, text_y, 0.6, 0.6, COLOR_BLACK, current_slot.name.c_str());
    }
}
void slots_list::draw_interface(void)
{
    this->slots[this->selected_slot].draw_info(false, this->passwords_hidden);
    if(this->selected_backup != 0)
        this->backups[this->selected_backup-1].draw_info(true, this->passwords_hidden);
    this->draw_top();
    this->draw_list();
}

void slots_list::next_slot_right(void)
{
    do {
        this->selected_slot++;
        this->selected_slot %= CFG_WIFI_SLOTS;
    } while(!this->slots[this->selected_slot].exists);
}
void slots_list::next_slot_left(void)
{
    do {
        if(this->selected_slot == 0)
            this->selected_slot = CFG_WIFI_SLOTS;
        this->selected_slot--;
    } while(!this->slots[this->selected_slot].exists);
}

void slots_list::next_backup_up(void)
{
    if(this->selected_backup > 0)
        this->selected_backup--;
}
void slots_list::next_backup_down(void)
{
    if(this->selected_backup < this->backups.size())
        this->selected_backup++;
}

void slots_list::select_slot(int id)
{
    if(this->slots[id].exists)
        this->selected_slot = id;
}

void slots_list::write_to(int id)
{
    if(this->selected_backup != 0)
        this->slots[id].copy_slot(this->backups[this->selected_backup-1]);
}
void slots_list::save_from(int id)
{
    if(this->slots[id].exists)
    {
        if(this->selected_backup == 0)
        {
            std::stringstream original_stream;
            std::time_t t = std::time(nullptr);
            std::tm tm = *std::localtime(&t);
            original_stream << this->slots[id].name << "_" << std::put_time(&tm, "%H-%M-%S_%e-%m-%Y");
            std::string original_string = original_stream.str();
            const char * original_text = original_string.c_str();
            char * file_name = get_input(0x100, original_text);
            if(file_name != NULL)
            {
                std::stringstream path_stream;
                path_stream << WORKING_DIR << "/" << file_name << ".bin";
                wifi_slot new_slot = wifi_slot(path_stream.str());
                if(!new_slot.exists) //prevent clicking too fast causing duplicates
                {
                    new_slot.copy_slot(this->slots[id]);
                    this->backups.push_back(new_slot);
                    std::sort(this->backups.begin(), this->backups.end());
                }
            }
            free(file_name);
        }
        else
            this->backups[this->selected_backup-1].copy_slot(this->slots[id]);
    }
}

void slots_list::write_to_selected(void)
{
    this->write_to(this->selected_slot);
}
void slots_list::save_from_selected(void)
{
    this->save_from(this->selected_slot);
}

void slots_list::delete_selected_backup(void)
{
    if(this->selected_backup != 0)
    {
        this->backups[this->selected_backup-1].delete_slot();
        this->backups.erase(this->backups.begin()+(--this->selected_backup));
    }
}

void slots_list::toggle_password_visibility(void)
{
    this->passwords_hidden = !this->passwords_hidden;
}

bool slots_list::selected_new_backup(void)
{
    return this->selected_backup == 0;
}
bool slots_list::slot_exists(int id)
{
    return this->slots[id].exists;
}
bool slots_list::can_go_down(void)
{
    return this->selected_backup < this->backups.size();
}
