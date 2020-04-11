#include "../inc/MarlinConfig.h"

#if ENABLED(LGT_LCD_TFT)
#include "lgttftlcd.h"
#include "lgttftdef.h"
#include "lcddrive/lcdapi.h"
#include "../feature/touch/xpt2046.h"
#include "w25qxx.h"

// #include "../module/temperature.h"
// #include "../sd/cardreader.h"
#include "../module/motion.h"
#include "../module/planner.h"
// #include "../module/printcounter.h"
// #include "../feature/runout.h"
// #include "../feature/powerloss.h"

#define DEBUG_LGTLCDTFT
#define DEBUG_OUT ENABLED(DEBUG_LGTLCDTFT)
#include "../../core/debug_out.h"

// wrap a new name
#define lcd                         lgtlcd
#define displayImage(x, y, addr)    lcd.showImage(x, y, addr)
#define White                       WHITE
#define lcdClear(color)             lcd.clear(color)
#define LCD_Fill(x, y, ex, ey, c)   lcd.fill(x, y, ex, ey, c)
#define color                       lcd.m_color
#define POINT_COLOR                 color
#define bgColor                     lcd.m_bgColor
#define display_image               LgtLcdTft
// #define enqueue_and_echo_commands_P 

#ifndef Chinese
	#define LCD_ShowString(x,y,txt)          lcd.print(x,y,txt) 
#else
	#define LCD_ShowString(x,y,txt)          lcd_print_gbk(x,y,txt)
#endif

#define CLEAN_STRING(str)				   		memset((void*)str,0,sizeof(str))
// #define SHOW_SINGLE_TXT(str, x, y, pt_color)    showText((char *)str, pt_color, WHITE, false, (uint16_t)(x), (uint16_t)(y))
// #define SHOW_BLACK_SINGLE_TXT(str, x, y)        SHOW_SINGLE_TXT(str, x, y, BLACK)
#define FILL_ZONE(x, y, w, h, bg_color)         LCD_Fill((uint16_t)(x), (uint16_t)(y), (uint16_t)((x)+(w)-1), (uint16_t)((y)+(h)-1), (uint16_t)bg_color)
#define CLEAN_ZONE(x, y, w, h)                  FILL_ZONE(x, y, w, h, WHITE)
#define CLEAN_SINGLE_TXT(x, y, w)               CLEAN_ZONE(x, y, w, 16)     /* clean single line text */


LgtLcdTft lgtlcdtft;

// extern bool check_filament_disable,list_order;
// extern ARGUMENTS re_settings;
// config_storedata ConfigSettings;
// extern Stopwatch print_job_timer;
// sdfile CardFile;
// extern uint8_t cp_time_minutes, cp_time_seconds,Remaining_minutes;	
// extern uint16_t cp_time_hours,Remaining_hours;
// extern CardReader card;
// extern uint16_t color,bgColor;
// extern  W25Qxx W25QxxFlash;
// extern display_image Display_Screen;
// extern Planner planner;
// extern uint16_t lcdId;
// extern uint16_t color;
// extern int16_t xCalibration, yCalibration, xOffset, yOffset;
// extern float resume_xyze_position[XYZE];
// extern float save_feedtate;
bool recovery_flag;
bool is_setting_change=false;

// extern void disable_all_steppers();
// extern void LGT_Lcdfsmc_init();

// uint16_t POINT_COLOR=0x0000;
// uint8_t image_buffer[GET_FLASH_SIZE];	//2k

// int16_t cur_fanspeed=0;
uint8_t default_move_distance=5;
// int8_t dir_auto_feed=0;
// static uint8_t total_out_distance=0;
// uint8_t printpercent=0;
// uint8_t page_index_num=0;
// int8_t choose_printfile=-1;
	
static uint16_t cur_x=0,cur_y=0;
// uint16_t page_index_max=0,page_index=0,file_count=0,choose_file_page=0;

static char s_text[64];
// char pdata[64];
// uint32_t print_times=0;
// const float manual_feedrate_mm_m[] = MANUAL_FEEDRATE;

// bool is_aixs_homed[XYZ]={false};
// static bool is_bed_select = false;
// bool sd_insert=false;
// bool is_printing=false;
// bool is_print_finish=false;
// bool pause_print=false;
// bool cur_flag=false;  
// char cur_pstatus=10;   //0 is heating ,1 is printing, 2 is pause
// char cur_ppage=10;   //  0 is heating page , 1 is printing page, 2 is pause page

// bool setting_return_more=false;
// float default_parameter[4]=DEFAULT_MAX_FEEDRATE;

// E_PRINT_CMD current_print_cmd=E_PRINT_CMD_NONE;
E_BUTTON_KEY current_button_id=eBT_BUTTON_NONE;
// /**********  window definition  **********/
E_WINDOW_ID current_window_ID = eMENU_HOME,next_window_ID =eWINDOW_NONE;

LgtLcdTft::LgtLcdTft()
{

}

// /***************************launch page*******************************************/
void LgtLcdTft::displayStartUpLogo(void)
{
    lcdClear(White);
  #if defined(U30) || defined(U20) || defined(U20_PLUS) 
  	displayImage(60, 95, IMG_ADDR_STARTUP_LOGO_0);
  #elif defined(LK1_PLUS) ||  defined(LK1) || defined(LK2) || defined(LK4)  
	displayImage(45, 100, IMG_ADDR_STARTUP_LOGO_2);
  #endif
}

// /***************************home page*******************************************/
void LgtLcdTft::displayWindowHome(void)
{
	lcdClear(White);
	LCD_Fill(0, 0, 320, 24, BG_COLOR_CAPTION_HOME); 	//caption background
	#ifndef Chinese
		displayImage(115, 5, IMG_ADDR_CAPTION_HOME);		//caption words  
	#else
		displayImage(115, 5, IMG_ADDR_CAPTION_HOME_CN);
	#endif
	displayImage(50, 45, IMG_ADDR_BUTTON_MOVE);
	displayImage(133, 45, IMG_ADDR_BUTTON_FILE);
	displayImage(215, 45, IMG_ADDR_BUTTON_EXTRUDE);
	displayImage(50, 145, IMG_ADDR_BUTTON_PREHEAT);
	if(false==recovery_flag)
	{
		displayImage(133, 145, IMG_ADDR_BUTTON_RECOVERY_DISABLE);
		color=PT_COLOR_DISABLE;
		CLEAN_STRING(s_text);
		sprintf((char*)s_text,"%s",TXT_MENU_HOME_RECOVERY);
		LCD_ShowString(129,209,s_text);
	}
	else
	{
		displayImage(133, 145, IMG_ADDR_BUTTON_RECOVERY);
		color=BLACK;
		CLEAN_STRING(s_text);
		sprintf((char*)s_text,"%s",TXT_MENU_HOME_RECOVERY);
		LCD_ShowString(129,209,s_text);	
	}
	displayImage(215, 145, IMG_ADDR_BUTTON_MORE);
	color=BLACK;
	CLEAN_STRING(s_text);
	sprintf((char*)s_text,"%s",TXT_MENU_HOME_MORE);
	LCD_ShowString(227,209,s_text);
	sprintf((char*)s_text,"%s",TXT_MENU_HOME_FILE);
	LCD_ShowString(142,109,s_text);
	sprintf((char*)s_text,"%s",TXT_MENU_HOME_EXTRUDE);
	LCD_ShowString(216,109,s_text);
	sprintf((char*)s_text,"%s",TXT_MENU_HOME_MOVE);
	#ifndef Chinese
		LCD_ShowString(41,109,s_text);
	#else
		LCD_ShowString(62,109,s_text);
	#endif
	sprintf((char*)s_text,"%s",TXT_MENU_HOME_PREHEAT);
	#ifndef Chinese
		LCD_ShowString(39,209,s_text);
	#else
		LCD_ShowString(62,209,s_text);
	#endif
}

void display_image::scanWindowHome(uint16_t rv_x, uint16_t rv_y)
{
	if(rv_x>50&&rv_x<105&&rv_y>45&&rv_y<95)
	{
		next_window_ID=eMENU_MOVE;
	}
	else if(rv_x>133&&rv_x<188&&rv_y>45&&rv_y<95)
	{
		next_window_ID=eMENU_FILE;
	}
	else if(rv_x>215&&rv_x<270&&rv_y>45&&rv_y<95)
	{
		next_window_ID=eMENU_EXTRUDE;
	}
	else if(rv_x>50&&rv_x<105&&rv_y>145&&rv_y<195)
	{
		next_window_ID=eMENU_PREHEAT;
	}
	else if(rv_x>133&&rv_x<188&&rv_y>145&&rv_y<195) //recovery
	{
		if(recovery_flag)
		{
			next_window_ID=eMENU_DIALOG_RECOVERY;
		}
	}
	else if(rv_x>215&&rv_x<270&&rv_y>145&&rv_y<195)
	{
		next_window_ID=eMENU_HOME_MORE;
	}
	
}

// /***************************Move page*******************************************/
void display_image::displayWindowMove(void)
{
	lcdClear(White);
	LCD_Fill(0, 0, 320, 25, BG_COLOR_CAPTION_MOVE); 	//caption background
	#ifndef Chinese
		displayImage(115, 5, IMG_ADDR_CAPTION_MOVE);		//caption words
	#else
		displayImage(115, 5, IMG_ADDR_CAPTION_MOVE_CN);		//caption words
	#endif
	displayImage(5, 180, IMG_ADDR_BUTTON_HOME_ALL);
	displayImage(5, 55, IMG_ADDR_BUTTON_HOME_X);
	displayImage(125, 55, IMG_ADDR_BUTTON_HOME_Y);
	displayImage(125, 180, IMG_ADDR_BUTTON_HOME_Z);
	displayImage(0, 118, IMG_ADDR_BUTTON_MINUS_X);
	displayImage(65, 170, IMG_ADDR_BUTTON_MINUS_Y);
	displayImage(193, 170, IMG_ADDR_BUTTON_MINUS_Z);
	displayImage(115, 118, IMG_ADDR_BUTTON_PLUS_X);
	displayImage(65, 55, IMG_ADDR_BUTTON_PLUS_Y);
	displayImage(193, 55, IMG_ADDR_BUTTON_PLUS_Z);
    default_move_distance = 5;		//default distance
	initialMoveDistance(260, 55);	
	displayImage(260, 110, IMG_ADDR_BUTTON_UNLOCK);
	displayImage(260, 180, IMG_ADDR_BUTTON_RETURN);
	POINT_COLOR=BLUE;
	CLEAN_STRING(s_text);
	sprintf((char*)s_text,"%s","X:");
	LCD_ShowString(40,32,s_text);
	sprintf((char*)s_text,"%s","Y:");
	LCD_ShowString(130,32,s_text);
	sprintf((char*)s_text,"%s","Z:");
	LCD_ShowString(220,32,s_text);	
	displayMoveCoordinate();
}

void display_image::changeMoveDistance(uint16_t pos_x, uint16_t pos_y)
{
		switch(default_move_distance)
		{
			default: break;
			case 1:
				default_move_distance = 5;
				displayImage(pos_x, pos_y, IMG_ADDR_BUTTON_DISTANCE_5);
			break;
			case 5:
				default_move_distance = 10;
				displayImage(pos_x, pos_y, IMG_ADDR_BUTTON_DISTANCE_10);
			break;
			case 10:
				if(current_window_ID == eMENU_EXTRUDE 
					|| current_window_ID == eMENU_ADJUST
					||current_window_ID == eMENU_ADJUST_MORE
					|| current_window_ID == eMENU_PREHEAT)
				{
					default_move_distance = 15;
					displayImage(pos_x, pos_y, IMG_ADDR_BUTTON_DISTANCE_MAX);
            	}
				else    /* if not in extrude or adjust menu */
				{  
					default_move_distance = 1;
					displayImage(pos_x, pos_y, IMG_ADDR_BUTTON_DISTANCE_1);
				}
			break;
			case 15:
				default_move_distance = 1;
				displayImage(pos_x, pos_y, IMG_ADDR_BUTTON_DISTANCE_1);
			break;
		}
}

void display_image::initialMoveDistance(uint16_t pos_x, uint16_t pos_y)
{
	switch(default_move_distance)
	{
		default:   break;	
		case 1:    displayImage(pos_x, pos_y, IMG_ADDR_BUTTON_DISTANCE_1); break;
		case 5:	   displayImage(pos_x, pos_y, IMG_ADDR_BUTTON_DISTANCE_5); break;
		case 10:   displayImage(pos_x, pos_y, IMG_ADDR_BUTTON_DISTANCE_10); break;
        case 15:case 0xff: displayImage(pos_x, pos_y, IMG_ADDR_BUTTON_DISTANCE_MAX); break;
	}
}

void display_image::displayMoveCoordinate(void)
{
	for(int i=0;i<3;i++)
	{
		CLEAN_SINGLE_TXT(POS_MOVE_COL_TXT+20 + POS_MOVE_TXT_INTERVAL * i, POS_MOVE_ROW_0, 60); 
		CLEAN_STRING(s_text);
		sprintf(s_text,"%.1f",current_position[i]);
		LCD_ShowString(60 + 90 * i, 32,s_text);
	}
}

void display_image::scanWindowMove( uint16_t rv_x, uint16_t rv_y )
{
    if(rv_x>260&&rv_x<315&&rv_y>180&&rv_y<235)  //return home
	{
		next_window_ID=eMENU_HOME;
	}
	else if(rv_x>0&&rv_x<60&&rv_y>115&&rv_y<165) //-X move		
	{						
		current_button_id=eBT_MOVE_X_MINUS;			
	}
	else if(rv_x>115&&rv_x<175&&rv_y>115&&rv_y<165)  	//+X move
	{ 						
			 current_button_id=eBT_MOVE_X_PLUS;	
	}
	else if(rv_x>65&&rv_x<115&&rv_y>170&&rv_y<230)  	//-Y move
	{					
		 current_button_id=eBT_MOVE_Y_MINUS;	
	}
	else if(rv_x>65&&rv_x<115&&rv_y>55&&rv_y<115)  	//+Y move
	{						
		current_button_id=eBT_MOVE_Y_PLUS;
	}
	else if(rv_x>190&&rv_x<240&&rv_y>170&&rv_y<230)  //-Z move
	{						
		current_button_id=eBT_MOVE_Z_MINUS;
	}
	else if(rv_x>190&&rv_x<240&&rv_y>55&&rv_y<115)   //+Z move
	{						
		current_button_id=eBT_MOVE_Z_PLUS;
	}
	else if(rv_x>5&&rv_x<55&&rv_y>55&&rv_y<105)   //x homing
	{							
		current_button_id=eBT_MOVE_X_HOME;
	}
	else if(rv_x>125&&rv_x<175&&rv_y>55&&rv_y<105)   //y homing
	{						
		current_button_id=eBT_MOVE_Y_HOME;
	}
	else if(rv_x>125&&rv_x<175&&rv_y>180&&rv_y<230)   //z homing
	{						
		current_button_id=eBT_MOVE_Z_HOME;
	}
	else if(rv_x>5&&rv_x<55&&rv_y>180&&rv_y<230)  	//all homing
	{						
		current_button_id=eBT_MOVE_ALL_HOME;
	}
	else if(rv_x>260&&rv_x<315&&rv_y>110&&rv_y<165)   //unlock all motors
	{						
		disable_all_steppers();
		set_all_unhomed();
	}
	else if(rv_x>260&&rv_x<315&&rv_y>55&&rv_y<95)  //select distance
	{	  					
		current_button_id=eBT_DISTANCE_CHANGE;
	}
}

// /***************************File page*******************************************/

// /***************************Extrude page*******************************************/

// /***************************preheating page*******************************************/

// /***************************home More page*******************************************/
void display_image::displayWindowHomeMore(void)
{
	lcdClear(White);
	LCD_Fill(0, 0, 320, 24, BG_COLOR_CAPTION_HOME); 	//caption background
	#ifndef Chinese
		displayImage(115, 5, IMG_ADDR_CAPTION_HOME);		//caption words
	#else
		displayImage(115, 5, IMG_ADDR_CAPTION_HOME_CN);
	#endif
	displayImage(50, 45, IMG_ADDR_BUTTON_LEVELING);
	displayImage(133, 45, IMG_ADDR_BUTTON_SETTINGS);
	displayImage(215, 45, IMG_ADDR_BUTTON_ABOUT);
	displayImage(215, 145, IMG_ADDR_BUTTON_RETURN);
	POINT_COLOR = BLACK;
	CLEAN_STRING(s_text);
	sprintf((char*)s_text,"%s",TXT_MENU_HOME_MORE_ABOUT);
	LCD_ShowString(224,109,s_text);	
	sprintf((char*)s_text,"%s",TXT_MENU_HOME_MORE_RETURN);
	LCD_ShowString(220,209,s_text);
	sprintf((char*)s_text,"%s",TXT_MENU_HOME_MORE_LEVELING);
   	LCD_ShowString(46,109,s_text); 
	sprintf((char*)s_text,"%s",TXT_MENU_HOME_MORE_SETTINGS);
	LCD_ShowString(130,109,s_text);
}

void display_image::scanWindowMoreHome(uint16_t rv_x, uint16_t rv_y)
{
	if(rv_x>50&&rv_x<105&&rv_y>45&&rv_y<95)
	{
		next_window_ID=eMENU_LEVELING;
	}
	else if(rv_x>133&&rv_x<188&&rv_y>45&&rv_y<95)
	{
		next_window_ID=eMENU_SETTINGS;
	}
	else if(rv_x>215&&rv_x<270&&rv_y>45&&rv_y<95)
	{
		next_window_ID=eMENU_ABOUT;
	}
	else if(rv_x>215&&rv_x<270&&rv_y>145&&rv_y<195)  //return home
	{
		next_window_ID=eMENU_HOME;
	}
}

// /***************************Printing page*******************************************/

// /***************************Adjust page*******************************************/

// /***************************dialog page*******************************************/


/**
 * page switching
 */
bool display_image::LGT_Ui_Update(void)
{
	bool button_type=false;   
	switch (next_window_ID)
		{
			case eMENU_HOME:
				// if(dir_auto_feed!=0)
				// 	stopAutoFeed();
				// else if(current_window_ID==eMENU_DIALOG_END)
				// {
				// 	card.flag.abort_sd_printing=true;
				// 	is_printing=wait_for_user = wait_for_heatup=false;
				// 	//clearVarPrintEnd();
				// }
				// else if(current_window_ID==eMENU_FILE)
				// {
				// 	card.returnroot();
				// 	clearfilevar();
				// }
				current_window_ID=eMENU_HOME;
				next_window_ID=eWINDOW_NONE;
				displayWindowHome();
				break;
			case eMENU_HOME_MORE:
				// if((current_window_ID==eMENU_LEVELING)&&all_axes_homed())
				// 	enqueue_and_echo_commands_P(PSTR("G0 Z10 F500"));
				if(current_window_ID==eMENU_SETTINGS&&is_setting_change)
				{
					// page_index=0;
					// choose_setting=ARGUMENST_MAX_NUM;
					// setting_return_more=true;
					// dispalyDialogYesNo(eDIALOG_SETTS_SAVE);
					// current_window_ID=eMENU_DIALOG_SAVE;
				}
				else
				{
					// if(current_window_ID==eMENU_DIALOG_SAVE)
					// 	ConfigSettings.settingsLoad();
					// else if(current_window_ID==eMENU_SETTINGS)
					// {
					// 	page_index=0;
					// 	choose_setting=ARGUMENST_MAX_NUM;
					// }
					current_window_ID=eMENU_HOME_MORE;
					displayWindowHomeMore();
				}
				next_window_ID=eWINDOW_NONE;
			break;
			 case eMENU_MOVE:
				current_window_ID=eMENU_MOVE;
				next_window_ID=eWINDOW_NONE;
				displayWindowMove();
			break;
			// case eMENU_FILE:
			// 	current_window_ID=eMENU_FILE;
			// 	next_window_ID=eWINDOW_NONE;
			// 	clearfilevar();
			// 	displayWindowFiles();
			// break;
			// case eMENU_FILE1:
			// 	current_window_ID=eMENU_FILE;
			// 	next_window_ID=eWINDOW_NONE;
			// 	displayChosenFile();
			// break;
			// case eMENU_PRINT:
			// 	current_window_ID=eMENU_PRINT;
			// 	next_window_ID=eWINDOW_NONE;
			// 	displayWindowPrint();
			// break;
			// case eMENU_ADJUST:
			// 	current_window_ID=eMENU_ADJUST;
			// 	next_window_ID=eWINDOW_NONE;
			// 	displayWindowAdjust();
			// break;
			// case eMENU_ADJUST_MORE:
			// 	if(current_window_ID==eMENU_EXTRUDE)
			// 	{
			// 		planner.set_e_position_mm((destination[E_AXIS] = current_position[E_AXIS] = (resume_xyze_position[E_AXIS] - 2)));
			// 		feedrate_mm_s=save_feedtate;
			// 	}
			// 	current_window_ID=eMENU_ADJUST_MORE;
			// 	next_window_ID=eWINDOW_NONE;
			// 	displayWindowAdjustMore();
			// break;
			// case eMENU_PREHEAT:
			// 	current_window_ID=eMENU_PREHEAT;
			// 	next_window_ID=eWINDOW_NONE;
			// 		displayWindowPreheat();
			// break;
			// case eMENU_LEVELING:
			// 	current_window_ID=eMENU_LEVELING;
			// 	next_window_ID=eWINDOW_NONE;
			// 	set_all_unhomed();
			// 	displayWindowLeveling();
			// break;
			// case eMENU_EXTRUDE:
			// 	current_window_ID=eMENU_EXTRUDE;
			// 	next_window_ID=eWINDOW_NONE;
			// 	is_bed_select=false;
			// 	displayWindowExtrude();
			// break;
			// case eMENU_SETTINGS:
			// 	current_window_ID=eMENU_SETTINGS;
			// 	next_window_ID=eWINDOW_NONE;
			// 	displayWindowSettings();
			// break;
			// // eMENU_SETTINGS2,
			// case eMENU_ABOUT:
			// 	current_window_ID=eMENU_ABOUT;
			// 	next_window_ID=eWINDOW_NONE;
			// 	displayWindowAbout();
			// break;
			// case eMENU_DIALOG_RECOVERY:
			// 	current_window_ID=eMENU_DIALOG_RECOVERY;
			// 	next_window_ID=eWINDOW_NONE;
			// 	dispalyDialogYesNo(eDIALOG_PRINT_RECOVERY);
			// break;
			default:
				button_type=true;
				break;
		}
		if(button_type==false)
			current_button_id=eBT_BUTTON_NONE;
	return button_type;
}

/**
 * touch scanning 
 */
bool LgtLcdTft::LGT_MainScanWindow(void)
{
		bool window_change=true;
		switch (current_window_ID)
		{
			case eMENU_HOME:
				scanWindowHome(cur_x,cur_y);
				cur_x=cur_y=0;
				break;
			case eMENU_HOME_MORE:
				scanWindowMoreHome(cur_x,cur_y);
				cur_x=cur_y=0;
			break;
			case eMENU_MOVE:
				scanWindowMove(cur_x,cur_y);
				cur_x=cur_y=0;
			break;
			// case eMENU_FILE:
			// 	scanWindowFile(cur_x,cur_y);
			// 	cur_x=cur_y=0;
			// break;
			// case eMENU_PRINT:
			// 	scanWindowPrint(cur_x,cur_y);
			// 	cur_x=cur_y=0;
			// break;
			// case eMENU_ADJUST:
			//     scanWindowAdjust(cur_x,cur_y);
			// 	cur_x=cur_y=0;
			// break;
			// case eMENU_ADJUST_MORE:
			// 	scanWindowAdjustMore(cur_x,cur_y);
			// 	cur_x=cur_y=0;
			// break;
			// case eMENU_PREHEAT:
			// 	scanWindowPreheating(cur_x,cur_y);
			// 	cur_x=cur_y=0;
			// break;
			// case eMENU_LEVELING:
			// 	scanWindowLeveling(cur_x,cur_y);
			// 	cur_x=cur_y=0;
			// break;
			// case eMENU_EXTRUDE:
			// 	scanWindowExtrude(cur_x,cur_y);
			// 	cur_x=cur_y=0;
			// break;
			// case eMENU_SETTINGS:
			// 	scanWindowSettings(cur_x,cur_y);
			// 	cur_x=cur_y=0;
			// break;
			// case eMENU_SETTINGS2:
			// 	scanWindowSettings2(cur_x,cur_y);
			// 	cur_x=cur_y=0;
			// break;
			// case eMENU_ABOUT:
			// 	scanWindowAbout(cur_x,cur_y);
			// 	cur_x=cur_y=0;
			// break;
			// case eMENU_DIALOG_START:case eMENU_DIALOG_NO_FIL:
			// 	scanDialogStart(cur_x,cur_y);
			// 	cur_x=cur_y=0;
			// break;
			// case eMENU_DIALOG_END:
			// 	scanDialogEnd(cur_x,cur_y);
			// 	cur_x=cur_y=0;
			// break;
			// case eMENU_DIALOG_RECOVERY:
			// 	scanDialogRecovery(cur_x,cur_y);
			// 	cur_x=cur_y=0;
			// break;
			// case eMENU_DIALOG_REFACTORY:
			// 	scanDialogRefactory(cur_x,cur_y);
			// 	cur_x=cur_y=0;
			// break;
			// case eMENU_DIALOG_SAVE:
			// 	scanDialogSave(cur_x,cur_y);
			// 	cur_x=cur_y=0;
			// break;
			// case eMENU_DIALOG_SAVE_OK:case eMENU_DIALOG_ERRORTEMPE:case eMENU_DIALOG_ERRORTEMPBED:
			// 	scanDialogYes(cur_x,cur_y);
			// 	cur_x=cur_y=0;
			// break;
			default:
				window_change=false;
				break;
		}
		return window_change;
}

void LgtLcdTft::init()
{
    // init tft-lcd
    lcd.init();
    lcd.clear();
    displayStartUpLogo();
    delay(1000);
    displayWindowHome();
}

void LgtLcdTft::loop()
{
    #define TOUCH_DELAY 10u // millsecond
    #define CHECK_TOUCH_TIME 4u
    #define TRUELY_TOUCHED() (touchCheck > CHECK_TOUCH_TIME)
    static millis_t nextTouchReadTime = 0;
    static uint8_t touchCheck = 0;
 
        if (touch.isTouched()) { 
            const millis_t time = millis();
            if (ELAPSED(time, nextTouchReadTime)) {
                nextTouchReadTime = time + TOUCH_DELAY;
                if (touch.isTouched()) {
                    touchCheck++;
                    if (TRUELY_TOUCHED()) {  // truely touched
                        touch.readTouchPoint(cur_x, cur_y);
                        DEBUG_ECHOPAIR("touched: x: ", cur_x);
                        DEBUG_ECHOLNPAIR(", y: ", cur_y);
                        LGT_MainScanWindow();
                    }
                }
            }
        } else if (TRUELY_TOUCHED()) {  // touch released
            touchCheck = 0;
            DEBUG_ECHOPGM("touch: released ");


            LGT_Ui_Update();

        } else {    // idle
            touchCheck = 0;
        }

}

#endif