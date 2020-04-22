#pragma once

#include "../module/planner.h"

#if ENABLED(LGT_LCD_TFT)

#define FLASH_ADDR_SETTINGS 0x300800u
#define SETTINGS_VERSION    "V01"

#ifndef LIST_ITEM_MAX
#define LIST_ITEM_MAX        5
#endif
#define SETTINGS_MAX_LEN 21 // it must sync with settings struct
#if (SETTINGS_MAX_LEN % 5) == 0
	#define SETTINGS_MAX_PAGE (SETTINGS_MAX_LEN / 5)
#else
    #define SETTINGS_MAX_PAGE (SETTINGS_MAX_LEN / 5 + 1)
#endif

struct Settings
{
	float acceleration; // data start
	float max_xy_jerk;
	float max_z_jerk;
	float max_e_jerk;
	float max_feedrate[XYZE];
	float minimumfeedrate;
	float mintravelfeedrate;
	uint32_t max_acceleration_units_per_sq_second[XYZE];
	float retract_acceleration;
	float axis_steps_per_unit[XYZE];

    // start to store in spiflash
    char version[4];    // Vxx\0
    // uint16_t crc;       // checksum for data below
	bool enabledRunout;
	bool listOrder;
    // bool enabledPowerloss;
}; 

class LgtStore 
{
private:
    Settings m_settings;    // store temp setttings data

    uint8_t m_currentPage;     // current page
    uint8_t m_currentItem;     // select item index
    uint8_t  m_currentSetting;    // select index
    bool m_isSelectSetting;

    bool m_settingsModified;

private:
    float distanceMultiplier(uint8_t i);
    void  *settingPointer(uint8_t i);
    bool validate();
    void _reset();

public:
    LgtStore();

    inline void clear()
    {
        m_currentPage = 0;     // current page
        m_currentItem = 0;     // select item index
        m_currentSetting = 0;    // select index
        m_isSelectSetting = false;       
    }

    void applySettings();
    void syncSettings();
    void save();
    bool load();
    void reset();

    void settingString(uint8_t i, char* p);
    inline void settingString(char *p) { settingString(settingIndex(), p); }
    void changeSetting(uint8_t i, int8_t distance);

    inline void changeSetting(int8_t d) 
    { 
        if (!isSettingSelected()) return;
        changeSetting(settingIndex(), d); 
    }

    inline uint8_t page() { return m_currentPage;}

    inline uint8_t nextPage()
    {
        if (m_currentPage < SETTINGS_MAX_PAGE - 1) {
            m_currentPage++;
            return 1;
        } else {
            return 0;
        }

    }

    inline uint8_t previousPage()
    {
        if (m_currentPage > 0) {
            m_currentPage--;
            return 1;
        } else {
            return 0;
        }
    }

    inline uint8_t settingIndex() {return m_currentSetting;} // get current selected setting

    inline uint8_t item() {return m_currentItem;}

    inline bool isSettingSelected() {return m_isSelectSetting;}

    inline uint8_t selectedPage()
    {
        if (isSettingSelected())
            return m_currentSetting / LIST_ITEM_MAX;
        else
            return 0;
    }

    bool selectSetting(uint16_t item);

    inline bool isModified() { return m_settingsModified; }
    inline void setModified(bool b) { m_settingsModified = b; }

};

extern LgtStore lgtStore;

#endif
