#pragma once

#include "../module/planner.h"

#if ENABLED(LGT_LCD_TFT)
#define SPIFLASH_SIZE               0x400000    // 4MB
#define SPIFLASH_DATA_SIZE          0x1000      // 1KB
#define SPIFLASH_ADDR_DATA          (SPIFLASH_SIZE - SPIFLASH_DATA_SIZE)
#define SPIFLASH_ADDR_TOUCH         SPIFLASH_ADDR_DATA      
#define SPIFLASH_ADDR_RECOVERY      (SPIFLASH_ADDR_TOUCH + 16)
#define SPIFLASH_ADDR_SETTINGS      (SPIFLASH_ADDR_RECOVERY + 64)

#define TOUCH_VERSION       "V01"
#define SETTINGS_VERSION    "V02"   // change value when settings struct is changed

#ifndef LIST_ITEM_MAX
    #define LIST_ITEM_MAX        5
#endif

#define SETTINGS_MAX_LEN 22 // ** it must sync with settings struct

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
    bool listOrder;
    bool enabledRunout;
    bool enabledPowerloss;
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
    bool validate(const char *current, const char*stored);
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

    void saveTouch();
    bool loadTouch();

    // void saveRecovery();
    // bool loadRecovery();
};

extern LgtStore lgtStore;

#endif
