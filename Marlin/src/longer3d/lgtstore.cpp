#include "lgtstore.h"

#if ENABLED(LGT_LCD_TFT)
#include "w25qxx.h"
#include "lgtsdcard.h"
#include "../feature/runout.h"
// #include "../feature/powerloss.h"
#include "../../src/libs/crc16.h"
#include "lgttftdef.h"

#define FLASH_WRITE_VAR(addr, value)  spiFlash.W25QXX_Write(reinterpret_cast<uint8_t *>(&value), uint32_t(addr), sizeof(value))
#define FLASH_READ_VAR(addr, value)   spiFlash.W25QXX_Read(reinterpret_cast<uint8_t *>(&value), uint32_t(addr), sizeof(value))
#define FLASH_ADDR_SETTINGS 0x300800u
#define SAVE_SETTINGS() FLASH_WRITE_VAR(FLASH_ADDR_SETTINGS, m_settings)
#define LOAD_SETTINGS() FLASH_READ_VAR(FLASH_ADDR_SETTINGS, m_settings)

LgtStore lgtStore;

LgtStore::LgtStore()
{
    memset(reinterpret_cast<void *>(&m_settings), 0, sizeof(m_settings));
}

void LgtStore::save()
{
    // set version string
    strcpy(m_settings.version, SETTINGS_VERSION);
    SERIAL_ECHOLNPAIR("store version: ", m_settings.version);
    // calc crc
    uint16_t crc = 0;
    crc16(&crc, reinterpret_cast<void *>(&m_settings.acceleration), sizeof(m_settings) - 4 - 2);
    SERIAL_ECHOLNPAIR("store crc: ", crc);
    m_settings.crc = crc;
    SAVE_SETTINGS();    // save setttings struct into spi flash
}

/**
 * spi flash -> settings struct -> memory(apply)
 */
void LgtStore::load()
{
    LOAD_SETTINGS();
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

    runout.enabled = m_settings.enabledRunout;
    lgtCard.setListOrder(m_settings.listOrder);
    // recovery.enable(m_settings.enabledPowerloss);
}

/**
 * reset variables
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

    runout.enabled = true;
    lgtCard.setListOrder(false);
    // recovery.enable(PLR_ENABLED_DEFAULT);
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

    runout.enabled = m_settings.enabledRunout;
    lgtCard.setListOrder(m_settings.listOrder);
    // recovery.enable(m_settings.enabledPowerloss);
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
    m_settings.enabledRunout = runout.enabled;
    m_settings.listOrder = lgtCard.isReverseList();
    // m_settings.enabledPowerloss = recovery.enabled;
}

void LgtStore::settingString(uint8_t i, char* p)
{
	if (i >= SETTINGS_MAX_LEN) {			/* error index */
		return;
	} else if (i >= 19) {  	/* bool type */				
        #ifndef Chinese
            if(*reinterpret_cast<bool *>(settingPointer(i)))
                sprintf(p,"%8s", TXT_MENU_SETTS_VALUE_ON);
            else
                sprintf(p,"%8s", TXT_MENU_SETTS_VALUE_OFF);
        #else
            if(*reinterpret_cast<bool *>(settingPointer(i)))
                sprintf(p,"%5s", TXT_MENU_SETTS_VALUE_INVERSE);
            else
                sprintf(p,"%5s", TXT_MENU_SETTS_VALUE_FORWARD);
        #endif
	} else if (i >= 10 && i<= 13) { /* uint32 type */		
		#ifndef Chinese 				
			sprintf(p,"%8lu", *reinterpret_cast<uint32_t *>(settingPointer(i)));			
		#else
			sprintf(p,"%6lu",  *reinterpret_cast<uint32_t *>(settingPointer(i)));
        #endif
	} else { /* float type */
        sprintf(p,"%8.2f", *reinterpret_cast<float *>(settingPointer(i)));
	}
}

void *LgtStore::settingPointer(uint8_t i)
{
	switch(i)
	{
		default: return 0;
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
		case 10:
			return &m_settings.max_acceleration_units_per_sq_second[0];
		case 11: 
			return &m_settings.max_acceleration_units_per_sq_second[1];
		case 12:
			return &m_settings.max_acceleration_units_per_sq_second[2];
		case 13:
			return &m_settings.max_acceleration_units_per_sq_second[3];
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
		case 19:
			return &m_settings.enabledRunout;
        case 20:
            return &m_settings.listOrder;
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

void LgtStore::changeSetting(uint8_t i, int8_t inc_sign, uint8_t distance)
{
	if(i >= SETTINGS_MAX_LEN){			/* error index */
		return;
	}
	else if(i >= 19)  	/* bool type */
	{				
		*(bool *)settingPointer(i) = !(*(bool *)settingPointer(i));
		return;
	}
	else if(i >= 10 && i<= 13)  /* unsigned long type */
	{		
		*(unsigned long *)settingPointer(i) = *(unsigned long *)settingPointer(i) +
			inc_sign * distance * (unsigned long)distanceMultiplier(i);
		if((long)*(unsigned long *)settingPointer(i) < 0) *(unsigned long *)settingPointer(i) = 0;	//minimum value
	}
	else if(i >= 15 && i<= 18) /* float type */
	{		
		*(float *)settingPointer(i) = *(float *)settingPointer(i) + inc_sign * distance * distanceMultiplier(i);
		if(*(float *)settingPointer(i) < 0.0) *(float *)settingPointer(i) = 0.0;		//minimum value
	}
	else /* float type */
	{								
		*(float *)settingPointer(i) = *(float *)settingPointer(i) + inc_sign * distance * distanceMultiplier(i);
		if(*(float *)settingPointer(i) < 0.0) *(float *)settingPointer(i) = 0.0;	//minimum value
		return;
	}
	// ConfigSettings.calcAccelRates();
}

#endif