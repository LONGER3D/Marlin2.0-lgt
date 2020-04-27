#pragma once

// #define TEST_TAG 		"T003"	// used for test release

#if !defined(TEST_TAG)
	#define TEST_TAG ""
#endif

#define FW_VERSION 		"V3.0" TEST_TAG "-" SHORT_BUILD_VERSION

#define LGT_LANGUAGE    "eng"     // choose language for lcd. Optional: eng, chn

#if defined(LK1) || defined(U20)
#define MAC_SIZE "300mm x 300mm x 400mm"
#elif defined(LK2) || defined(LK4) || defined(U30)
#define MAC_SIZE "220mm x 220mm x 250mm"
#elif defined(LK1_PLUS) ||  defined(U20_PLUS) 
#define MAC_SIZE "400mm x 400mm x 500mm"
#endif

#ifndef Chinese
//main page
#define TXT_MENU_HOME_MOVE                  "Move head"
#define TXT_MENU_HOME_FILE                  "Files"
#define TXT_MENU_HOME_EXTRUDE               "Extrude"
#define TXT_MENU_HOME_PREHEAT               "Preheating"
#define TXT_MENU_HOME_RECOVERY              "Recovery"
#define TXT_MENU_HOME_MORE                  "More"

//File page
#define TXT_MENU_FILE_SD_ERROR              "SD card error!"    
#define TXT_MENU_FILE_EMPTY                 "This folder is empty."  

//Extrude page
#define TXT_MENU_EXTRUDE_MANUAL             "JOG"            
#define TXT_MENU_EXTRUDE_AUTOMATIC          "AUTO"	

//More page
#define TXT_MENU_HOME_MORE_LEVELING         "Leveling"       
#define TXT_MENU_HOME_MORE_SETTINGS         "Settings"       
#define TXT_MENU_HOME_MORE_ABOUT            "About"           
#define TXT_MENU_HOME_MORE_RETURN           "Return"   

#define TXT_MENU_LEVELING_UNLOCK            "Unlock X-Y" 
//About page
#define TXT_MENU_ABOUT_MAX_SIZE_LABEL       "Max Buildable Size(LxWxH):"  
#define TXT_MENU_ABOUT_FW_VER_LABLE         "Firmware Version:"   
//settings
#define TXT_MENU_SETTS_JERK_X               "Vx-jerk(mm/s):"
#define TXT_MENU_SETTS_JERK_Y               "Vy-jerk(mm/s):"
#define TXT_MENU_SETTS_JERK_Z               "Vz-jerk(mm/s):"     
#define TXT_MENU_SETTS_JERK_E               "Ve-jerk(mm/s):"    
#define TXT_MENU_SETTS_VMAX_X               "Vmax x(mm/s):"     
#define TXT_MENU_SETTS_VMAX_Y               "Vmax y(mm/s):"		 
#define TXT_MENU_SETTS_VMAX_Z               "Vmax z(mm/s):"		 
#define TXT_MENU_SETTS_VMAX_E               "Vmax e(mm/s):"     
#define TXT_MENU_SETTS_VMIN                 "Vmin(mm/s):"       
#define TXT_MENU_SETTS_VTRAVEL              "Vtrav min(mm/s):"    
#define TXT_MENU_SETTS_AMAX_X               "Amax x(mm/s^2):"   
#define TXT_MENU_SETTS_AMAX_Y               "Amax y(mm/s^2):"	 
#define TXT_MENU_SETTS_AMAX_Z               "Amax z(mm/s^2):"    
#define TXT_MENU_SETTS_AMAX_E               "Amax e(mm/s^2):"    
#define TXT_MENU_SETTS_ARETRACT             "A-retract(mm/s^2):"  
//#define TXT_MENU_SETTS_ATRAVEL              "A-travel:"     
#define TXT_MENU_SETTS_STEP_X               "X(steps/mm):"       
#define TXT_MENU_SETTS_STEP_Y               "Y(steps/mm):"			
#define TXT_MENU_SETTS_STEP_Z               "Z(steps/mm):"		
#define TXT_MENU_SETTS_STEP_E               "E(steps/mm):"
#define TXT_MENU_SETTS_ACCL                 "Accel(mm/s^2):"    
#define TXT_MENU_SETTS_LIST_ORDER           "File list order:"
#define TXT_MENU_SETTS_CHECK_FILA           "Filament check:"      
#define TXT_MENU_SETTS_RECOVERY           	"powerloss recovery:"
#define TXT_MENU_SETTS_VALUE_ON             "ON"                
#define TXT_MENU_SETTS_VALUE_OFF            "OFF"                 
#define TXT_MENU_SETTS_VALUE_FORWARD        "FORWARD"             
#define TXT_MENU_SETTS_VALUE_INVERSE        "INVERSE"	

//Printing page
#define TXT_MENU_PRINT_STATUS_HEATING       "Heating..."          
#define TXT_MENU_PRINT_STATUS_PAUSING       "Pause printing..."    
#define TXT_MENU_PRINT_STATUS_RUNNING       "Printing..."            
#define TXT_MENU_PRINT_STATUS_RECOVERY      "Recovering..."           
#define TXT_MENU_PRINT_STATUS_FINISH        "Printing finished!"   
#define TXT_MENU_PRINT_STATUS_NO_FILAMENT   "No enough materials!"     
#define TXT_MENU_PRINT_CD_TIMER_NULL        "-- H -- M"
#define TXT_MENU_PRINT_CD_TIMER             "%d H %d M"
#define TXT_MENU_PRINT_TEMP_NULL            "B: --/--"

#define TXT_DIALOG_CAPTION_START            "Start"     
#define TXT_DIALOG_CAPTION_EXIT             "Exit"           
#define TXT_DIALOG_CAPTION_ABORT            "Stop"          
#define TXT_DIALOG_CAPTION_ABORT_WAIT       "Wait Stop"      
#define TXT_DIALOG_CAPTION_RECOVERY         "Recovery"        
#define TXT_DIALOG_CAPTION_ERROR            "Error"           
#define TXT_DIALOG_CAPTION_RESTORE          "Restore"           
#define TXT_DIALOG_CAPTION_SAVE             "Save"           
#define TXT_DIALOG_CAPTION_NO_FIALMENT      "No Filament"    
#define TXT_DIALOG_CAPTION_OPEN_FOLER       "Open Folder"  

// touch calibration
#define TXT_TFT_CONTROLLER_ID            "ControllerID:  %04X" //"ControllerID:"
#define TXT_TFT_CONTROLLER               "Controller: %s"
#define TXT_TOUCH_CALIBRATION            "Touch calibration"
#define TXT_TOP_LEFT                     "Top Left"
#define TXT_BOTTOM_LEFT                  "Bottom Left"
#define TXT_TOP_RIGHT                    "Top Right"
#define TXT_BOTTOM_RIGHT                 "Bottom Right"
#define TXT_CALI_COMPLETED               "Touch calibration completed"
#define TXT_X_CALIBRATION                "X_CALIBRATION:"
#define TXT_Y_CALIBRATION                "Y_CALIBRATION:"
#define TXT_X_OFFSET              		 "X_OFFSET:"
#define TXT_Y_OFFSET                     "Y_OFFSET:"
#define TXT_PROMPT_INFO1                 "Please reboot the printer manually"
// #define TXT_PROMPT_INFO2                 "return to the main home page!"

//Printer killed
#define TXT_PRINTER_KILLED_INFO1        "Printer halted."
#define TXT_PRINTER_KILLED_INFO2        "Please restart your printer."

// killed error message
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

//Dialog
#define DIALOG_PROMPT_PRINT_START1		"Are you sure to"//"Are you sure to start printing?"
#define DIALOG_PROMPT_PRINT_START2		"start printing?"
#define DIALOG_PROMPT_PRINT_START3
#define DIALOG_PROMPT_PRINT_EXIT1 		"Job's done. Do"//"Job's done. Do you want to exit?"
#define DIALOG_PROMPT_PRINT_EXIT2		"you want to exit?"
#define DIALOG_PROMPT_PRINT_EXIT3
#define DIALOG_PROMPT_PRINT_ABORT1 		"Job isn't done. Do"//"Job's not done. Do you want to stop it?"
#define DIALOG_PROMPT_PRINT_ABORT2		"you want to stop?"
#define DIALOG_PROMPT_PRINT_ABORT3
#define DIALOG_PROMPT_PRINT_RECOVERY1	"Do you want to"//"Are you sure to recovery printing?"
#define DIALOG_PROMPT_PRINT_RECOVERY2	"recovery print-"
#define DIALOG_PROMPT_PRINT_RECOVERY3	"ing?"
#define DIALOG_PROMPT_ERROR_READ1		"Failed to read file,"   //"Failed to read file, please try again."
#define DIALOG_PROMPT_ERROR_READ2		"please try again."
#define DIALOG_PROMPT_ERROR_READ3
#define DIALOG_PROMPT_SETTS_RESTORE1	"Are you sure to "   //	"Are you sure to reset factory settings?"
#define DIALOG_PROMPT_SETTS_RESTORE2	"reset factory se-"
#define DIALOG_PROMPT_SETTS_RESTORE3	"ttings?"
#define DIALOG_PROMPT_SETTS_SAVE_OK1	"Current settings"      //"Current settings has been saved."
#define DIALOG_PROMPT_SETTS_SAVE_OK2	"has been saved."
#define DIALOG_PROMPT_SETTS_SAVE_OK3
#define DIALOG_PROMPT_SETTS_SAVE1		"Do you want to "      //"Do you want to save current settings?"
#define DIALOG_PROMPT_SETTS_SAVE2		"save current se-"
#define DIALOG_PROMPT_SETTS_SAVE3		"ttings?"
#define DIALOG_PROMPT_NO_FILAMENT1		"Do you want to"      //"Do you want to change filament?"
#define DIALOG_PROMPT_NO_FILAMENT2		"change filament?"
#define DIALOG_PROMPT_NO_FILAMENT3
#define DIALOG_ERROR_FILE_TYPE1	     	"Failed to open file."   //	"Failed to open file. Unsupported file type."
#define DIALOG_ERROR_FILE_TYPE2			" Unsupported file type."
#define DIALOG_ERROR_FILE_TYPE3
#define DIALOG_ERROR_TEMP_BED1	     	 "Abnormal bed tem-"     
#define DIALOG_ERROR_TEMP_BED2			"perature is dete-"
#define DIALOG_ERROR_TEMP_BED3			"cted."
#define DIALOG_ERROR_TEMP_HEAD1    	    "Abnormal head te-" 
#define DIALOG_ERROR_TEMP_HEAD2 		"mperature is det-"
#define DIALOG_ERROR_TEMP_HEAD3			"ected."
#define DIALOG_PROMPT_MAX_FOLDER1		"Sorry, multi-level"
#define DIALOG_PROMPT_MAX_FOLDER2		"folders are not supported."
#define DIALOG_PROMPT_MAX_FOLDER3
#define DIALOG_START_PRINT_NOFILA1      "No filament, do"                 //No filament,please change filament and start printing
#define DIALOG_START_PRINT_NOFILA2		"you want to change"
#define DIALOG_START_PRINT_NOFILA3		"it?"

#else

#endif	// Chinese