#include "lgtstore.h"

#if ENABLED(LGT_LCD_TFT)
#include "w25qxx.h"
#include "lgtsdcard.h"
#include "lgttftdef.h"

// #include "../../src/libs/crc16.h"
#include "../feature/runout.h"
#include "../feature/powerloss.h"

#define WRITE_VAR(value)        do { FLASH_WRITE_VAR(addr, value); addr += sizeof(value); } while(0)
#define READ_VAR(value)         do { FLASH_READ_VAR(addr, value); addr += sizeof(value); } while(0)
// #define SAVE_SETTINGS()         FLASH_WRITE_VAR(FLASH_ADDR_SETTINGS, m_settings)
// #define LOAD_SETTINGS()         FLASH_READ_VAR(FLASH_ADDR_SETTINGS, m_settings)

LgtStore lgtStore;

// this coanst text arry must sync with settings struct and settingPointer function
static const char *txt_menu_setts[SETTINGS_MAX_LEN] = {
	TXT_MENU_SETTS_ACCL, //"Accel(mm/s^2):",
	TXT_MENU_SETTS_JERK_XY,//"Vxy-jerk(mm/s):",
	TXT_MENU_SETTS_JERK_Z,//"Vz-jerk(mm/s):",
	TXT_MENU_SETTS_JERK_E,//"Ve-jerk(mm/s):",
	TXT_MENU_SETTS_VMAX_X,//"Vmax x(mm/s):",
	TXT_MENU_SETTS_VMAX_Y,//"Vmax y(mm/s):",
	TXT_MENU_SETTS_VMAX_Z,//"Vmax z(mm/s):",
	TXT_MENU_SETTS_VMAX_E,//"Vmax e(mm/s):",
	TXT_MENU_SETTS_VMIN,//"Vmin(mm/s):",
	TXT_MENU_SETTS_VTRAVEL,//"Vtrav min(mm/s):",
	TXT_MENU_SETTS_AMAX_X,//"Amax x(mm/s^2):",
	TXT_MENU_SETTS_AMAX_Y,//"Amax y(mm/s^2):",	
	TXT_MENU_SETTS_AMAX_Z,//"Amax z(mm/s^2):",
	TXT_MENU_SETTS_AMAX_E,//"Amax e(mm/s^2):",
	TXT_MENU_SETTS_ARETRACT,//"A-retract(mm/s^2):",
	TXT_MENU_SETTS_STEP_X,//"X(steps/mm):",	
	TXT_MENU_SETTS_STEP_Y,//"Y(steps/mm):",	
	TXT_MENU_SETTS_STEP_Z,//"Z(steps/mm):",	
	TXT_MENU_SETTS_STEP_E,//"E(steps/mm):",
    TXT_MENU_SETTS_LIST_ORDER,//"File list order:"
	TXT_MENU_SETTS_CHECK_FILA,//"Filament check:",
    TXT_MENU_SETTS_RECOVERY
};

LgtStore::LgtStore()
{
    memset(reinterpret_cast<void *>(&m_settings), 0, sizeof(m_settings));
    clear();
}

void LgtStore::save()
{

    // set version string
    strcpy(m_settings.version, SETTINGS_VERSION);
    // SERIAL_ECHOLNPAIR("save version: ", m_settings.version);

    // calc crc
    // uint16_t crc = 0;
    // crc16(&crc, reinterpret_cast<void *>(&m_settings.acceleration), sizeof(m_settings) - 4 - 2);
    // SERIAL_ECHOLNPAIR("save crc: ", crc);
    // m_settings.crc = crc;
    
    // save some settings in spiflash
    uint32_t addr = FLASH_ADDR_SETTINGS;
    WRITE_VAR(m_settings.version);
    WRITE_VAR(m_settings.listOrder);
    WRITE_VAR(m_settings.enabledRunout);
    WRITE_VAR(m_settings.enabledPowerloss);
    SERIAL_ECHOPAIR("settings stored to spiflash(", addr - FLASH_ADDR_SETTINGS);
    SERIAL_ECHOLN(" bytes)");

    //  save other settings in internal flash
    queue.enqueue_now_P("M500"); 
    setModified(false);
}

/**
 * validate if settings is stored in spiflash
 */
bool LgtStore::validate()
{
    if (strcmp(m_settings.version, SETTINGS_VERSION) == 0)
        return true;
    return false;
}

/**
 * load settings from spiflash
 * spi flash -> settings struct -> memory(apply)
 * load sequence must be consistent with save
 */
bool LgtStore::load()
{
    uint32_t addr = FLASH_ADDR_SETTINGS;

    SERIAL_ECHOLN("-- load settings form spiflash start --");
    READ_VAR(m_settings.version);
    SERIAL_ECHOLNPAIR("stored version: ", m_settings.version);
    SERIAL_ECHOLNPAIR("current version: ", SETTINGS_VERSION);
    if (!validate()) {
       SERIAL_ECHOLN("load failed, reset settings");
       _reset();
       return false;    
    }

    READ_VAR(m_settings.listOrder);
    SERIAL_ECHOLNPAIR("listOrder: ", m_settings.listOrder);
    lgtCard.setListOrder(m_settings.listOrder);

    READ_VAR(m_settings.enabledRunout);
    SERIAL_ECHOLNPAIR("enabledRunout: ", m_settings.enabledRunout);
    runout.enabled = m_settings.enabledRunout;

    READ_VAR(m_settings.enabledPowerloss);
    SERIAL_ECHOLNPAIR("enabledPowerloss: ", m_settings.enabledPowerloss);
    recovery.enable(m_settings.enabledPowerloss); 
    SERIAL_ECHOLN("-- load settings form spiflash end --");

    return true;

    // LOAD_SETTINGS();

    // SERIAL_ECHOLNPAIR("load settings version: ", m_settings.version);
    // SERIAL_ECHOLNPAIR("load settings crc: ", m_settings.crc);
    // // calc crc
    // uint16_t crc = 0;
    // crc16(&crc, reinterpret_cast<void *>(&m_settings.acceleration), sizeof(m_settings) - 4 - 2);
    // SERIAL_ECHOLNPAIR("current crc: ", crc);   

    // LOOP_XYZE_N(i) {
    //     planner.settings.axis_steps_per_mm[i]          = m_settings.axis_steps_per_unit[i];
    //     planner.settings.max_feedrate_mm_s[i]          = m_settings.max_feedrate[i];
    //     planner.settings.max_acceleration_mm_per_s2[i] = m_settings.max_acceleration_units_per_sq_second[i];
    // }
    // planner.refresh_positioning();
    // planner.settings.acceleration = m_settings.acceleration;
    // planner.settings.retract_acceleration = m_settings.retract_acceleration;;
    // planner.settings.min_feedrate_mm_s =   m_settings.minimumfeedrate;
    // planner.settings.min_travel_feedrate_mm_s = m_settings.mintravelfeedrate;
    // planner.max_jerk[X_AXIS] = m_settings.max_xy_jerk;
    // planner.max_jerk[Y_AXIS] = m_settings.max_xy_jerk;
    // planner.max_jerk[Z_AXIS] =  m_settings.max_z_jerk;
    // #if DISABLED(JUNCTION_DEVIATION) || DISABLED(LIN_ADVANCE)
    //   planner.max_jerk[E_AXIS] =  m_settings.max_e_jerk;
    // #endif

    // runout.enabled = m_settings.enabledRunout;
    // lgtCard.setListOrder(m_settings.listOrder);
    // // recovery.enable(m_settings.enabledPowerloss); 
}

/**
 * reset lgttft variables
 */
void LgtStore::_reset()
{
    lgtCard.setListOrder(false);
    runout.enabled = true;
    recovery.enable(PLR_ENABLED_DEFAULT);
}

/**
 * reset all variables
 */
void LgtStore::reset()
{
    float tmp1[] = DEFAULT_AXIS_STEPS_PER_UNIT;
    float tmp2[] = DEFAULT_MAX_FEEDRATE;
    long tmp3[] = DEFAULT_MAX_ACCELERATION;

    LOOP_XYZE_N(i) {
        planner.settings.axis_steps_per_mm[i]          = tmp1[i];
        planner.settings.max_feedrate_mm_s[i]          = tmp2[i];
        planner.settings.max_acceleration_mm_per_s2[i] = tmp3[i];
    }
    planner.refresh_positioning();
    planner.settings.acceleration = DEFAULT_ACCELERATION;
    planner.settings.retract_acceleration = DEFAULT_RETRACT_ACCELERATION;;
    planner.settings.min_feedrate_mm_s =   DEFAULT_MINIMUMFEEDRATE;
    planner.settings.min_travel_feedrate_mm_s = DEFAULT_MINTRAVELFEEDRATE;
    planner.max_jerk[X_AXIS] = DEFAULT_XJERK;
    planner.max_jerk[Y_AXIS] = DEFAULT_YJERK;
    planner.max_jerk[Z_AXIS] = DEFAULT_ZJERK;
    #if DISABLED(JUNCTION_DEVIATION) || DISABLED(LIN_ADVANCE)
      planner.max_jerk[E_AXIS] =  DEFAULT_EJERK;
    #endif

    _reset();

// ///////////////////////////
//     LOOP_XYZE_N(i) {
//         m_settings.axis_steps_per_unit[i]=tmp1[i];  
//         m_settings.max_feedrate[i]=tmp2[i];  
//         m_settings.max_acceleration_units_per_sq_second[i]=tmp3[i];
//     }
//     planner.refresh_positioning();
// 	m_settings.acceleration=DEFAULT_ACCELERATION;
// 	m_settings.max_xy_jerk=DEFAULT_XJERK;     //DEFAULT_XJERK=DEFAULT_YJERK
//     m_settings.max_z_jerk=DEFAULT_ZJERK;
//     m_settings.max_e_jerk=DEFAULT_EJERK;
//     m_settings.minimumfeedrate=DEFAULT_MINIMUMFEEDRATE;
// 	m_settings.mintravelfeedrate=DEFAULT_MINTRAVELFEEDRATE;
// 	m_settings.retract_acceleration=DEFAULT_RETRACT_ACCELERATION;	
// 	m_settings.enabledRunout = true;
//     m_settings.listOrder =  false; // default: forward order
//     // m_settings.enabledPowerloss = PLR_ENABLED_DEFAULT;
}

/**
 * apply settings struct to variables
 */
void LgtStore::applySettings()
{
    LOOP_XYZE_N(i) {
        planner.settings.axis_steps_per_mm[i]          = m_settings.axis_steps_per_unit[i];
        planner.settings.max_feedrate_mm_s[i]          = m_settings.max_feedrate[i];
        planner.settings.max_acceleration_mm_per_s2[i] = m_settings.max_acceleration_units_per_sq_second[i];
    }
    planner.refresh_positioning();
    planner.settings.acceleration = m_settings.acceleration;
    planner.settings.retract_acceleration = m_settings.retract_acceleration;;
    planner.settings.min_feedrate_mm_s =   m_settings.minimumfeedrate;
    planner.settings.min_travel_feedrate_mm_s = m_settings.mintravelfeedrate;
    planner.max_jerk[X_AXIS] = m_settings.max_xy_jerk;
    planner.max_jerk[Y_AXIS] = m_settings.max_xy_jerk;
    planner.max_jerk[Z_AXIS] =  m_settings.max_z_jerk;
    #if DISABLED(JUNCTION_DEVIATION) || DISABLED(LIN_ADVANCE)
      planner.max_jerk[E_AXIS] =  m_settings.max_e_jerk;
    #endif

    lgtCard.setListOrder(m_settings.listOrder);
    runout.enabled = m_settings.enabledRunout;
    recovery.enable(m_settings.enabledPowerloss);
}

/**
 * sync variables to settings struct
 */
void LgtStore::syncSettings()
{
    LOOP_XYZE_N(i) {
       m_settings.axis_steps_per_unit[i] = planner.settings.axis_steps_per_mm[i];
        m_settings.max_feedrate[i] = planner.settings.max_feedrate_mm_s[i];
       m_settings.max_acceleration_units_per_sq_second[i] = planner.settings.max_acceleration_mm_per_s2[i];
    }
    // planner.refresh_positioning();
    m_settings.acceleration = planner.settings.acceleration;
    m_settings.retract_acceleration = planner.settings.retract_acceleration;
    m_settings.minimumfeedrate = planner.settings.min_feedrate_mm_s;
    m_settings.mintravelfeedrate = planner.settings.min_travel_feedrate_mm_s;
    m_settings.max_xy_jerk = planner.max_jerk[X_AXIS];
    m_settings.max_xy_jerk = planner.max_jerk[Y_AXIS];
    m_settings.max_z_jerk = planner.max_jerk[Z_AXIS];
    #if DISABLED(JUNCTION_DEVIATION) || DISABLED(LIN_ADVANCE)
      m_settings.max_e_jerk = planner.max_jerk[E_AXIS];
    #endif
    m_settings.listOrder = lgtCard.isReverseList();
    m_settings.enabledRunout = runout.enabled;
    m_settings.enabledPowerloss = recovery.enabled;
}

void LgtStore::settingString(uint8_t i, char* str)
{
    char p[10] = {0};
	if (i >= SETTINGS_MAX_LEN) { /* error index */
		return;
	} else if (i >= 20) {  	    /* bool type */				
        #ifndef Chinese
            const char * format = "%8s";
        #else
            const char * format = "%5s";
        #endif
        if(*reinterpret_cast<bool *>(settingPointer(i)))
            sprintf(p, format, TXT_MENU_SETTS_VALUE_ON);
        else
            sprintf(p, format, TXT_MENU_SETTS_VALUE_OFF);
	} else if (i == 19) {       // bool type
        #ifndef Chinese
            const char * format = "%8s";
        #else
            const char * format = "%5s";
        #endif
        if(*reinterpret_cast<bool *>(settingPointer(i)))
            sprintf(p, format, TXT_MENU_SETTS_VALUE_INVERSE);
        else
            sprintf(p, format, TXT_MENU_SETTS_VALUE_FORWARD);
    } else if (i >= 10 && i<= 13) { /* uint32 type */		
		#ifndef Chinese 				
			sprintf(p,"%8lu", *reinterpret_cast<uint32_t *>(settingPointer(i)));			
		#else
			sprintf(p,"%6lu",  *reinterpret_cast<uint32_t *>(settingPointer(i)));
        #endif
	} else { /* float type */
        sprintf(p,"%8.2f", *reinterpret_cast<float *>(settingPointer(i)));
	}

    sprintf(str, "%-20s%s", txt_menu_setts[i], p);

}

void *LgtStore::settingPointer(uint8_t i)
{
	switch(i)
	{
        // float type
		case 0:
			return &m_settings.acceleration;	
		case 1:
			return &m_settings.max_xy_jerk;		
		case 2:
			return &m_settings.max_z_jerk;			
		case 3:
			return &m_settings.max_e_jerk;		
		case 4:
			return &m_settings.max_feedrate[0];			
		case 5: 
			return &m_settings.max_feedrate[1];
		case 6:
			return &m_settings.max_feedrate[2];			
		case 7:
			return &m_settings.max_feedrate[3];
		case 8: 
			return &m_settings.minimumfeedrate;
		case 9:
			return &m_settings.mintravelfeedrate;
        // uint32 type
		case 10:
			return &m_settings.max_acceleration_units_per_sq_second[0];
		case 11: 
			return &m_settings.max_acceleration_units_per_sq_second[1];
		case 12:
			return &m_settings.max_acceleration_units_per_sq_second[2];
		case 13:
			return &m_settings.max_acceleration_units_per_sq_second[3];
        // float type
		case 14: 
			return &m_settings.retract_acceleration;
		case 15:
			return &m_settings.axis_steps_per_unit[0];
		case 16:
			return &m_settings.axis_steps_per_unit[1];
		case 17: 
			return &m_settings.axis_steps_per_unit[2];
		case 18:
			return &m_settings.axis_steps_per_unit[3];
        // bool type
        case 19:
            return &m_settings.listOrder;        
		case 20:
			return &m_settings.enabledRunout;
        case 21:
            return &m_settings.enabledPowerloss;
		default: return 0;
	}
}

float LgtStore::distanceMultiplier(uint8_t i)
{
	switch(i){
		default:
			return 0.0; 
		case 2: case 15: case 16: case 17: case 18:	
			return 0.1;
		case 1:	case 3:case 4: case 5: case 6: case 7:
		case 8: case 9: case 12:
			return 1.0;
		case 0: case 10: case 11: case 13: case 14:
			return 100.0;
	}
}

void LgtStore::changeSetting(uint8_t i, int8_t distance)
{
	if(i >= SETTINGS_MAX_LEN){			/* error index */
		return;
	}
	else if(i >= 19)  	/* bool type */
	{				
		*(bool *)settingPointer(i) = !(*(bool *)settingPointer(i));
	}
	else if(i >= 10 && i<= 13)  /* unsigned long type */
	{		
		*(unsigned long *)settingPointer(i) = *(unsigned long *)settingPointer(i) +
			distance * (unsigned long)distanceMultiplier(i);
		if((long)*(unsigned long *)settingPointer(i) < 0)
            *(unsigned long *)settingPointer(i) = 0;	//minimum value
	} 
    else /* float type */
	{								
		*(float *)settingPointer(i) = *(float *)settingPointer(i) + distance * distanceMultiplier(i);
		if(*(float *)settingPointer(i) < 0.0) 
            *(float *)settingPointer(i) = 0.0;	//minimum value
	}
    setModified(true);
	// ConfigSettings.calcAccelRates();
}

 bool LgtStore::selectSetting(uint16_t item)
 {
    if (item < LIST_ITEM_MAX) {
        uint16_t n = m_currentPage * LIST_ITEM_MAX + item;
        if (n < SETTINGS_MAX_LEN) {
            m_currentItem = item;
            m_currentSetting = n;
            m_isSelectSetting = true;
            return false;   // success to set
        }
    }
    return true;   // fail to set
 }


#endif