#pragma once

// #include "../inc/MarlinConfigPre.h"

#if ENABLED(LGT_LCD_TFT)
class LgtLcdTft {
public:
    LgtLcdTft();
    void init();
    void loop();
private:
    void displayStartUpLogo(void);

    // bool LGT_MainScanWindow(void);
    // bool LGT_Ui_Update(void);
    // void LGT_Printer_Data_Update(void);
    // void LGT_Ui_Buttoncmd(void);

    // bool setTemperatureInWindow(bool is_bed, bool sign);
    // void LGT_Tempabnormal_Warning(const char* info);    //is_printing=false
    // void LGT_Printerabnormal_Kill(const char* info);

    // /***************************Move page*******************************************/
    // void scanWindowMove( uint16_t rv_x, uint16_t rv_y );
    // void changeMoveDistance(uint16_t pos_x, uint16_t pos_y);
    // void initialMoveDistance(uint16_t pos_x, uint16_t pos_y);
    // void displayMoveCoordinate(void);
    // /***************************File page*******************************************/
    // void scanWindowFile( uint16_t rv_x, uint16_t rv_y );
    // void displayFilePageNumber(void);
    // void displayFileList();
    // void displayChosenFile(void);
    // void displayPromptSDCardError(void);
    // void displayPromptEmptyFolder(void);

    // /***************************Extrude page*******************************************/
    // void scanWindowExtrude( uint16_t rv_x, uint16_t rv_y );
    // void dispalyExtrudeTemp(void);
    // void displayRunningAutoFeed(void);

    // /***************************preheating page*******************************************/
    // void scanWindowPreheating( uint16_t rv_x, uint16_t rv_y );
    // void updatePreheatingTemp(void);

    // /***************************More page*******************************************/
    // void scanWindowMoreHome(uint16_t rv_x, uint16_t rv_y);
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


    // /***************************Start page*******************************************/
    // void displayStartUpLogo(void);
    // void displayWindowHome(void);

    // /***************************Home page*******************************************/
    // void scanWindowHome(uint16_t rv_x, uint16_t rv_y);
    // void displayWindowMove(void);
    // void displayWindowFiles(void);
    // void displayWindowExtrude(void);
    // void displayWindowPreheat(void);
    // void displayWindowHomeMore(void);


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
