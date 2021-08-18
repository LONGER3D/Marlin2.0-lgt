#pragma once

#if ENABLED(LGT_LCD_DW)

#define RECOVER_E_ADD 4//8
#define LOAD_FILA_LEN 500
#define UNLOAD_FILA_LEN -500
#define STARTUP_COUNTER 3000
#define LED_RED 4
#define LED_GREEN 5
#define LED_BLUE 6
#define DATA_SIZE 37    //the size of ScreenData and Receive_Cmd
#define FILE_LIST_NUM  25

#define EEPROM_INDEX 4000

//Printer kill reason
#define E_TEMP_ERROR		"Error 0: abnormal E temp"   //Heating failed
#define B_TEMP_ERROR		"Error 1: abnormal B temp"   //Heating failed
#define M112_ERROR			"Error 2: emergency stop"
#define SDCARD_ERROR		"Error 3: SD card error"
#define HOME_FAILE			"Error 4: homing failed"
#define TIMEOUT_ERROR		"Error 5: timeout error"
#define EXTRUDER_NUM_ERROR  "Error 6: E number error"
#define DRIVER_ERROR		"Error 7: driver error"
#define E_MINTEMP_ERROR     "Error 8: E mintemp triggered"
#define B_MINTEMP_ERROR     "Error 9: B mintemp triggered"
#define E_MAXTEMP_ERROR     "Error 10: E maxtemp triggered"
#define B_MAXTEMP_ERROR     "Error 11: B maxtemp triggered"
#define E_RUNAWAY_ERROR     "Error 12: E thermal runaway"    // Heated, then temperature fell too far
#define B_RUNAWAY_ERROR     "Error 13: B thermal runaway"

// new killed error message
#define TXT_ERR_MINTEMP					"E1 MINTEMP"		
#define TXT_ERR_MIN_TEMP_BED 			"Bed MINTEMP"
#define TXT_ERR_MAXTEMP					"E1 MAXTEMP"
#define TXT_ERR_MAX_TEMP_BED			"Bed MAXTEMP"
#define TXT_ERR_HEATING_FAILED			"E1 Heating Failed"
#define TXT_ERR_HEATING_FAILED_BED		"Bed Heating Failed"
#define TXT_ERR_TEMP_RUNAWAY			"E1 Thermal Runaway"
#define TXT_ERR_TEMP_RUNAWAY_BED		"Bed Thermal Runaway"
#define TXT_ERR_HOMING_FAILED			"Homing Failed"
#define TXT_ERR_PROBING_FAILED			"Probing Failed"

// DWIN serial transfer protocol
#define DW_FH_0 		  0x5A
#define DW_FH_1 		  0xA5
#define DW_CMD_VAR_W 	  0x82
#define DW_CMD_VAR_R      0x83
#define JX_CMD_REG_W      0x80
#define JX_CMD_REG_R	  0x81
#define JX_ADDR_REG_PAGE  0x03

#define LEN_FILE_NAME 32
#define LEN_WORD 2
#define LEN_DWORD 4
#define LEN_4_CHR 4
#define LEN_6_CHR 6

#define TEMP_RANGE 2
#define PLA_E_TEMP PREHEAT_1_TEMP_HOTEND  //200
#define PLA_B_TEMP PREHEAT_1_TEMP_BED     //60
#define ABS_E_TEMP PREHEAT_2_TEMP_HOTEND  //230
#define ABS_B_TEMP PREHEAT_2_TEMP_BED     //80

#define	MAC_LENGTH		X_BED_SIZE
#define	MAC_WIDTH	    Y_BED_SIZE
#define	MAC_HEIGHT		Z_MAX_POS

#ifdef LK1_PRO
	#define MAC_MODEL       "LK1 Pro"
	#define MAC_SIZE		"300*300*400(mm)"
	//#define FILAMENT_RUNOUT_MOVE "G1 X10 Y260 F3000"
	#define FILAMENT_RUNOUT_MOVE_X 10
	#define FILAMENT_RUNOUT_MOVE_Y 260
	#define FILAMENT_RUNOUT_MOVE_F 50
#elif defined(LK5_PRO)
	#define MAC_MODEL       "LK5 Pro"
	#define MAC_SIZE		"300*300*400(mm)"
	//#define FILAMENT_RUNOUT_MOVE "G1 X10 Y260 F3000"
	#define FILAMENT_RUNOUT_MOVE_X 10
	#define FILAMENT_RUNOUT_MOVE_Y 260
	#define FILAMENT_RUNOUT_MOVE_F 50
#else // LK4 PRO
	#define MAC_MODEL       "LK4 Pro"
	#define MAC_SIZE "220*220*250(mm)"
	//#define FILAMENT_RUNOUT_MOVE "G1 X10 Y200 F3000"
	#define FILAMENT_RUNOUT_MOVE_X 10
	#define FILAMENT_RUNOUT_MOVE_Y 200
	#define FILAMENT_RUNOUT_MOVE_F 50
#endif // LK1_Pro

#if defined(MANUAL_FEEDRATE)
	#undef MANUAL_FEEDRATE
#endif
#define MANUAL_FEEDRATE { 50*60, 50*60, 4*60, 60 } // Feedrates for manual moves along X, Y, Z, E from panel

#define FW_TEST_TAG "T001"
#ifndef FW_TEST_TAG
	#define FW_TEST_TAG ""
#endif
#define	BOARD_FW_VER    "0.3.5" FW_TEST_TAG "-Marlin" 


// DWIN system variable address
#define DW_ADDR_CHANGE_PAGE 0x0084
#define DW_PAGE_VAR_BASE 0x5A010000UL 

// user defined variable address
#define ADDR_USER_VAR_BASE                  (0x1000)
#define ADDR_VAL_MENU_TYPE                   ADDR_USER_VAR_BASE                             // 1000
#define ADDR_VAL_BUTTON_KEY                 (ADDR_VAL_MENU_TYPE + LEN_WORD)                 // 1002
#define ADDR_VAL_LOADING			        (ADDR_VAL_BUTTON_KEY + LEN_WORD)                // 1004
#define ADDR_VAL_LAUNCH_LOGO			    (ADDR_VAL_LOADING + LEN_WORD)					// 1006
#define ADDR_TXT_ABOUT_MAC_TIME				(ADDR_VAL_LAUNCH_LOGO+LEN_WORD)                 //1008
// HOME
#define ADDR_VAL_CUR_E                      (0x1010)                                        // 1010
#define ADDR_VAL_TAR_E                      (ADDR_VAL_CUR_E + LEN_WORD)                     // 1012
#define ADDR_VAL_CUR_B                      (ADDR_VAL_TAR_E + LEN_WORD)                     // 1014
#define ADDR_VAL_TAR_B                      (ADDR_VAL_CUR_B + LEN_WORD)                     // 1016
#define ADDR_VAL_ICON_HIDE                  (ADDR_VAL_TAR_B+LEN_WORD)                       //1018
// TUNE
#define ADDR_VAL_FAN                        (0x1030)                                        // 1030
#define ADDR_VAL_FEED                       (ADDR_VAL_FAN + LEN_WORD)                       // 1032    
#define ADDR_VAL_FLOW                       (ADDR_VAL_FEED + LEN_WORD)                      // 1034
#define ADDR_VAL_LEDS_SWITCH                (ADDR_VAL_FLOW + LEN_WORD)                      // 1036
#define ADDR_VAL_CUR_FEED                   (ADDR_VAL_LEDS_SWITCH+LEN_WORD)                  //1038

// LEVELING
#define ADDR_VAL_LEVEL_Z_UP_DOWN             (ADDR_VAL_CUR_FEED+LEN_WORD)                   //103A

// MOVE
#define ADDR_VAL_MOVE_POS_X                 (0x1050)                                        // 1050                  
#define ADDR_VAL_MOVE_POS_Y                 (ADDR_VAL_MOVE_POS_X + LEN_WORD)                // 1052
#define ADDR_VAL_MOVE_POS_Z                 (ADDR_VAL_MOVE_POS_Y + LEN_WORD)                // 1054
#define ADDR_VAL_MOVE_POS_E                 (ADDR_VAL_MOVE_POS_Z + LEN_WORD)                // 1056
// PRINT
	//... FILE
#define ADDR_TXT_PRINT_FILE_ITEM_0      (0x1070)                                        // 1070 
#define ADDR_TXT_PRINT_FILE_ITEM_1      (ADDR_TXT_PRINT_FILE_ITEM_0 + LEN_FILE_NAME)    // 1090
#define ADDR_TXT_PRINT_FILE_ITEM_2      (ADDR_TXT_PRINT_FILE_ITEM_1 + LEN_FILE_NAME)    // 10B0
#define ADDR_TXT_PRINT_FILE_ITEM_3      (ADDR_TXT_PRINT_FILE_ITEM_2 + LEN_FILE_NAME)    // 10D0
#define ADDR_TXT_PRINT_FILE_ITEM_4      (ADDR_TXT_PRINT_FILE_ITEM_3 + LEN_FILE_NAME)    // 10F0

#define ADDR_TXT_PRINT_FILE_ITEM_5      (ADDR_TXT_PRINT_FILE_ITEM_4 + LEN_FILE_NAME)    // 1110 
#define ADDR_TXT_PRINT_FILE_ITEM_6      (ADDR_TXT_PRINT_FILE_ITEM_5 + LEN_FILE_NAME)    // 1130
#define ADDR_TXT_PRINT_FILE_ITEM_7      (ADDR_TXT_PRINT_FILE_ITEM_6 + LEN_FILE_NAME)    // 1150
#define ADDR_TXT_PRINT_FILE_ITEM_8      (ADDR_TXT_PRINT_FILE_ITEM_7 + LEN_FILE_NAME)    // 1170
#define ADDR_TXT_PRINT_FILE_ITEM_9      (ADDR_TXT_PRINT_FILE_ITEM_8 + LEN_FILE_NAME)    // 1190

#define ADDR_TXT_PRINT_FILE_ITEM_10     (ADDR_TXT_PRINT_FILE_ITEM_9 + LEN_FILE_NAME)    // 11B0
#define ADDR_TXT_PRINT_FILE_ITEM_11     (ADDR_TXT_PRINT_FILE_ITEM_10 + LEN_FILE_NAME)   // 11D0
#define ADDR_TXT_PRINT_FILE_ITEM_12     (ADDR_TXT_PRINT_FILE_ITEM_11 + LEN_FILE_NAME)   // 11F0
#define ADDR_TXT_PRINT_FILE_ITEM_13     (ADDR_TXT_PRINT_FILE_ITEM_12 + LEN_FILE_NAME)   // 1210
#define ADDR_TXT_PRINT_FILE_ITEM_14     (ADDR_TXT_PRINT_FILE_ITEM_13 + LEN_FILE_NAME)   // 1230

#define ADDR_TXT_PRINT_FILE_ITEM_15     (ADDR_TXT_PRINT_FILE_ITEM_14 + LEN_FILE_NAME)   // 1250
#define ADDR_TXT_PRINT_FILE_ITEM_16     (ADDR_TXT_PRINT_FILE_ITEM_15 + LEN_FILE_NAME)   // 1270
#define ADDR_TXT_PRINT_FILE_ITEM_17     (ADDR_TXT_PRINT_FILE_ITEM_16 + LEN_FILE_NAME)   // 1290
#define ADDR_TXT_PRINT_FILE_ITEM_18     (ADDR_TXT_PRINT_FILE_ITEM_17 + LEN_FILE_NAME)   // 12B0
#define ADDR_TXT_PRINT_FILE_ITEM_19     (ADDR_TXT_PRINT_FILE_ITEM_18 + LEN_FILE_NAME)   // 12D0

#define ADDR_TXT_PRINT_FILE_ITEM_20     (ADDR_TXT_PRINT_FILE_ITEM_19 + LEN_FILE_NAME)   // 12F0
#define ADDR_TXT_PRINT_FILE_ITEM_21     (ADDR_TXT_PRINT_FILE_ITEM_20 + LEN_FILE_NAME)   // 1310
#define ADDR_TXT_PRINT_FILE_ITEM_22     (ADDR_TXT_PRINT_FILE_ITEM_21 + LEN_FILE_NAME)   // 1330
#define ADDR_TXT_PRINT_FILE_ITEM_23     (ADDR_TXT_PRINT_FILE_ITEM_22 + LEN_FILE_NAME)   // 1350
#define ADDR_TXT_PRINT_FILE_ITEM_24     (ADDR_TXT_PRINT_FILE_ITEM_23 + LEN_FILE_NAME)   // 1370


// PRINT home
#define ADDR_TXT_HOME_FILE_NAME             (0x1400)                                        // 1400
#define ADDR_TXT_HOME_ELAP_TIME             (ADDR_TXT_HOME_FILE_NAME + LEN_FILE_NAME)       // 1420
#define ADDR_VAL_HOME_PROGRESS              (ADDR_TXT_HOME_ELAP_TIME + LEN_6_CHR)           // 1426
#define ADDR_VAL_HOME_Z_HEIGHT              (ADDR_VAL_HOME_PROGRESS + LEN_WORD)             // 1428

// UTILITIES
	// filament
#define ADDR_VAL_UTILI_FILA_CHANGE_LEN  (0x1440)                                        // 1440
#define ADDR_VAL_FILA_CHANGE_TEMP       (ADDR_VAL_UTILI_FILA_CHANGE_LEN + LEN_WORD)     // 1442
// DIALOG NO TEMP    
#define ADDR_VAL_EXTRUDE_TEMP               (0x1460)                                        // 1460    
// ABOUT
#define ADDR_TXT_ABOUT_MODEL                (0x1480)                                        // 1480
#define ADDR_TXT_ABOUT_SIZE                 (ADDR_TXT_ABOUT_MODEL + LEN_FILE_NAME)          // 14A0
#define ADDR_TXT_ABOUT_FW_SCREEN            (ADDR_TXT_ABOUT_SIZE + LEN_FILE_NAME)           // 14C0
#define ADDR_TXT_ABOUT_FW_BOARD             (ADDR_TXT_ABOUT_FW_SCREEN + LEN_FILE_NAME)      // 14E0
#define ADDR_TXT_ABOUT_WORK_TIME_MAC         (ADDR_TXT_ABOUT_FW_BOARD+LEN_FILE_NAME)        //1500
// FILE SELECT
#define ADDR_VAL_PRINT_FILE_SELECT          (0x1550)                                        // 1550
#define ADDR_TXT_PRINT_FILE_SELECT          (ADDR_VAL_PRINT_FILE_SELECT + LEN_WORD)         // 1552  

#define ADDR_KILL_REASON                     (0x2000) 

// SP definition
#define SP_TXT_PRINT_FILE_ITEM_0            (0x6000)                                        // 6000
#define SP_COLOR_TXT_PRINT_FILE_ITEM_0		(SP_TXT_PRINT_FILE_ITEM_0 + 3)					// 6003
// ...
#define SP_TXT_PRINT_FILE_ITEM_24           (0x6300)                                        // 6300
#define SP_COLOR_TXT_PRINT_FILE_ITEM_24		(SP_TXT_PRINT_FILE_ITEM_24 + 3)					// 6303

// color
#define COLOR_LIGHT_RED                     (0xA001)
#define COLOR_WHITE							(0xFFFF)
// color_change(SP_TXT_PRINT_FILE_ITEM_0 + i*LEN_FILE_NAME, COLOR_LIGHT_RED)  


enum E_BUTTON_KEY {

	eBT_MOVE_XY_HOME,           //0  0000
	eBT_MOVE_Z_HOME,
	eBT_MOVE_X_PLUS_0,
	eBT_MOVE_X_MINUS_0,
	eBT_MOVE_Y_PLUS_0,
	eBT_MOVE_Y_MINUS_0,         //5  0005
	eBT_MOVE_Z_PLUS_0,
	eBT_MOVE_Z_MINUS_0,
	eBT_MOVE_E_PLUS_0,
	eBT_MOVE_E_MINUS_0,

	eBT_MOVE_X_PLUS_1,          // 10  000A 
	eBT_MOVE_X_MINUS_1,
	eBT_MOVE_Y_PLUS_1,
	eBT_MOVE_Y_MINUS_1,
	eBT_MOVE_Z_PLUS_1,
	eBT_MOVE_Z_MINUS_1,         //15  000F
	eBT_MOVE_E_PLUS_1,
	eBT_MOVE_E_MINUS_1,

	eBT_MOVE_X_PLUS_2,
	eBT_MOVE_X_MINUS_2,
	eBT_MOVE_Y_PLUS_2,          //20 0014
	eBT_MOVE_Y_MINUS_2,
	eBT_MOVE_Z_PLUS_2,
	eBT_MOVE_Z_MINUS_2,
	eBT_MOVE_E_PLUS_2,
	eBT_MOVE_E_MINUS_2,         //25 0019
	eBT_MOVE_DISABLE,
	eBT_MOVE_ENABLE,
	eBT_PRINT_FILE_OPEN,
	eBT_PRINT_FILE_OPEN_YES,

	eBT_PRINT_HOME_PAUSE,       //30  001E
	eBT_PRINT_HOME_RESUME,
	eBT_PRINT_HOME_ABORT,
	eBT_PRINT_HOME_FINISH,

	eBT_UTILI_FILA_PLA,
	eBT_UTILI_FILA_ABS,         //35  0023
	eBT_UTILI_FILA_LOAD,
	eBT_UTILI_FILA_UNLOAD,

	eBT_HOME_RECOVERY_YES,
	eBT_HOME_RECOVERY_NO,
	eBT_DIAL_FILA_NO_TEMP_RET,    //40 0028
	eBT_DIAL_MOVE_NO_TEMP_RET,
	eBT_PRINT_FILE_CLEAN,

	eBT_UTILI_LEVEL_MEASU_START,  // == PREVIOUS
	eBT_UTILI_LEVEL_CORNER_POS_1,
	eBT_UTILI_LEVEL_CORNER_POS_2, //45 002D
	eBT_UTILI_LEVEL_CORNER_POS_3,
	eBT_UTILI_LEVEL_CORNER_POS_4,
	eBT_UTILI_LEVEL_CORNER_POS_5,
	eBT_UTILI_LEVEL_MEASU_DIS_0,
	eBT_UTILI_LEVEL_MEASU_DIS_1,    //50 0032
	eBT_UTILI_LEVEL_MEASU_S1_NEXT,

	eBT_UTILI_LEVEL_MEASU_S2_NEXT,
	eBT_UTILI_LEVEL_MEASU_S1_EXIT_NO,
	eBT_UTILI_LEVEL_MEASU_S2_EXIT_NO,
	eBT_UTILI_LEVEL_MEASU_EXIT_OK,     //55  0037
	eBT_UTILI_LEVEL_MEASU_S3_EXIT_NO,
	eBT_MOVE_P0,       
	eBT_MOVE_P1,
	eBT_MOVE_P2,
	eBT_TUNE_SWITCH_LEDS,			//60  003C
	eBT_UTILI_LEVEL_MEASU_STOP_MOVE,
	eBT_UTILI_LEVEL_CORNER_BACK,
	eBT_PRINT_FILA_CHANGE_YES,
	eBT_PRINT_FILA_HEAT_NO,
	eBT_PRINT_FILA_UNLOAD_OK,		//65 0041
	eBT_PRINT_FILA_LOAD_OK,

	eBT_PRINT_HOME_FILAMENT,		// added for JX scrren
	eBT_PRINT_TUNE_FILAMENT			// added for JX scrren
};

enum E_MENU_TYPE {

	eMENU_IDLE,         // 0
	eMENU_HOME,
	eMENU_TUNE,         // 2
	eMENU_MOVE,
	eMENU_TUNE_E,       // 4
	eMENU_TUNE_B,
	eMENU_TUNE_FAN,     // 6
	eMENU_TUNE_SPEED,
	eMENU_TUNE_FLOW,    // 8 
	eMENU_UTILI_FILA,
	eMENU_PRINT_HOME,   // 10   
	eMENU_HOME_FILA,
	eMENU_FILE			//12

};

#define ID_MENU_HOME                (1)
#define ID_MENU_PRINT_HOME          (45)
#define ID_MENU_PRINT_HOME_PAUSE    (46)
#define ID_MENU_PRINT_TUNE			(47)
#define ID_MENU_PRINT_FILES_O		(21)

#define ID_DIALOG_PRINT_RECOVERY    (93)
#define ID_DIALOG_NO_FILA           (85)
#define ID_DIALOG_PRINT_START_0     (26)
#define ID_DIALOG_PRINT_START_1     (96)
#define ID_DIALOG_PRINT_FINISH      (81)
#define ID_DIALOG_FILA_NO_TEMP      (91)
#define ID_DIALOG_MOVE_NO_TEMP      (94)
#define ID_DIALOG_MOVE_WAIT         (126)
#define ID_DIALOG_PRINT_WAIT		(127)
#define ID_DIALOG_PRINT_TUNE_WAIT   (133)
#define ID_DIALOG_LEVEL_WAIT        (128)
#define ID_DIALOG_LEVEL_FAILE       (129)
#define ID_DIALOG_PRINT_LEVEL_FAILE (146)
#define ID_DIALOG_UTILI_FILA_WAIT	(139)
#define ID_DIALOG_UTILI_FILA_LOAD	(140)
#define ID_DIALOG_UTILI_FILA_UNLOAD	(141)
#define ID_DIALOG_LOAD_FINISH       (144)

#define ID_DIALOG_PRINT_FILA_WAIT	(134)
#define ID_DIALOG_PRINT_FILA_LOAD	(135)
#define ID_DIALOG_PRINT_FILA_UNLOAD	(136)

#define ID_MENU_UTILI_FILA_0        (87)
#define ID_MENU_HOME_FILA_0         (100)
#define ID_MENU_MOVE_0              (3)
#define ID_MENU_MOVE_1              (36)
#define ID_CRASH_KILLED             (107)
#define ID_MENU_MEASU_S1            (112)
#define ID_MENU_MEASU_S2            (114)
#define ID_MENU_MEASU_S3            (116)
#define ID_MENU_MEASU_FINISH        (123)

#define ID_DIALOG_CHANGE_FILA_0     (130) // added for JX screen
#define ID_DIALOG_CHANGE_FILA_1     (131) // added for JX screen

enum eAxis    : uint8_t { X=0, Y, Z };
enum eExtruder : uint8_t { E0=0, E1, E2, E3, E4, E5, E6, E7 };
enum eHeater   : uint8_t { H0=0, H1, H2, H3, H4, H5, BED, CHAMBER };
enum eFan      : uint8_t { FAN0=0, FAN1, FAN2, FAN3, FAN4, FAN5, FAN6, FAN7 };

#endif      // LGT_LCD_DW