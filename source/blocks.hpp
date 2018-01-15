#pragma once

#include "basic.hpp"

#define CFG_WIFI_BLKID (u32 )0x00080000
#define CFG_WIFI_SLOTS 3
#define CFG_WIFI_SLOT_SIZE (u32 )0xC00

typedef enum {
    WIFI_ENCRYPTION_OPEN = 0,
    WIFI_ENCRYPTION_WEP_64,
    WIFI_ENCRYPTION_WEP_128,
    WIFI_ENCRYPTION_WEP_152,
    WIFI_ENCRYPTION_WPA_PSK_TKIP,
    WIFI_ENCRYPTION_WPA_PSK_AES,
    WIFI_ENCRYPTION_WPA2_PSK_TKIP,
    WIFI_ENCRYPTION_WPA2_PSK_AES,

    WIFI_ENCRYPTION_AMOUNT,
} wifi_encryption_type;

typedef enum {
    WIFI_AOSS = 0,
    WIFI_WPS,
} wifi_multissid_type;

typedef struct {
    char SSID[0x20];
    u8 SSID_length;
    u8 encryption_type;
    u8 __padding1[2];
    char password_text[0x40];
    u8 password_key[0x20];
    u8 __padding2[0x20];
} __attribute__((packed)) sub_network_s;

typedef struct {
    u16 version;
    u16 crc_checksum;

    u8 network_enable;

    u8 editable_security;
    u8 __padding1[2];
    sub_network_s main_network;

    u8 multissid_enable;
    u8 multissid_type;
    u8 multissid_amount;
    u8 __padding2[1];
    sub_network_s multissid_networks[4];

    u8 enable_DHCP;
    u8 enable_autoDNS;
    u8 __padding3[2];

    u8 ip_address[4];
    u8 gateway_address[4];
    u8 subnet_mask[4];

    u8 dns_primary[4];
    u8 dns_secondary[4];

    u8 connection_test_success;
    u8 group_privacy_mode; //needs more info
    u8 __padding4[2];
    u8 ip_to_use[4];
    u8 AP_MAC_address[6];
    u8 channel;
    u8 __padding5[1];

    u8 enable_proxy;
    u8 proxy_auth;

    u16 port_number;
    char proxy_url[0x30];
    u8 __padding6[0x34];
    char proxy_username[0x20];
    char proxy_password[0x20];

    u16 UPnP_value;
    u16 MTU_value;

    u8 __padding7[0x7EC];
} __attribute__((packed)) wifi_slot_s;

class wifi_slot
{
public:
    wifi_slot();
    wifi_slot(int id);
    wifi_slot(std::string path);

    bool exists;

    std::string name;
    std::string password;

    wifi_encryption_type encryption;

    bool editable_security;
    wifi_multissid_type type;

    Result delete_slot(void);
    void copy_slot(wifi_slot other_slot);
    wifi_slot_s get_data(void);
    void draw_info(bool to_the_right, bool hide_password);

    bool operator<(const wifi_slot &slot) const { return path.compare(slot.path) < 0; }; //for sorting

private:
    void fix_slot_crc(void);

    Result read_slot(void);
    Result write_slot(void);

    std::string path;
    int id;

    wifi_slot_s slot_data;
};

class slots_list
{
public:
    slots_list();
    slots_list(std::string main_path);
    void draw_interface(void);

    void next_slot_right(void);
    void next_slot_left(void);
    void next_backup_up(void);
    void next_backup_down(void);

    void select_slot(int id);
    void select_backup(int id);

    void write_to(int id);
    void write_to_selected(void);
    void save_from(int id);
    void save_from_selected(void);

    void delete_selected_backup(void);

    void toggle_password_visibility(void);

    bool slot_exists(int id);
    bool selected_new_backup(void);
    bool can_go_down(void);

private:
    wifi_slot slots[CFG_WIFI_SLOTS];
    std::vector<wifi_slot> backups;

    void draw_top(void);
    void draw_list(void);

    bool passwords_hidden;
    unsigned int selected_slot;
    unsigned int selected_backup;
    unsigned int scroll;
};

extern slots_list list;
