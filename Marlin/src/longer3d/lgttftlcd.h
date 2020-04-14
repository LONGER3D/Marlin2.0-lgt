#pragma once

// #include "../inc/MarlinConfigPre.h"

#if ENABLED(LGT_LCD_TFT)

typedef enum{
	eMENU_HOME = 0,
    eMENU_HOME_MORE,
	eMENU_MOVE,
	eMENU_FILE,
	eMENU_FILE1,   //next_window_id=eMENU_FILE1 when click eBT_DIALOG_PRINT_NO
	eMENU_PRINT,
	eMENU_ADJUST,
	eMENU_ADJUST_MORE,
	eMENU_PREHEAT,
	eMENU_LEVELING,
	eMENU_EXTRUDE,
	eMENU_SETTINGS,
	eMENU_SETTINGS2,
	eMENU_ABOUT,
	eMENU_DIALOG_START,   //print or not print
	eMENU_DIALOG_END,     //stop print or not stop print
	eMENU_DIALOG_NO_FIL,   // no filament page
	eMENU_DIALOG_RECOVERY,
	eMENU_DIALOG_REFACTORY,
	eMENU_DIALOG_SAVE,
	eMENU_DIALOG_SAVE_OK,
	eMENU_DIALOG_ERRORTEMPBED,
	eMENU_DIALOG_ERRORTEMPE,
// 	eMENU_PCB_TEST,
// 	eMENU_SCREEN_CALIBRATION,
// 	eMENU_AGING_TEST,
// #ifdef LCD_COLOR_TEST
// 	eMENU_SCREEN_COLOR_TEST,
// #endif	
 	eWINDOW_MAX ,
	eWINDOW_NONE
}E_WINDOW_ID;

enum E_BUTTON_KEY {
/*******************Move page****************************/
	eBT_BUTTON_NONE=0,
	eBT_MOVE_X_MINUS,eBT_MOVE_X_PLUS,eBT_MOVE_X_HOME,
	eBT_MOVE_Y_MINUS,eBT_MOVE_Y_PLUS,eBT_MOVE_Y_HOME,
	eBT_MOVE_Z_MINUS,eBT_MOVE_Z_PLUS,eBT_MOVE_Z_HOME,
	eBT_MOVE_ALL_HOME,

/*******************Leveling page****************************/
	eBT_MOVE_L0,eBT_MOVE_L1,eBT_MOVE_L2,eBT_MOVE_L3,eBT_MOVE_L4, eBT_MOVE_RETURN,

/*******************preheating page****************************/
	eBT_PR_PLA,eBT_PR_ABS,eBT_PR_PETG,eBT_PR_COOL,
	eBT_PR_E_PLUS,eBT_PR_E_MINUS,eBT_PR_B_PLUS,EBT_PR_B_MINUS,

/*******************Extrude page****************************/	
	eBT_TEMP_PLUS,eBT_TEMP_MINUS,//eBT_BED_PLUS,eBT_BED_MINUS,
	eBT_JOG_EPLUS,eBT_JOG_EMINUS,eBT_AUTO_EPLUS,eBT_AUTO_EMINUS,eBT_STOP,eBT_BED_E,

/*******************File page****************************/	
	eBT_FILE_NEXT,eBT_FILE_LAST,eBT_FILE_OPEN,eBT_FILE_FOLDER,
	eBT_FILE_LIST1,eBT_FILE_LIST2,eBT_FILE_LIST3,eBT_FILE_LIST4,eBT_FILE_LIST5,

/*******************printing page****************************/	
	eBT_PRINT_PAUSE,eBT_PRINT_ADJUST,eBT_PRINT_END,

/*******************Adjust page****************************/
	eBT_ADJUSTE_PLUS,eBT_ADJUSTE_MINUS,eBT_ADJUSTBED_PLUS,eBT_ADJUSTBED_MINUS,
	eBT_ADJUSTFAN_PLUS,eBT_ADJUSTFAN_MINUS,eBT_ADJUSTSPEED_PLUS,eBT_ADJUSTSPEED_MINUS,

/****************************Dialog page*******************************/
	eBT_DIALOG_PRINT_START,eBT_DIALOG_PRINT_NO,eBT_DIALOG_REFACTORY_YES,eBT_DIALOG_SAVE_YES,
	
/****************************Settings page*******************************/
	eBT_SETTING_ADJUST,eBT_SETTING_REFACTORY,eBT_SETTING_SAVE,eBT_SETTING_LAST,eBT_SETTING_NEXT,eBT_SETTING_ADD,eBT_SETTING_SUB,

	eBT_DISTANCE_CHANGE
};

class LgtLcdTft {
public:
    LgtLcdTft();
    void init();
    void loop();
private:
    bool LGT_MainScanWindow(void);
    bool LGT_Ui_Update(void);
    // void LGT_Printer_Data_Update(void);
    void LGT_Ui_Buttoncmd(void);

    // bool setTemperatureInWindow(bool is_bed, bool sign);
    // void LGT_Tempabnormal_Warning(const char* info);    //is_printing=false
    // void LGT_Printerabnormal_Kill(const char* info);

    // /***************************launch page*******************************************/
    void displayStartUpLogo(void);
    // /***************************home page*******************************************/
    void displayWindowHome(void);
    void scanWindowHome(uint16_t rv_x, uint16_t rv_y);

    // /***************************Move page*******************************************/
    void displayWindowMove(void);
    void scanWindowMove( uint16_t rv_x, uint16_t rv_y );
    void changeMoveDistance(uint16_t pos_x, uint16_t pos_y);
    void initialMoveDistance(uint16_t pos_x, uint16_t pos_y);
    void displayMoveCoordinate(void);
    // /***************************File page*******************************************/
    void displayWindowFiles(void);
    void displayFilePageNumber(void);
    void displayFileList();
    // void displayChosenFile(void);
    void displayPromptSDCardError(void);
    void displayPromptEmptyFolder(void);
    void scanWindowFile( uint16_t rv_x, uint16_t rv_y );


    // /***************************Extrude page*******************************************/
    // void displayWindowExtrude(void);
    // void scanWindowExtrude( uint16_t rv_x, uint16_t rv_y );
    // void dispalyExtrudeTemp(void);
    // void displayRunningAutoFeed(void);

    // /***************************preheating page*******************************************/
    // void displayWindowPreheat(void);
    // void scanWindowPreheating( uint16_t rv_x, uint16_t rv_y );
    // void updatePreheatingTemp(void);

    // /***************************home More page*******************************************/
    void displayWindowHomeMore(void);
    void scanWindowMoreHome(uint16_t rv_x, uint16_t rv_y);
    // void scanWindowLeveling( uint16_t rv_x, uint16_t rv_y );
    // void scanWindowAbout(uint16_t rv_x, uint16_t rv_y);
    // void displayWindowLeveling(void);
    // void displayWindowAbout(void);

    // void displayArugumentPageNumber(void);
    // void displayArgumentList(void);
    // void displayWindowSettings(void);
    // void scanWindowSettings(uint16_t rv_x, uint16_t rv_y);
    // void displayWindowSettings2(void);
    // void displayModifyArgument(void);
    // void scanWindowSettings2(uint16_t rv_x, uint16_t rv_y);



    // /***************************Printing page*******************************************/
    // void scanWindowPrint( uint16_t rv_x, uint16_t rv_y );
    // void displayWindowPrint(void);
    // void displayPrintInformation(void);
    // void displayRunningFan(uint16_t pos_x, uint16_t pos_y);
    // void displayFanSpeed(void);
    // void displayHeating(void);
    // void displayPrinting(void);
    // void displayPause(void);
    // void displayPrintTemperature(void);
    // void displayPrintProgress(void);
    // void displayHeightValue(void);
    // void dispalyCurrentStatus(void);
    // //void calcCountUpTime(void);
    // void displayCountUpTime(void);
    // void displayCountDownTime(void);

    // /***************************Adjust page*******************************************/
    // void displayWindowAdjust(void);
    // void dispalyAdjustTemp(void);
    // void dispalyAdjustFanSpeed(void);
    // void dispalyAdjustMoveSpeed(void);
    // void scanWindowAdjust(uint16_t rv_x,uint16_t rv_y);
    // void dispalyAdjustFlow(void);
    // void displayWindowAdjustMore(void);
    // void scanWindowAdjustMore(uint16_t rv_x,uint16_t rv_y);

    // /***************************dialog page*******************************************/
    // void dispalyDialogYesNo(uint8_t dialog_index);
    // void dispalyDialogYes(uint8_t dialog_index);
    // void displayDialogText(uint8_t dialog_index);
    // void scanDialogStart(uint16_t rv_x, uint16_t rv_y );
    // void scanDialogEnd( uint16_t rv_x, uint16_t rv_y ); 
    // void displayNofilament(void);
    // void scanDialogRecovery( uint16_t rv_x, uint16_t rv_y);
    // void scanDialogRefactory(uint16_t rv_x, uint16_t rv_y);
    // void scanDialogSave(uint16_t rv_x, uint16_t rv_y);
    // void scanDialogYes(uint16_t rv_x, uint16_t rv_y);

private:
    bool extrude2file = false;

};

extern LgtLcdTft lgtlcdtft; 

#endif
