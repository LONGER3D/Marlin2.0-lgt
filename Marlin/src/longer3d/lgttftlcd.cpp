#include "../inc/MarlinConfig.h"

#if ENABLED(LGT_LCD_TFT)
#include "lgttftlcd.h"
#include "lgttftdef.h"
#include "lcddrive/lcdapi.h"
#include "../feature/touch/xpt2046.h"
#include "w25qxx.h"
#include "lgtsdcard.h"

#include "../module/temperature.h"
#include "../sd/cardreader.h"
#include "../HAL/STM32F1/sdio.h"
#include "../module/motion.h"
#include "../module/planner.h"
// #include "../module/printcounter.h"
// #include "../feature/runout.h"
// #include "../feature/powerloss.h"

#define DEBUG_LGTLCDTFT
#define DEBUG_OUT ENABLED(DEBUG_LGTLCDTFT)
#include "../../core/debug_out.h"

// wrap a new name
#define lcd                             lgtlcd
#define displayImage(x, y, addr)        lcd.showImage(x, y, addr)
#define White                           WHITE
#define Black                           BLACK
#define lcdClear(color)                 lcd.clear(color)
#define LCD_Fill(x, y, ex, ey, c)       lcd.fill(x, y, ex, ey, c)
#define color                           lcd.m_color
#define POINT_COLOR                     color
#define bgColor                         lcd.m_bgColor
#define display_image                   LgtLcdTft
#define enqueue_and_echo_commands_P(s)  queue.enqueue_now_P(s)
#define LCD_DrawLine(x, y, ex, ey)      lcd.drawHVLine(x, y, ex, ey)
#define LCD_DrawRectangle(x, y, ex, ey) lcd.drawRect(x, y, ex, ey)
// #define temp_hotend[0].current   temp_hotend[0].celsius


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
static int8_t dir_auto_feed=0;
static uint8_t total_out_distance=0;
// uint8_t printpercent=0;
uint8_t page_index_num=0;
int8_t choose_printfile=-1;
	
static uint16_t cur_x=0,cur_y=0;
// uint16_t page_index_max=0,page_index=0,file_count=0,choose_file_page=0;

static char s_text[64];
// char pdata[64];
// uint32_t print_times=0;
// const float manual_feedrate_mm_m[] = MANUAL_FEEDRATE;

static bool is_aixs_homed[XYZ]={false};
static bool is_bed_select = false;
// bool sd_insert=false;
static bool is_printing=false;	// print status
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


// /***************************static function definition****************************************/

static void LGT_Line_To_Current_Position(AxisEnum axis) 
{
	const float manual_feedrate_mm_m[] = MANUAL_FEEDRATE;
	if (!planner.is_full())
		planner.buffer_line(current_position, MMM_TO_MMS(manual_feedrate_mm_m[(int8_t)axis]), active_extruder);
}

static void stopAutoFeed(void)
{
	if(dir_auto_feed==1||dir_auto_feed==-1)
	{
		queue.clear();	
		planner.quick_stop();
		enqueue_and_echo_commands_P(PSTR("G90"));
		dir_auto_feed=0;
		total_out_distance=0;
	}
}

static void actAutoFeed(void)
{
	if(dir_auto_feed == 0) return; 
	if(dir_auto_feed == 1)   //positive direction
	{		       				
		enqueue_and_echo_commands_P(PSTR("G0 E15 F120"));
	}
	else if(dir_auto_feed == -1)  //negative direction
	{      
        if(total_out_distance < 90)
		{
            enqueue_and_echo_commands_P(PSTR("G0 E-10 F600"));
            total_out_distance += 1;
        }
		else  // arrive max length 
		{  
            stopAutoFeed();
           thermalManager.temp_hotend[0].target=0;  //prevent from fire harzard   
        }
	}
}



static void startAutoFeed(int8_t dir)
{
	if(dir == dir_auto_feed) return; 
	if(thermalManager.temp_hotend[0].target<200)
	{
		thermalManager.temp_hotend[0].target=200;
		thermalManager.start_watching_hotend(0);
		if(thermalManager.temp_hotend[0].celsius>195)
		{
			 if(abs(dir) == 1)    //  validity checking
			{    
        		dir_auto_feed = dir;  
				enqueue_and_echo_commands_P(PSTR("G91"));
        		total_out_distance = 0;
   			 }
		}
		if(is_bed_select)
		{
			is_bed_select=false;
			lcd.showImage(15, 95, IMG_ADDR_BUTTON_BED_OFF);
		}
	}
	else if(thermalManager.temp_hotend[0].celsius>195)
	{
		if(abs(dir) == 1)    //  validity checking
		{    
        	dir_auto_feed = dir;  
			enqueue_and_echo_commands_P(PSTR("G91"));
        	total_out_distance = 0;
   		 }
		if(is_bed_select)
		{
			is_bed_select=false;
			lcd.showImage(15, 95, IMG_ADDR_BUTTON_BED_OFF);
		}
	}
}
// /***************************class definition start************************************/
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
void display_image::displayWindowFiles(void)
{
	lcdClear(White);
	LCD_Fill(0, 0, 320, 24, BG_COLOR_CAPTION_FILE); 	//caption background
	#ifndef Chinese
    	displayImage(115, 5, IMG_ADDR_CAPTION_FILE);		//caption words  
		displayImage(255, 30, IMG_ADDR_BUTTON_OPEN);
	#else
		displayImage(115, 5,  IMG_ADDR_CAPTION_FILE_CN);		//caption words
		displayImage(255, 30, IMG_ADDR_BUTTON_OPEN_CN);
	#endif
	displayImage(255, 105, IMG_ADDR_BUTTON_RETURN_FOLDER);
	displayImage(150, 180, IMG_ADDR_BUTTON_PAGE_NEXT);
	displayImage(35, 180, IMG_ADDR_BUTTON_PAGE_LAST);
	displayImage(255, 180, IMG_ADDR_BUTTON_RETURN);	
	// draw frame
	POINT_COLOR=DARKBLUE;	
	LCD_DrawLine(0, 175, 240, 175);
	LCD_DrawLine(0, 176, 240, 176);
	LCD_DrawLine(240, 175, 240, 25);
	LCD_DrawLine(241, 176, 241, 25);

	if((SDIO_GetCardState() == SDIO_CARD_ERROR)) {
		displayPromptSDCardError();
        lgtCard.clear();
    } else {
        int fCount = lgtCard.count();
        DEBUG_ECHOLNPAIR("sd filecount", fCount);
		if (fCount == 0) {
			displayPromptEmptyFolder();
		} else {
            displayFileList();
            displayFilePageNumber();           
        }
	}

}

void display_image::displayPromptSDCardError(void)
{
	LCD_Fill(100, 195, 145, 215, White);    //clean file page number display zone
	LCD_Fill(0, 25, 239, 174,White);	//clean file list display zone 
	displayImage(45, 85, IMG_ADDR_PROMPT_ERROR);
	color=RED;	
	CLEAN_STRING(s_text);
	sprintf((char*)s_text,"%s", TXT_MENU_FILE_SD_ERROR);
	LCD_ShowString(80, 92,s_text);
	color=Black;
	// clearfilevar();
}

void display_image::displayPromptEmptyFolder(void)
{
    LCD_Fill(100, 195, 145, 215, White);    //clean file page number display zone
	LCD_Fill(0, 25, 239, 174,White);	//clean file list display zone 
	color = GRAY;
	CLEAN_STRING(s_text);
	sprintf((char*)s_text,"%s", TXT_MENU_FILE_EMPTY);
	LCD_ShowString(35,87,s_text); 
	color = BLACK;
}

void display_image::displayFilePageNumber(void)
{
	LCD_Fill(100, 195, 145, 215, White);	//celan file page number display zone
	if(lgtCard.fileCount() > 0)
	{
		CLEAN_STRING(s_text);
		POINT_COLOR=BLACK;
		sprintf((char *)s_text, "%d/%d", lgtCard.page() + 1, lgtCard.pageCount());
		LCD_ShowString(105, 200,s_text);
	}
}

void display_image::displayFileList()
{
    // debug
    lcd.setColor(BLACK);
	if(!lgtCard.isReverseList())    //forward
	{
        uint16_t start = lgtCard.page() * LIST_ITEM_MAX;
        uint16_t end = start + LIST_ITEM_MAX;
        NOMORE(end, lgtCard.fileCount());
        DEBUG_ECHOLNPAIR("list start:", start);
        DEBUG_ECHOLNPAIR("list end: ", end);
		for (uint16_t i = start, j = 0; i < end; ++i, ++j) {
            DEBUG_ECHOLNPAIR("sd filename: ", lgtCard.filename(i));
            LCD_ShowString(35, 32 + j * 30, lgtCard.filename(i));
            if(lgtCard.isDir())
				displayImage(0, 25 + j * 30, IMG_ADDR_INDICATOR_FOLDER);
			else
				displayImage(0, 25 + j * 30, IMG_ADDR_INDICATOR_FILE);
		}
	} else {        // inverse
        uint16_t start = lgtCard.fileCount() - 1 - lgtCard.page() * LIST_ITEM_MAX;
        uint16_t end = start > LIST_ITEM_MAX ? start - LIST_ITEM_MAX : 0;
        DEBUG_ECHOLNPAIR("list start:", start);
        DEBUG_ECHOLNPAIR("list end: ", end);
		for (uint16_t i = start, j = 0; i > end; --i, ++j) {
            DEBUG_ECHOLNPAIR("sd filename: ", lgtCard.filename(i));
            LCD_ShowString(35, 32 + j * 30, lgtCard.filename(i));
            if(lgtCard.isDir())
				displayImage(0, 25 + j * 30, IMG_ADDR_INDICATOR_FOLDER);
			else
				displayImage(0, 25 + j * 30, IMG_ADDR_INDICATOR_FILE);
		}
	}

}

void LgtLcdTft::highlightChosenItem(uint16_t item)
{
    uint16_t lastItem = lgtCard.item();
    uint16_t lastIndex = lgtCard.fileIndex();   // save last selected file index
    uint16_t lastPage = lgtCard.selectedPage(); // save last selected page
    if (lastItem == item && item > 0)   // nothing should change
        return;
    if (!lgtCard.setItem(item)) // fail to set item
        return;
    // if (lgtCard.isFileSelected() && lastIndex == lgtCard.fileIndex())
    //     return;
    DEBUG_ECHOLNPAIR("last item: ", lastItem);
    DEBUG_ECHOLNPAIR("last index: ", lastIndex);
    DEBUG_ECHOLNPAIR("select item: ", item);
    DEBUG_ECHOLNPAIR("select index: ", lgtCard.fileIndex());

    if (lastPage == lgtCard.page()) {  // only restore when selected page is as same as last one
        // restore last selected item
        lcd.fill(35, 25 + lastItem * 30, 239, 55 - 1 + lastItem * 30, WHITE);
        lcd.print(35, 32 + lastItem*30, lgtCard.filename(lastIndex));
    }
    // highlight selecetd item
    // lgtCard.setItem(item);
    // .. darkblue background
    lcd.fill(35, 25 + item * 30, 239, 55 - 1 + item * 30, DARKBLUE);
    // .. reprint filename
    lcd.setColor(WHITE);
    lcd.setBgColor(DARKBLUE);
    lcd.print(35, 32 + item*30, lgtCard.filename());

    lcd.setColor(BLACK);
    lcd.setBgColor(WHITE);
}

void display_image::scanWindowFile( uint16_t rv_x, uint16_t rv_y )
{
	if(rv_x>260&&rv_x<315&&rv_y>176&&rv_y<226)  //return home
	{
		next_window_ID=eMENU_HOME;
	}
	else if(rv_x>0&&rv_x<240&&rv_y>25&&rv_y<55)	//1st file
	{		
		current_button_id=eBT_FILE_LIST1;
	}
	else if(rv_x>0&&rv_x<240&&rv_y>55&&rv_y<85)	//2nd file
	{		
		current_button_id=eBT_FILE_LIST2;
	}
	else if(rv_x>0&&rv_x<240&&rv_y>85&&rv_y<115)	//3rd file
	{		
		current_button_id=eBT_FILE_LIST3;
	}
	else if(rv_x>0&&rv_x<240&&rv_y>115&&rv_y<145)	//4th file
	{		
	current_button_id=eBT_FILE_LIST4;
	} 
	else if(rv_x>0&&rv_x<240&&rv_y>145&&rv_y<175)  //5th file
	{		
		current_button_id=eBT_FILE_LIST5;
	}
	else if(rv_x>35&&rv_x<90&&rv_y>180&&rv_y<235)  //last page
	{		
		current_button_id=eBT_FILE_LAST;
	}
	else if(rv_x>150&&rv_x<205&&rv_y>180&&rv_y<235)  	//next page
	{
		current_button_id=eBT_FILE_NEXT;
	}
	else if(rv_x>255&&rv_x<310&&rv_y>30&&rv_y<85)	//open file or folder	
	{							
		current_button_id=eBT_FILE_OPEN;
	}
	else if(rv_x>255&&rv_x<310&&rv_y>105&&rv_y<160)	//return parent dir
	{	
		current_button_id=eBT_FILE_FOLDER;
	}
}

// /***************************Extrude page*******************************************/
void display_image::displayWindowExtrude(void)
{
	lcdClear(White);
	LCD_Fill(0, 0, 320, 24, BG_COLOR_CAPTION_EXTRUDE); 	//caption background
	#ifndef Chinese
		displayImage(115, 5, IMG_ADDR_CAPTION_EXTRUDE);		//caption words
	#else
		displayImage(115, 5, IMG_ADDR_CAPTION_EXTRUDE_CN);		//caption words
	#endif
	displayImage(5, 34, IMG_ADDR_BUTTON_ADD);
	displayImage(5, 176, IMG_ADDR_BUTTON_SUB);
	displayImage(15, 95, IMG_ADDR_BUTTON_BED_OFF);

	displayImage(86, 34, IMG_ADDR_BUTTON_PLUS_E);
	displayImage(86, 166, IMG_ADDR_BUTTON_MINUS_E);	
	displayImage(167, 44, IMG_ADDR_BUTTON_FEED_IN_0);
	displayImage(167, 166, IMG_ADDR_BUTTON_FEED_OUT_0);
	default_move_distance = 10;
	initialMoveDistance(260, 41);
	#ifndef Chinese
		displayImage(260, 101, IMG_ADDR_BUTTON_FEED_STOP);
	#else
		displayImage(260, 101, IMG_ADDR_BUTTON_FEED_STOP_CN);
	#endif
	displayImage(260, 176, IMG_ADDR_BUTTON_RETURN);
	POINT_COLOR = 0xC229;
	LCD_DrawRectangle(96, 121, 134, 140);	//jog frame //97  126
	POINT_COLOR = BLUE;	
	LCD_DrawRectangle(180, 121, 219, 140);	//auto frame
	POINT_COLOR=BLACK;
	CLEAN_STRING(s_text);
	sprintf((char*)s_text,"%s",TXT_MENU_EXTRUDE_MANUAL);
	LCD_ShowString(100,123,s_text);	
	sprintf((char*)s_text,"%s",TXT_MENU_EXTRUDE_AUTOMATIC);
	LCD_ShowString(184,123,s_text);
	dispalyExtrudeTemp();
}

void display_image::dispalyExtrudeTemp(void)
{
	LCD_Fill(5,143,65,163,White);		//clean extruder/bed temperature display zone
	POINT_COLOR=BLACK;
	CLEAN_STRING(s_text);
	if(!is_bed_select)
		sprintf((char *)s_text,"%d/%d",(int16_t)thermalManager.temp_hotend[0].celsius, thermalManager.temp_hotend[0].target);

	else{
		sprintf((char *)s_text,"%d/%d", (int16_t)thermalManager.temp_bed.celsius,thermalManager.temp_bed.target);
	}
	LCD_ShowString(8,143,s_text);

}

void display_image::displayRunningAutoFeed(void)
{
	if(dir_auto_feed==0) return;
	static bool is_display_run_auto_feed = false;
	if(!is_display_run_auto_feed)
	{		
		if(dir_auto_feed == 1)
		{			
			LCD_Fill(167,96,234,99,White);	 //clean partial feed in display zone		
			displayImage(167, 41, IMG_ADDR_BUTTON_FEED_IN_1);
		}
		else
		{		
			LCD_Fill(167,166,234,169,White); //clean partial feed out display zone			
			displayImage(167, 169, IMG_ADDR_BUTTON_FEED_OUT_1);	
		}
	}
	else
	{
		if(dir_auto_feed == 1)
		{		
			LCD_Fill(167,41,234,44,White);	 //clean partial feed in display zone					
			displayImage(167, 44, IMG_ADDR_BUTTON_FEED_IN_0);
		}
		else
		{
			LCD_Fill(167,221,234,224,White); //clean partial feed out display zone	
			displayImage(167, 166, IMG_ADDR_BUTTON_FEED_OUT_0);

		}
	}
	is_display_run_auto_feed = !is_display_run_auto_feed;
}

void display_image::scanWindowExtrude( uint16_t rv_x, uint16_t rv_y )
{
	if(rv_x>260&&rv_x<315&&rv_y>176&&rv_y<226)  //return home
	{
		// if(extrude2file)
		// {
		// 	extrude2file=false;
		// 	next_window_ID=eMENU_FILE1;
		// 	return;
		// }
		if(is_printing)
			next_window_ID=eMENU_ADJUST_MORE;
		else
			next_window_ID=eMENU_HOME;
		
	}
	else if(rv_x>5&&rv_x<60&&rv_y>34&&rv_y<89) //add extruder/bed temperature
	{				
		current_button_id=eBT_TEMP_PLUS;
	}
	else if(rv_x>5&&rv_x<60&&rv_y>176&&rv_y<231)   //subtract extruder/bed temperature
	{				
		current_button_id=eBT_TEMP_MINUS;
	}
	else if(rv_x>15&&rv_x<65&&rv_y>95&&rv_y<136)   //select bed/extruder
	{				
			current_button_id=eBT_BED_E;		
	}
	else if(rv_x>85&&rv_x<140&&rv_y>35&&rv_y<100)  //+e move
	{	
		current_button_id=eBT_JOG_EPLUS;
	}
	else if(rv_x>85&&rv_x<140&&rv_y>165&&rv_y<230)  //-e move
	{		
		current_button_id=eBT_JOG_EMINUS;
	}
	else if(rv_x>167&&rv_x<237&&rv_y>45&&rv_y<100)  //autofeed in positive direction 
	{			
		current_button_id=eBT_AUTO_EPLUS;
	}
	else if(rv_x>167&&rv_x<237&&rv_y>165&&rv_y<220) //autofeed in negative direction 
	{	
		current_button_id=eBT_AUTO_EMINUS;		
	}
	else if(rv_x>260&&rv_x<315&&rv_y>40&&rv_y<80)  //change distance
	{	
		current_button_id=eBT_DISTANCE_CHANGE;			
	}
	else if(rv_x>260&&rv_x<315&&rv_y>100&&rv_y<155) //stop autofeed
	{	
		current_button_id=eBT_STOP;		
	}
}

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


/********************************************************
 * is_bed:false->extruder0, true->bed
 * sign:  false->plus, true->minus 
 * return:  false->no change
 *********************************************************/
bool display_image::setTemperatureInWindow(bool is_bed, bool sign)
{
	if((is_bed&&thermalManager.temp_bed.celsius<0)||
	(thermalManager.temp_hotend[0].celsius<0))
		return false; 
	int16_t temp_limit;
    int16_t *p_temp;
    if(!sign)
	{      /* add */
        if(!is_bed){     /* extruder */
            temp_limit = MAX_ADJUST_TEMP_EXTRUDE;
            p_temp = &thermalManager.temp_hotend[0].target;
        }
        else{           /* bed */
            temp_limit = MAX_ADJUST_TEMP_BED;
            p_temp = &thermalManager.temp_bed.target;   
        }
        if(*p_temp < temp_limit){ /* within the limit */
            if(default_move_distance == 0xff)
                if(!is_bed){
                    if(*p_temp < NORMAL_ADJUST_TEMP_EXTRUDE)
                        *p_temp = NORMAL_ADJUST_TEMP_EXTRUDE;
                    else                     
                        *p_temp = MAX_ADJUST_TEMP_EXTRUDE;
                }
                else{
                    if(*p_temp < NORMAL_ADJUST_TEMP_BED)
                        *p_temp = NORMAL_ADJUST_TEMP_BED;
                    else                     
                        *p_temp = MAX_ADJUST_TEMP_BED;
                }
            else{   /* if distance is 1, 5, 10 */
                *p_temp += default_move_distance;
                if(*p_temp > temp_limit)
                    *p_temp= temp_limit; 
            }
            return true;
        }   
    }
	 else
	 {       /* minus */
        if(!is_bed){    /* extruder */
            temp_limit = MIN_ADJUST_TEMP_EXTRUDE; 
            p_temp = &thermalManager.temp_hotend[0].target;
        }
        else    {       /* bed */
            temp_limit = MIN_ADJUST_TEMP_BED;
            p_temp = &thermalManager.temp_bed.target;   
        }
        if(*p_temp > temp_limit){ /* within the limit */
            if(default_move_distance == 0xff)
                if(!is_bed){
                    if(*p_temp <= NORMAL_ADJUST_TEMP_EXTRUDE)
                        *p_temp = MIN_ADJUST_TEMP_EXTRUDE;
                    else                     
                        *p_temp = NORMAL_ADJUST_TEMP_EXTRUDE;
                }
                else{
                    if(*p_temp <= NORMAL_ADJUST_TEMP_BED)
                        *p_temp = MIN_ADJUST_TEMP_BED;
                    else                     
                        *p_temp = NORMAL_ADJUST_TEMP_BED;
                }       
            else{
                *p_temp -= default_move_distance;
                if(*p_temp < temp_limit)
                    *p_temp = temp_limit;  
            }
            return true;
	    }
    }
return false;
}

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
			case eMENU_FILE:
				current_window_ID=eMENU_FILE;
				next_window_ID=eWINDOW_NONE;
				// clearfilevar();
				displayWindowFiles();
			break;
			// case eMENU_FILE1:
			// 	current_window_ID=eMENU_FILE;
			// 	next_window_ID=eWINDOW_NONE;
			// 	displayChosenFile();
			// break;
			case eMENU_EXTRUDE:
				current_window_ID=eMENU_EXTRUDE;
				next_window_ID=eWINDOW_NONE;
				is_bed_select=false;
				displayWindowExtrude();
			break;

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
			default:    // no page change just button press
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
			case eMENU_FILE:
				scanWindowFile(cur_x,cur_y);
				cur_x=cur_y=0;
			break;
			case eMENU_EXTRUDE:
				scanWindowExtrude(cur_x,cur_y);
				cur_x=cur_y=0;
			break;
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

/**
 * process button pressing
 */
void display_image::LGT_Ui_Buttoncmd(void)
{
        DEBUG_ECHOLNPAIR("button id:", current_button_id);
		switch (current_button_id)
		{
            // move buttons
			case eBT_MOVE_X_MINUS:
				current_position[X_AXIS]-=default_move_distance;
				if(is_aixs_homed[X_AXIS]||all_axes_homed())
				{
					if(current_position[X_AXIS]<0)
					current_position[X_AXIS]=0;
				}
				LGT_Line_To_Current_Position(X_AXIS);
				displayMoveCoordinate();
				current_button_id=eBT_BUTTON_NONE;
			break;
			case eBT_MOVE_X_PLUS:
				current_position[X_AXIS]+=default_move_distance;
				if(current_position[X_AXIS]>X_BED_SIZE)
					current_position[X_AXIS]=X_BED_SIZE;
				LGT_Line_To_Current_Position(X_AXIS);
				displayMoveCoordinate();
				current_button_id=eBT_BUTTON_NONE;
			break;
			case eBT_MOVE_X_HOME:
				enqueue_and_echo_commands_P(PSTR("G28 X0"));
				current_button_id=eBT_BUTTON_NONE;
				is_aixs_homed[X_AXIS]=true;
			break;
			case eBT_MOVE_Y_MINUS:
				current_position[Y_AXIS]-=default_move_distance;
				if(is_aixs_homed[Y_AXIS]||all_axes_homed())
				{
					if(current_position[Y_AXIS]<0)
						current_position[Y_AXIS]=0;
				}
				LGT_Line_To_Current_Position(Y_AXIS);
				displayMoveCoordinate();
				current_button_id=eBT_BUTTON_NONE;
			break;
			case eBT_MOVE_Y_PLUS:
				current_position[Y_AXIS]+=default_move_distance;
				if(current_position[Y_AXIS]>Y_BED_SIZE)
					current_position[Y_AXIS]=Y_BED_SIZE;
				LGT_Line_To_Current_Position(Y_AXIS);
				displayMoveCoordinate();
				current_button_id=eBT_BUTTON_NONE;
			break;
			case eBT_MOVE_Y_HOME:
				enqueue_and_echo_commands_P(PSTR("G28 Y0"));
				current_button_id=eBT_BUTTON_NONE;
				is_aixs_homed[Y_AXIS]=true;
			break;
			case eBT_MOVE_Z_MINUS:
				current_position[Z_AXIS]-=default_move_distance;
				if(is_aixs_homed[Z_AXIS]||all_axes_homed())
				{
					if(current_position[Z_AXIS]<0)
						current_position[Z_AXIS]=0;
				}
				LGT_Line_To_Current_Position(Z_AXIS);
				displayMoveCoordinate();
				current_button_id=eBT_BUTTON_NONE;
			break;
			case eBT_MOVE_Z_PLUS:
				current_position[Z_AXIS]+=default_move_distance;
				if(current_position[Z_AXIS]>Z_MACHINE_MAX)
					current_position[Z_AXIS]=Z_MACHINE_MAX;
				LGT_Line_To_Current_Position(Z_AXIS);
				displayMoveCoordinate();
				current_button_id=eBT_BUTTON_NONE;
			break;
			case eBT_MOVE_Z_HOME:
				enqueue_and_echo_commands_P(PSTR("G28 Z0"));
				current_button_id=eBT_BUTTON_NONE;
				is_aixs_homed[Z_AXIS]=true;
			break;
			case eBT_MOVE_ALL_HOME:
				enqueue_and_echo_commands_P(PSTR("G28"));
				current_button_id=eBT_BUTTON_NONE;
			break;

	
		// 	case eBT_MOVE_L0:
		// 		if(!all_axes_homed())
		// 		{
		// 			enqueue_and_echo_commands_P(PSTR("G28"));
		// 			thermalManager.setTargetHotend(0, 0);
		// 			thermalManager.setTargetBed(0);
		// 		}
		// 		enqueue_and_echo_commands_P(PSTR("G0 Z10 F500"));
		// 		#if defined(LK1) || defined(U20)
		// 			enqueue_and_echo_commands_P(PSTR("G0 X50 Y250 F5000"));
		// 		#elif defined(LK2) || defined(LK4) || defined(U30)
		// 			enqueue_and_echo_commands_P(PSTR("G0 X50 Y170 F5000"));
		// 		#elif  defined(LK1_PLUS) ||  defined(U20_PLUS) 
		// 			enqueue_and_echo_commands_P(PSTR("G0 X50 Y350 F5000"));
		// 		#endif
		// 	//	enqueue_and_echo_commands_P(PSTR("G0 X50 Y170 F5000"));
		// 		enqueue_and_echo_commands_P(PSTR("G0 Z0 F300"));
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_MOVE_L1:
		// 		if(!all_axes_homed())
		// 		{
		// 			enqueue_and_echo_commands_P(PSTR("G28"));
		// 			thermalManager.setTargetHotend(0, 0);
		// 			thermalManager.setTargetBed(0);
		// 		}
		// 		enqueue_and_echo_commands_P(PSTR("G0 Z10 F500"));
		// 		#if defined(LK1) || defined(U20)
		// 			enqueue_and_echo_commands_P(PSTR("G0 X250 Y250 F5000"));
		// 		#elif defined(LK2) || defined(LK4) || defined(U30)
		// 			enqueue_and_echo_commands_P(PSTR("G0 X170 Y170 F5000"));
		// 		#elif  defined(LK1_PLUS) ||  defined(U20_PLUS) 
		// 			enqueue_and_echo_commands_P(PSTR("G0 X350 Y350 F5000"));
		// 		#endif
		// 	//	enqueue_and_echo_commands_P(PSTR("G0 X170 Y170 F5000"));
		// 		enqueue_and_echo_commands_P(PSTR("G0 Z0 F300"));
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_MOVE_L2:
		// 		if(!all_axes_homed())
		// 		{
		// 			enqueue_and_echo_commands_P(PSTR("G28"));
		// 			thermalManager.setTargetHotend(0, 0);
		// 			thermalManager.setTargetBed(0);
		// 		}
		// 		enqueue_and_echo_commands_P(PSTR("G0 Z10 F500"));
		// 		#if defined(LK1) || defined(U20)
		// 			enqueue_and_echo_commands_P(PSTR("G0 X250 Y50 F5000"));
		// 	#elif defined(LK2) || defined(LK4) || defined(U30)
		// 			enqueue_and_echo_commands_P(PSTR("G0 X170 Y50 F5000"));
		// 		#elif  defined(LK1_PLUS) ||  defined(U20_PLUS) 
		// 			enqueue_and_echo_commands_P(PSTR("G0 X350 Y50 F5000"));
		// 		#endif
		// //		enqueue_and_echo_commands_P(PSTR("G0 X170 Y50 F5000"));
		// 		enqueue_and_echo_commands_P(PSTR("G0 Z0 F300"));
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_MOVE_L3:
		// 		if(!all_axes_homed())
		// 		{
		// 			enqueue_and_echo_commands_P(PSTR("G28"));
		// 			thermalManager.setTargetHotend(0, 0);
		// 			thermalManager.setTargetBed(0);
		// 		}
		// 		enqueue_and_echo_commands_P(PSTR("G0 Z10 F500"));
		// 		#if defined(LK1) || defined(U20)
		// 			enqueue_and_echo_commands_P(PSTR("G0 X50 Y50 F5000"));
		// 		#elif defined(LK2) || defined(LK4) || defined(U30)
		// 			enqueue_and_echo_commands_P(PSTR("G0 X50 Y50 F5000"));
		// 		#elif  defined(LK1_PLUS) ||  defined(U20_PLUS) 
		// 			enqueue_and_echo_commands_P(PSTR("G0 X50 Y50 F5000"));
		// 		#endif
		// //		enqueue_and_echo_commands_P(PSTR("G0 X50 Y50 F5000"));
		// 		enqueue_and_echo_commands_P(PSTR("G0 Z0 F300"));
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_MOVE_L4:
		// 		if(!all_axes_homed())
		// 		{
		// 			enqueue_and_echo_commands_P(PSTR("G28"));
		// 			thermalManager.setTargetHotend(0, 0);
		// 			thermalManager.setTargetBed(0);
		// 		}
		// 		enqueue_and_echo_commands_P(PSTR("G0 Z10 F500"));
		// 		#if defined(LK1) || defined(U20)
		// 			enqueue_and_echo_commands_P(PSTR("G0 X150 Y150 F5000"));
		// 		#elif defined(LK2) || defined(LK4) || defined(U30)
		// 			enqueue_and_echo_commands_P(PSTR("G0 X110 Y110 F5000"));
		// 		#elif  defined(LK1_PLUS) ||  defined(U20_PLUS) 
		// 			enqueue_and_echo_commands_P(PSTR("G0 X200 Y200 F5000"));
		// 		#endif
		// //		enqueue_and_echo_commands_P(PSTR("G0 X110 Y110 F5000"));
		// 		enqueue_and_echo_commands_P(PSTR("G0 Z0 F300"));
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	// case eBT_MOVE_RETURN:
		// 	// 	enqueue_and_echo_commands_P(PSTR("G0 Z10 F500"));
		// 	// 	current_button_id=eBT_BUTTON_NONE;
		// 	// break;
		// 	case eBT_PR_PLA:
		// 		current_button_id=eBT_BUTTON_NONE;
		// 		if(thermalManager.temp_hotend[0].celsius<0||thermalManager.temp_bed.current<0)
		// 			break;
		// 		thermalManager.temp_hotend[0].target=PREHEAT_PLA_TEMP_EXTRUDE;
		// 		thermalManager.start_watching_hotend(0);
		// 		thermalManager.temp_bed.target=PREHEAT_PLA_TEMP_BED;
		// 		thermalManager.start_watching_bed();
		// 		updatePreheatingTemp();
		// 	//	current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_PR_ABS:
		// 		current_button_id=eBT_BUTTON_NONE;
		// 		if(thermalManager.temp_hotend[0].celsius<0||thermalManager.temp_bed.current<0)
		// 			break;
		// 		thermalManager.temp_hotend[0].target=PREHEAT_ABS_TEMP_EXTRUDE;
		// 		thermalManager.start_watching_hotend(0);
		// 		thermalManager.temp_bed.target=PREHEAT_ABS_TEMP_BED;
		// 		thermalManager.start_watching_bed();
		// 		updatePreheatingTemp();
		// 	//	current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_PR_PETG:
		// 		current_button_id=eBT_BUTTON_NONE;
		// 		if(thermalManager.temp_hotend[0].celsius<0||thermalManager.temp_bed.current<0)
		// 			break;
		// 		thermalManager.temp_hotend[0].target=PREHEAT_PETG_TEMP_EXTRUDE;
		// 		thermalManager.start_watching_hotend(0);
		// 		thermalManager.temp_bed.target=PREHEAT_PETG_TEMP_BED;
		// 		thermalManager.start_watching_bed();
		// 		updatePreheatingTemp();
		// 	//	current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_PR_COOL:
		// 		current_button_id=eBT_BUTTON_NONE;
		// 		if(thermalManager.temp_hotend[0].celsius>0)
		// 			thermalManager.temp_bed.target=MIN_ADJUST_TEMP_BED;
		// 		if(thermalManager.temp_hotend[0].celsius>0)
		// 			thermalManager.temp_hotend[0].target=MIN_ADJUST_TEMP_EXTRUDE;
		// 		updatePreheatingTemp();
		// 	//	current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_PR_E_PLUS:
		// 		if(setTemperatureInWindow(false, false))
		// 		{
		// 			thermalManager.start_watching_hotend(0);
		// 			updatePreheatingTemp();
		// 		}
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_PR_E_MINUS:
		// 		if(setTemperatureInWindow(false, true))
		// 		{
		// 			thermalManager.start_watching_hotend(0);
		// 			updatePreheatingTemp();
		// 		}
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_PR_B_PLUS:
		// 		if(setTemperatureInWindow(true, false))
		// 		{
		// 			thermalManager.start_watching_bed();
		// 			updatePreheatingTemp();
		// 		}
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case EBT_PR_B_MINUS:
		// 		if(setTemperatureInWindow(true, true))
		// 		{
		// 			thermalManager.start_watching_bed();
		// 			updatePreheatingTemp();
		// 		}
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;

		// extrude buttons
			case eBT_TEMP_PLUS:
				if(is_bed_select)   //add bed temperature
				{
					if(setTemperatureInWindow(true,false))
						thermalManager.start_watching_bed();
				}
				else            //add extrude  temprature
				{
					if(setTemperatureInWindow(false,false))
						thermalManager.start_watching_hotend(0);
				}
				dispalyExtrudeTemp();
				current_button_id=eBT_BUTTON_NONE;
			break;
			case eBT_TEMP_MINUS:
				if(is_bed_select)   //subtract bed temperature
				{
					if(setTemperatureInWindow(true,true))
						thermalManager.start_watching_bed();
				}
				else                //subtract extrude temprature
				{
					if(setTemperatureInWindow(false,true))
						thermalManager.start_watching_hotend(0);
				}
				dispalyExtrudeTemp();
				current_button_id=eBT_BUTTON_NONE;
			break;
			case eBT_JOG_EPLUS:
				stopAutoFeed();
				if(thermalManager.temp_hotend[0].target<200)
				{
					thermalManager.temp_hotend[0].target=200;
					thermalManager.start_watching_hotend(0);
					if(thermalManager.temp_hotend[0].celsius>195)
					{
						CLEAN_STRING(s_text);
						sprintf((char*)s_text,PSTR("G0 E%d F60"),default_move_distance);
						enqueue_and_echo_commands_P(PSTR("G91"));
						enqueue_and_echo_commands_P(s_text);
						enqueue_and_echo_commands_P(PSTR("G90"));
					}
					if(is_bed_select)
					{
						is_bed_select=false;
						displayImage(15, 95, IMG_ADDR_BUTTON_BED_OFF);
					}
				}
				else if(thermalManager.temp_hotend[0].celsius>195)
				{
					CLEAN_STRING(s_text);
					sprintf((char*)s_text,PSTR("G0 E%d F60"),default_move_distance);
					enqueue_and_echo_commands_P(PSTR("G91"));
					enqueue_and_echo_commands_P(s_text);
					enqueue_and_echo_commands_P(PSTR("G90"));
					if(is_bed_select)
					{
						is_bed_select=false;
						displayImage(15, 95, IMG_ADDR_BUTTON_BED_OFF);
					}
				}
				current_button_id=eBT_BUTTON_NONE;
			break;
			case eBT_JOG_EMINUS:
				stopAutoFeed();
				if(thermalManager.temp_hotend[0].target<200)
				{
					thermalManager.temp_hotend[0].target=200;
					thermalManager.start_watching_hotend(0);
					if(thermalManager.temp_hotend[0].celsius>195)
					{
						CLEAN_STRING(s_text);
						sprintf((char*)s_text,PSTR("G0 E-%d F60"),default_move_distance);
						enqueue_and_echo_commands_P(PSTR("G91"));
						enqueue_and_echo_commands_P(s_text);
						enqueue_and_echo_commands_P(PSTR("G90"));
					}
					if(is_bed_select)
					{
						is_bed_select=false;
						displayImage(15, 95, IMG_ADDR_BUTTON_BED_OFF);
					}
				}
				else if(thermalManager.temp_hotend[0].celsius>195)
				{
					CLEAN_STRING(s_text);
					sprintf((char*)s_text,PSTR("G0 E-%d F120"),default_move_distance);
					enqueue_and_echo_commands_P(PSTR("G91"));
					enqueue_and_echo_commands_P(s_text);
					enqueue_and_echo_commands_P(PSTR("G90"));
					if(is_bed_select)
					{
						is_bed_select=false;
						displayImage(15, 95, IMG_ADDR_BUTTON_BED_OFF);
					}
				}
				current_button_id=eBT_BUTTON_NONE;
			break;
			case eBT_AUTO_EPLUS:
			   if(dir_auto_feed==-1)
			   		stopAutoFeed();
				startAutoFeed(1);
				current_button_id=eBT_BUTTON_NONE;
			break;
			case eBT_AUTO_EMINUS:
				if(dir_auto_feed==1)
					stopAutoFeed();
				startAutoFeed(-1);
				current_button_id=eBT_BUTTON_NONE;
			break;
			case eBT_STOP:
				stopAutoFeed();
				current_button_id=eBT_BUTTON_NONE;
			break;
			case eBT_BED_E:
				is_bed_select=!is_bed_select;
				if(is_bed_select)  //bed mode
					displayImage(15, 95, IMG_ADDR_BUTTON_BED_ON);			
				else  //extruder mode
					displayImage(15, 95, IMG_ADDR_BUTTON_BED_OFF);	
				current_button_id=eBT_BUTTON_NONE;
				dispalyExtrudeTemp();			
			break;
			case eBT_DISTANCE_CHANGE:
				switch(current_window_ID)
				{
					case eMENU_MOVE:
						changeMoveDistance(260, 55);
					break;
					case eMENU_PREHEAT:
						changeMoveDistance(260,37);
					break;
					case eMENU_EXTRUDE:case eMENU_ADJUST:case eMENU_ADJUST_MORE:
						changeMoveDistance(260,40);
					break;
					case eMENU_SETTINGS2:
					 	changeMoveDistance(255,43);	
					break;
					default:
					break;
				}
				current_button_id=eBT_BUTTON_NONE;
			break;

            // menu file
			case eBT_FILE_NEXT:
				if (lgtCard.nextPage()) {
					LCD_Fill(0, 25, 239, 174,White);	//clean file list display zone 
					displayFileList();
					displayFilePageNumber();
					// if(choose_file_page==page_index&&choose_printfile!=-1)
						// CardFile.ChoseFile(choose_printfile);
				}
				current_button_id=eBT_BUTTON_NONE;
			    break;
			case eBT_FILE_LAST:
				if (lgtCard.previousPage()) {
					LCD_Fill(0, 25, 239, 174,White);
					displayFileList();
					displayFilePageNumber();
					// if(choose_file_page==page_index&&choose_printfile!=-1)
						// CardFile.ChoseFile(choose_printfile);
				}
				current_button_id=eBT_BUTTON_NONE;
			    break;
			case eBT_FILE_LIST1:
				if(current_window_ID==eMENU_FILE) {
                    highlightChosenItem(0);
                } else
					;//ChoseArgument(0);
				current_button_id=eBT_BUTTON_NONE;
			break;
			case eBT_FILE_LIST2:
				if(current_window_ID==eMENU_FILE) {
                    highlightChosenItem(1);
                } else
					;//ChoseArgument(1);	
				current_button_id=eBT_BUTTON_NONE;
			break;
			case eBT_FILE_LIST3:
				if(current_window_ID==eMENU_FILE)
					highlightChosenItem(2);
				else
					;//ChoseArgument(2);
				current_button_id=eBT_BUTTON_NONE;
			break;
			case eBT_FILE_LIST4:
				if(current_window_ID==eMENU_FILE)
					highlightChosenItem(3);
				else
					;//ChoseArgument(3);
				current_button_id=eBT_BUTTON_NONE;
			break;
			case eBT_FILE_LIST5:
				if(current_window_ID==eMENU_FILE)
					highlightChosenItem(4);
				else
					;// ChoseArgument(4);
				current_button_id=eBT_BUTTON_NONE;
			break;
		// 	case eBT_FILE_OPEN:
		// 		current_button_id=eBT_BUTTON_NONE;
		// 		if(choose_printfile==-1||(choose_printfile!=-1&&(choose_file_page!=(page_index)))
		// 			)
		// 		{
		// 			//current_button_id=eBT_BUTTON_NONE;
		// 			break;
		// 		}
		// 		if(list_order)
		// 			card.getfilename(choose_file_page*5+choose_printfile);
		// 		else
		// 			card.getfilename(file_count-(choose_file_page)*5-choose_printfile-1);
		// 		// Serial1.print(CardFile.getsdfilename(choose_file_page*5+choose_printfile));
		// 		 Serial1.println(card.longFilename);
		// 		if(CardFile.filenameIsFolder)
		// 		{
		// 			card.chdir(card.filename);
		// 			if(card.curWorkDirDepth!=MAX_DIR_DEPTH)
		// 			{
		// 				clearfilevar();
		// 				LCD_Fill(0, 25, 239, 174,White);
		// 				if((SDIO_GetCardState()==SDIO_CARD_ERROR))
		// 					displayPromptSDCardError();
		// 				else
		// 				{
		// 					file_count=card.getnrfilenames();
		// 					if(file_count==0)
		// 					{
		// 						displayPromptEmptyFolder();
		// 						return;
		// 					}
		// 					page_index_max=CardFile.getsdfilepage();
		// 					displayFileList();
		// 					displayFilePageNumber();
		// 				}
		// 			}
		// 		}
		// 		else
		// 		{
		// 			uint8_t check_cn=0;
		// 			for(int i=CHECK_FILAMENT_TIMES;i>=0;i--)
		// 			{
		// 				if(READ(FIL_RUNOUT_PIN))
		// 					check_cn++;
		// 			}
		// 			if(!check_filament_disable&&(check_cn>(CHECK_FILAMENT_TIMES-1)))
		// 			{
		// 				dispalyDialogYesNo(eDIALOG_START_JOB_NOFILA);
		// 				current_window_ID=eMENU_DIALOG_NO_FIL;
		// 				extrude2file=true;
		// 			}
		// 			else
		// 			{
		// 				dispalyDialogYesNo(eDIALOG_PRINT_START);
		// 				current_window_ID=eMENU_DIALOG_START;
		// 			}
		// 		}
		// 		//current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_FILE_FOLDER:
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	card.updir();
		// 	if(card.curWorkDirDepth!=MAX_DIR_DEPTH)
		// 	{
		// 		clearfilevar();
		// 		LCD_Fill(0, 25, 239, 174,White);
		// 		if((SDIO_GetCardState()==SDIO_CARD_ERROR))
		// 			displayPromptSDCardError();
		// 		else
		// 		{
		// 			file_count=card.getnrfilenames();
		// 			if(file_count==0)
		// 			{
		// 				displayPromptEmptyFolder();
		// 				return;
		// 			}
		// 			page_index_max=CardFile.getsdfilepage();
		// 			displayFileList();
		// 			displayFilePageNumber();
		// 		}
		// 	}
		// 	break;

		// 	case eBT_PRINT_PAUSE:
		// 		switch(current_print_cmd)
		// 		{
		// 			case E_PRINT_DISPAUSE:
		// 			break;
		// 			case E_PRINT_PAUSE:
		// 				enqueue_and_echo_commands_P((PSTR("M25")));
		// 				LCD_Fill(260,30,320,90,White);		//clean pause/resume icon display zone
		// 				displayImage(260, 30, IMG_ADDR_BUTTON_RESUME);	
		// 				cur_pstatus=2;	
		// 				current_print_cmd=E_PRINT_CMD_NONE;
		// 				displayPause();
		// 			break;
		// 			case E_PRINT_RESUME:
		// 				enqueue_and_echo_commands_P((PSTR("M24")));
		// 				LCD_Fill(260,30,320,90,White);		//clean pause/resume icon display zone
		// 				displayImage(260, 30, IMG_ADDR_BUTTON_PAUSE);	
		// 				cur_pstatus=1;	
		// 				current_print_cmd=E_PRINT_CMD_NONE;
		// 				displayPrinting();
		// 			break;
		// 			default:
		// 			break;
		// 		}
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_PRINT_ADJUST:
		// 		next_window_ID=eMENU_ADJUST;
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_PRINT_END:
		// 		if(is_print_finish)
		// 		{
		// 			clearVarPrintEnd();
		// 			displayWindowHome();
		// 			current_window_ID=eMENU_HOME;
		// 		}
		// 		else
		// 		{
		// 			dispalyDialogYesNo(eDIALOG_PRINT_ABORT);
		// 			current_window_ID=eMENU_DIALOG_END;
		// 		}

		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;

		// 	case eBT_ADJUSTE_PLUS:
		// 		if(current_window_ID==eMENU_ADJUST)   //add e temp
		// 		{
		// 			if(setTemperatureInWindow(false,false))
		// 				dispalyAdjustTemp();
		// 		}
		// 		else     //subtract flow
		// 		{
		// 			planner.flow_percentage[0]+=default_move_distance;
		// 			if(planner.flow_percentage[0]>999)
		// 				planner.flow_percentage[0]=999;
		// 			dispalyAdjustFlow();
		// 		}
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_ADJUSTE_MINUS:
		// 		if(current_window_ID==eMENU_ADJUST)   //subtract e temp
		// 		{
		// 			if(setTemperatureInWindow(false,true))
		// 				dispalyAdjustTemp();
		// 		}
		// 		else     //subtract flow
		// 		{
		// 			planner.flow_percentage[0]-=default_move_distance;
		// 			if(planner.flow_percentage[0]<10)
		// 				planner.flow_percentage[0]=0;
		// 			dispalyAdjustFlow();
		// 		}
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_ADJUSTBED_PLUS:
		// 		if(setTemperatureInWindow(true,false))
		// 			dispalyAdjustTemp();
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_ADJUSTBED_MINUS:
		// 		if(setTemperatureInWindow(true,true))
		// 			dispalyAdjustTemp();
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_ADJUSTFAN_PLUS:
		// 		cur_fanspeed=thermalManager.fan_speed[0];
		// 		cur_fanspeed+=default_move_distance;
		// 		if(cur_fanspeed>255)
		// 			cur_fanspeed=255;
		// 		thermalManager.fan_speed[0]=cur_fanspeed;
		// 		dispalyAdjustFanSpeed();
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_ADJUSTFAN_MINUS:
		// 		cur_fanspeed=thermalManager.fan_speed[0];
		// 		cur_fanspeed-=default_move_distance;
		// 		if(cur_fanspeed<0)
		// 			cur_fanspeed=0;
		// 		thermalManager.fan_speed[0]=cur_fanspeed;
		// 		dispalyAdjustFanSpeed();
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_ADJUSTSPEED_PLUS:
		// //	Serial1.println(default_parameter[0]);
		// 		feedrate_percentage+=default_move_distance;
		// 		if(feedrate_percentage>999)
		// 			feedrate_percentage=999;
		// 		dispalyAdjustMoveSpeed();
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_ADJUSTSPEED_MINUS:
		// 		feedrate_percentage-=default_move_distance;
		// 		if(feedrate_percentage<10)
		// 			feedrate_percentage=10;
		// 		dispalyAdjustMoveSpeed();
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;


		// 	case eBT_DIALOG_PRINT_START:
		// 	 Serial1.println(card.filename);
		// 	if(current_window_ID==eMENU_DIALOG_NO_FIL)
		// 	{
		// 		displayWindowExtrude();
		// 		current_window_ID=eMENU_EXTRUDE;
		// 	}
		// 	else
		// 	{
		// 		is_printing=true;
		// 		is_print_finish=cur_flag=false;
		// 		cur_ppage=0;cur_pstatus=0;
		// 		if(current_window_ID==eMENU_DIALOG_START)
		// 			recovery_flag=false;
		// 		if(recovery_flag==false)
		// 		{
		// 			#if ENABLED(POWER_LOSS_RECOVERY)
        //   				card.removeJobRecoveryFile();
        // 			#endif
		// 		//	card.openAndPrintFile(card.filename);	
		// 			char cmd[4+ strlen(card.filename) + 1];
		// 			sprintf_P(cmd, PSTR("M23 %s"),card.filename);
		// 			enqueue_and_echo_command_now(cmd);
  		// 			enqueue_and_echo_commands_P(PSTR("M24"));
		// 			W25QxxFlash.W25QXX_Write((uint8_t*)card.longFilename,SAVE_FILE_ADDR,(uint16_t)sizeof(card.longFilename));
		// 		}
		// 		else   //recovery
		// 		{
		// 			enqueue_and_echo_commands_P(PSTR("M1000"));
		// 			recovery_flag=false;
		// 			W25QxxFlash.W25QXX_Read((uint8_t*)card.longFilename,SAVE_FILE_ADDR,(uint16_t)sizeof(card.longFilename));
		// 		}
		// 		displayWindowPrint();
		// 		current_window_ID=eMENU_PRINT;
		// 	}
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_DIALOG_PRINT_NO:
		// 		next_window_ID=eMENU_FILE1;
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_DIALOG_REFACTORY_YES:
		// 		current_button_id=eBT_BUTTON_NONE;
		// 		current_window_ID=eMENU_SETTINGS;
		// 		ConfigSettings.restoreDefaultSettings();
		// 		page_index=0;
		// 		is_setting_change=true;
		// 		displayWindowSettings();
		// 	break;
		// 	case eBT_DIALOG_SAVE_YES:
		// 		current_button_id=eBT_BUTTON_NONE;
		// 		FLASH_WRITE_VAR(SAVE_ADDR_SETTINGS, re_settings);
		// 		dispalyDialogYes(eDIALOG_SETTS_SAVE_OK);
		// 		current_window_ID=eMENU_DIALOG_SAVE_OK;
		// 		//ConfigSettings.settingsReset();
		// 		ConfigSettings.settingsLoad();
		// 		is_setting_change=false;
		// 	break;
		// 	case eBT_SETTING_ADJUST:
		// 		current_button_id=eBT_BUTTON_NONE;
		// 		if(choose_setting>=ARGUMENST_MAX_NUM) break;
		// 		current_window_ID=eMENU_SETTINGS2;
		// 		displayWindowSettings2();
		// 		// change_window = true ;
		// 		// highlightSelectedIcon(255,30,55,55);	
		// 		// displayImage(255, 30, pic_address_button_modify);
		// 	break;
		// 	case eBT_SETTING_REFACTORY:
		// 		dispalyDialogYesNo(eDIALOG_SETTS_RESTORE);
		// 		current_window_ID=eMENU_DIALOG_REFACTORY;
		// 		//ConfigSettings.restoreDefaultSettings();
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_SETTING_SAVE:
		// 		current_button_id=eBT_BUTTON_NONE;
		// 		dispalyDialogYesNo(eDIALOG_SETTS_SAVE);
		// 		current_window_ID=eMENU_DIALOG_SAVE;
		// 	break;
		// 	case eBT_SETTING_LAST:
		// 		if(page_index > 0)
		// 		{
		// 			page_index = page_index - 1;
		// 		//	choose_printfile=0;
		// 			LCD_Fill(0, 25, 239, 174,White);	//clean file list display zone 
		// 			displayArgumentList();
		// 			displayArugumentPageNumber();	
		// 			displayImage(5, 180, IMG_ADDR_BUTTON_PAGE_LAST);
		// 		}
		// 		 choose_setting=ARGUMENST_MAX_NUM;
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_SETTING_NEXT:
		// 		if(page_index < ARGUMENST_MAX_PAGE)
		// 		{
		// 			page_index = page_index + 1;
		// 		//	choose_printfile=0; 	 
		// 			LCD_Fill(0, 25, 239, 174,White);	//clean file list display zone 
		// 			displayArgumentList();
		// 			displayArugumentPageNumber();
		// 			displayImage(101, 180, IMG_ADDR_BUTTON_PAGE_NEXT);
		// 		}
		// 		 choose_setting=ARGUMENST_MAX_NUM;
		// 		current_button_id=eBT_BUTTON_NONE;
		// 	break;
		// 	case eBT_SETTING_ADD:
		// 		current_button_id=eBT_BUTTON_NONE;
		// 		changeValueArgument(choose_setting, 1);
		// 		displayModifyArgument();
		// 		is_setting_change=true;
		// 	break;
		// 	case eBT_SETTING_SUB:
		// 		current_button_id=eBT_BUTTON_NONE;
		// 		changeValueArgument(choose_setting, -1);
		// 		displayModifyArgument();
		// 		is_setting_change=true;
		// 	break;
			case eBT_BUTTON_NONE:
			default:
				current_button_id=eBT_BUTTON_NONE;
			break;
		}
}

void display_image::LGT_Printer_Data_Update(void)
{
	constexpr millis_t UPDATE_INTERVAL = 1000u;
	static millis_t next_update_Time = 0;
	const millis_t now = millis();
	if(ELAPSED(now, next_update_Time)){
		next_update_Time = UPDATE_INTERVAL + millis();
		// checkTemprature();
		switch (current_window_ID) {
			// case eMENU_HOME:
			// 	SDIO_Init();
			// 	switch(SDIO_GetCardState())	
			// 	{
			// 		case SDIO_CARD_ERROR:
			// 			if(sd_insert)
			// 			{
			// 				sd_insert=false;
			// 			}
			// 		break;
			// 		default:
			// 			if(!sd_insert)
			// 			{
			// 				card.initsd();
			// 				sd_insert=true;
			// 			}
			// 		break;
			// 	}
			// break;
			case eMENU_MOVE:
				displayMoveCoordinate();
			break;
			// case eMENU_PRINT:
			// 	displayPrintInformation();
			// break;
			// case eMENU_ADJUST:
			// 	dispalyAdjustFanSpeed(); 
			// 	dispalyAdjustTemp(); 	
			// 	dispalyAdjustMoveSpeed();
			// 	if(thermalManager.fan_speed[0]>0)
			// 	{
			// 		displayRunningFan(144, 105);	
			// 	}
			// 	// if(cur_pstatus==3)
			// 	// 	cur_ppage=10;
			// 	switch(cur_pstatus)   //save current status page when in adjust page 
			// 	{
			// 		case 0:
			// 			cur_ppage=0;
			// 		break;
			// 		case 1:
			// 			cur_ppage=1;
			// 		break;
			// 		case 2:
			// 			cur_ppage=2;
			// 		break;
			// 		case 3:
			// 			cur_ppage=10;
			// 		break;
			// 		default:
			// 		break;
			// 	}
			// break;
			// case eMENU_ADJUST_MORE:
			// 	dispalyAdjustFlow();
			// 	switch(cur_pstatus)   //save current status page when in adjust page 
			// 	{
			// 		case 0:
			// 			cur_ppage=0;
			// 		break;
			// 		case 1:
			// 			cur_ppage=1;
			// 		break;
			// 		case 2:
			// 			cur_ppage=2;
			// 		break;
			// 		case 3:
			// 			cur_ppage=10;
			// 		break;
			// 		default:
			// 		break;
			// 	}
			// break;
			// case eMENU_PREHEAT:
			// 	updatePreheatingTemp();
			// break;
			case eMENU_EXTRUDE:
				dispalyExtrudeTemp();
				actAutoFeed();
				displayRunningAutoFeed();
			break;
			// case eMENU_FILE:
			// 	SDIO_Init();
			// 	switch(SDIO_GetCardState())	
			// 	{
			// 		case SDIO_CARD_ERROR:
			// 			if(sd_insert)
			// 			{
			// 				sd_insert=false;
			// 				displayPromptSDCardError();
			// 			}
			// 		break;
			// 		default:
			// 			if(!sd_insert)
			// 			{
			// 				card.initsd();
			// 				file_count=CardFile.getsdfilecount();
			// 				page_index_max=CardFile.getsdfilepage();
			// 				LCD_Fill(0, 25, 239, 174,White);	//clean  
			// 				displayFileList();
			// 				displayFilePageNumber();
			// 				sd_insert=true;
			// 			}
			// 		break;
			// 	}
			// break;
		// case eMENU_DIALOG_ERRORTEMPBED:
		// 	if((thermalManager.temp_bed.current>MIN_ADJUST_TEMP_BED)&&(thermalManager.temp_bed.current<MAX_ADJUST_TEMP_BED))
		// 	{
		// 		displayWindowHome();
		// 		current_window_ID=eMENU_HOME;
		// 		check_temp_Bed=0;     //checkTemprature();
		// 	}
		// break;
		// case eMENU_DIALOG_ERRORTEMPE:
		// 	if((thermalManager.temp_hotend[0].celsius>MIN_ADJUST_TEMP_EXTRUDE)&&(thermalManager.temp_hotend[0].celsius<MAX_ADJUST_TEMP_EXTRUDE))
		// 	{
		// 		displayWindowHome();
		// 		current_window_ID=eMENU_HOME;
		// 		check_temp_E=0;    
		// 	}
		// break;
			default:
				break;
		}
	}
}

void LgtLcdTft::init()
{
    // init tft-lcd
    lcd.init();
    lcd.clear();
    // touch.calibrate();
    displayStartUpLogo();
    delay(10);
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
        if (!TRUELY_TOUCHED()) {
            const millis_t time = millis();
            if (ELAPSED(time, nextTouchReadTime)) {
                nextTouchReadTime = time + TOUCH_DELAY;
                touchCheck++;
                if (TRUELY_TOUCHED()) {  // truely touched
                    touch.readTouchPoint(cur_x, cur_y);
                    DEBUG_ECHOPAIR("touch: x: ", cur_x);
                    DEBUG_ECHOLNPAIR(", y: ", cur_y);
                    LGT_MainScanWindow();   // touch pos will be clear after scanning
                }
            }
        }
    } else if (TRUELY_TOUCHED()) {  // touch released
        touchCheck = 0;
        DEBUG_ECHOLN("touch: released ");
        if(LGT_Ui_Update())
            LGT_Ui_Buttoncmd();

    } else {    // idle
        touchCheck = 0;
		LGT_Printer_Data_Update();
    }

}

#endif