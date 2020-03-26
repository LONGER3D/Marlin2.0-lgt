#include "lgtdwlcd.h"

#if ENABLED(LGT_LCD_DW)

#include "lgtdwdef.h"

#include "../module/temperature.h"
#include "../sd/cardreader.h"
#include "../module/motion.h"
#include "../module/planner.h"

// debug define
#define DEBUG_LGTDWLCD
#define DEBUG_OUT ENABLED(DEBUG_LGTDWLCD)
#include "../core/debug_out.h"

LGT_SCR LGT_LCD;
DATA Rec_Data;
DATA Send_Data;
// duration_t Duration_Time;
unsigned char data_storage[DATA_SIZE];
char total_time[7];         //Total print time for each model 
uint32_t total_print_time = 0;
char printer_work_time[31];  //Total work time of printers
int gcode_id[FILE_LIST_NUM], sel_fileid =-1;
int gcode_num=0;
millis_t recovery_time=0;
uint8_t recovery_percent = 0;
float level_z_height = 0.0;
float recovery_z_height = 0.0,recovery_E_len=0.0;
float resume_x_position=0.0,resume_y_position=0.0,resume_e_position= 0.0;
bool sd_init_flag = true;
bool tartemp_flag = false;
bool LGT_is_printing = false;
bool LGT_stop_printing = false;
bool return_home = false;
#ifdef U20_Pro
	bool led_on = true;
#endif // U20_Pro
bool xy_home = false;
#ifdef U30_Pro
	bool xyz_home = false,z_home=false;
#endif
bool leveling_wait = false;
int re_count = 0;
E_MENU_TYPE menu_type= eMENU_IDLE;
PRINTER_STATUS status_type= PRINTER_SETUP;
PRINTER_KILL_STATUS kill_type = PRINTER_NORMAL;

static char fila_type = 0;  // 0 refer to PLA, 1 refer to ABS
// 0 no check, 1 check PLA, 2 check ABS, used for "no enough temp" dialog in fila [OK] return
char menu_fila_type_chk = 0;
// 0 for 10 mm, 1 for 1 mm, 2 for 0.1 mm, used for "no enough temp" dialog in move [OK] return
char menu_move_dis_chk = 0;
static char menu_measu_dis_chk = 1;	//step 1 to 1mm and step 2 to 0.1mm
static char menu_measu_step = 0;	// 0 for not start, 1 for step 1, 2 for step 2, 3 for step 3

char cmd_E[16] = { 0 };
unsigned int filament_len = 10;
unsigned int filament_temp = 200;

#define MYSERIAL1 MSerial2


LGT_SCR::LGT_SCR()
{
	memset(data_storage, 0, sizeof(data_storage));
	memset(&Rec_Data,0,sizeof(Rec_Data));
	memset(total_time, 0, sizeof(total_time));
	memset(printer_work_time, 0, sizeof(printer_work_time));
	Rec_Data.head[0]  = DW_FH_0;
	Rec_Data.head[1]  = DW_FH_1;
	Send_Data.head[0] = DW_FH_0;
	Send_Data.head[1] = DW_FH_1;
}

void LGT_SCR::begin()
{

    MYSERIAL1.begin(115200);
    delay(600); 
    status_type = PRINTER_SETUP;
    #if ENABLED(POWER_LOSS_RECOVERY)
        // check_print_job_recovery();
    #endif
    DEBUG_PRINT_P("dw begin");
    // LGT_LCD.LGT_Change_Page(ID_MENU_HOME);

}


bool check_recovery = false;
// char leveling_sta = 0;
int ii_setup = 0;
void LGT_SCR::LGT_LCD_startup_settings()
{

    
    if (ii_setup < STARTUP_COUNTER)
    {
        if (ii_setup >= (STARTUP_COUNTER-1000))
        {
            tartemp_flag = true;
            if (card.isMounted())
                DEBUG_PRINT_P("sd ok");//LGT_LCD.LGT_Display_Filename();
            if (check_recovery == false)
            {
                DEBUG_PRINT_P("got go home");
                menu_type = eMENU_HOME;
                LGT_LCD.LGT_Change_Page(ID_MENU_HOME);
            }
            else
            {
                return_home = true;
                check_recovery = false;
                // enable_Z();

                LGT_LCD.LGT_Change_Page(ID_DIALOG_PRINT_RECOVERY);
            }
            LGT_LCD.LGT_Printer_Data_Updata();
            LGT_LCD.LGT_DW_Setup(); //about machine
            ii_setup = STARTUP_COUNTER;
        }
        ii_setup++;
    }
    if (LGT_stop_printing == true)
    {
        LGT_stop_printing = false;
        //LGT_LCD.LGT_Stop_Printing();
    }
}


void LGT_SCR::LGT_Main_Function()
{
    static millis_t Next_Temp_Time = 0;
	LGT_Get_MYSERIAL1_Cmd();
	if (millis() >= Next_Temp_Time)
	{
		Next_Temp_Time += 2000;
		if (tartemp_flag == true)  //M104/M109/M190/140
		{
			tartemp_flag = false;
			if(LGT_is_printing==false)
				status_type = PRINTER_HEAT;
			// LGT_Send_Data_To_Screen(ADDR_VAL_TAR_E, thermalManager.target_temperature[0]);
			// LGT_Send_Data_To_Screen(ADDR_VAL_TAR_B, thermalManager.target_temperature_bed);
			LGT_Send_Data_To_Screen(ADDR_VAL_TAR_E, thermalManager.degTargetHotend(0));
			LGT_Send_Data_To_Screen(ADDR_VAL_TAR_B, thermalManager.degTargetBed());            
		}
		LGT_Printer_Data_Updata();
		LGT_Get_MYSERIAL1_Cmd();
	}
	#ifdef U20_Pro
		if (led_on == true)
			LGT_Printer_Light_Update();
	#endif // U20_Pro
	//LGT_SDCard_Status_Update();
}

/*************************************
FUNCTION:	Getting and saving commands of MYSERIAL1(DWIN_Screen)
**************************************/
void LGT_SCR::LGT_Get_MYSERIAL1_Cmd()
{
	memset(data_storage, 0, sizeof(data_storage));
	while (re_count<DATA_SIZE &&MYSERIAL1.available() > 0)
	{
		data_storage[re_count] = MYSERIAL1.read();
		if (data_storage[0] != DW_FH_0)
		{
			continue;
		}
		delay(5);
		re_count++;
	}
	if (re_count < 1)    //null
		return;
	else if (re_count >= 2 && (Rec_Data.head[0] == data_storage[0]) && (Rec_Data.head[1] == data_storage[1]))
	{
		Rec_Data.cmd = data_storage[3];
		if (Rec_Data.cmd == DW_CMD_VAR_R)
		{
			Rec_Data.addr = data_storage[4];
			Rec_Data.addr = (Rec_Data.addr << 8) | data_storage[5];
			Rec_Data.datalen = data_storage[6];
			for (int i = 0; i < Rec_Data.datalen; i = i + 2)
			{
				Rec_Data.data[i / 2] = data_storage[7 + i];
				Rec_Data.data[i / 2] = (Rec_Data.data[i / 2] << 8) | data_storage[8 + i];
			}
			LGT_Analysis_DWIN_Screen_Cmd();
			re_count = 0;
			memset(data_storage, 0, sizeof(data_storage));
			memset(&Rec_Data,0,sizeof(Rec_Data));
			Rec_Data.head[0] = DW_FH_0;
			Rec_Data.head[1] = DW_FH_1;
		}
		else
		{
			re_count = 0;
			memset(data_storage, 0, sizeof(data_storage));
		}
	}
	else
	{
		re_count = 0;
		memset(data_storage, 0, sizeof(data_storage));
	}
}

void LGT_SCR::LGT_Change_Page(unsigned int pageid)
{
	memset(data_storage, 0, sizeof(data_storage));
    // unsigned char data_storage[10];
	data_storage[0] = DW_FH_0;
	data_storage[1] = DW_FH_1;
	data_storage[2] = 0x07;
	data_storage[3] = DW_CMD_VAR_W;
	data_storage[4] = 0x00;
	data_storage[5] = 0x84;
	data_storage[6] = 0x5A;
	data_storage[7] = 0x01;
	data_storage[8] = (unsigned char)(pageid >> 8) & 0xFF;
	data_storage[9] = (unsigned char)(pageid & 0x00FF);
	for (int i = 0; i < 10; i++)
		MYSERIAL1.write(data_storage[i]);
}

void LGT_SCR::LGT_Send_Data_To_Screen(uint16_t Addr, int16_t Num)
{
	    memset(data_storage, 0, sizeof(data_storage));
		data_storage[0] = Send_Data.head[0];
		data_storage[1] = Send_Data.head[1];
		data_storage[2] = 0x05;
		data_storage[3] = DW_CMD_VAR_W;
		data_storage[4] = (unsigned char)(Addr>> 8); //(Num >> 8) & 0xFF
		data_storage[5] = (unsigned char)(Addr & 0x00FF);
		data_storage[6] = (unsigned char)(Num >> 8);
		data_storage[7] = (unsigned char)(Num & 0x00FF);
		for (int i = 0; i < 8; i++)
		{
			MYSERIAL1.write(data_storage[i]);
			delayMicroseconds(1);
		}
}


void LGT_SCR::LGT_Send_Data_To_Screen(unsigned int addr, char* buf)
{
	memset(data_storage, 0, sizeof(data_storage));
	data_storage[0] = Send_Data.head[0];
	data_storage[1] = Send_Data.head[1];
	data_storage[2] = 0x0A;
	data_storage[3] = DW_CMD_VAR_W;
	data_storage[4] = (unsigned char)(addr >> 8);
	data_storage[5] = (unsigned char)(addr & 0x00FF);
	for (int i = 0; i < 7; i++)
	{
		data_storage[6 + i] = buf[i];
	}
	for (int i = 0; i < 13; i++)
	{
		MYSERIAL1.write(data_storage[i]);
		delayMicroseconds(1);
	}
}
void LGT_SCR::LGT_Send_Data_To_Screen1(unsigned int addr,const char* buf)
{
	memset(data_storage, 0, sizeof(data_storage));
	data_storage[0] = Send_Data.head[0];
	data_storage[1] = Send_Data.head[1];
	data_storage[2] = 0x22;
	data_storage[3] = DW_CMD_VAR_W;
	data_storage[4] = (unsigned char)(addr >> 8);
	data_storage[5] = (unsigned char)(addr & 0x00FF);
	for (int i = 0; i < 31; i++)
	{
		data_storage[6 + i] = buf[i];
	}
	for (int i = 0; i < 37; i++)

	{
		MYSERIAL1.write(data_storage[i]);
		delayMicroseconds(1);
	}
}


void LGT_SCR::LGT_Printer_Data_Updata()
{
	uint8_t progress_percent = 0;
	uint16_t LGT_feedrate = 0;
	switch (menu_type)
	{
	case eMENU_HOME:
		LGT_Send_Data_To_Screen(ADDR_VAL_CUR_E, (int16_t)thermalManager.degHotend(0));
		LGT_Send_Data_To_Screen(ADDR_VAL_CUR_B, (int16_t)thermalManager.degBed());
        // DEBUG_PRINT_P("home temp. update");
		break;
	case eMENU_TUNE:
		LGT_Send_Data_To_Screen(ADDR_VAL_CUR_E, (int16_t)thermalManager.degHotend(0));
		LGT_Send_Data_To_Screen(ADDR_VAL_CUR_B, (int16_t)thermalManager.degBed());
		LGT_Get_MYSERIAL1_Cmd();
		LGT_Send_Data_To_Screen(ADDR_VAL_FAN,thermalManager.scaledFanSpeed(0));
		LGT_Send_Data_To_Screen(ADDR_VAL_FEED,feedrate_percentage);
		LGT_Send_Data_To_Screen(ADDR_VAL_FLOW,planner.flow_percentage[0]);
		break;
	// case eMENU_MOVE:
	// 	LGT_Send_Data_To_Screen(ADDR_VAL_MOVE_POS_X, (int16_t)(current_position[X_AXIS] * 10));
	// 	LGT_Send_Data_To_Screen(ADDR_VAL_MOVE_POS_Y, (int16_t)(current_position[Y_AXIS] * 10));
	// 	LGT_Send_Data_To_Screen(ADDR_VAL_MOVE_POS_Z, (int16_t)(current_position[Z_AXIS] * 10));
	// 	LGT_Send_Data_To_Screen(ADDR_VAL_MOVE_POS_E, (int16_t)(current_position[E_AXIS] * 10));
	// 	break;
	// case eMENU_TUNE_E:
	// 	LGT_Send_Data_To_Screen(ADDR_VAL_CUR_E, (int16_t)thermalManager.current_temperature[0]);
	// 	break;
	// case eMENU_TUNE_B:
	// 	LGT_Send_Data_To_Screen(ADDR_VAL_CUR_B, (int16_t)thermalManager.current_temperature_bed);
	// 	break;
	// case eMENU_TUNE_FAN:
	// 	LGT_Send_Data_To_Screen(ADDR_VAL_FAN,fanSpeeds[0]);
	// 	break;
	// case eMENU_TUNE_SPEED:
	// 	LGT_Send_Data_To_Screen(ADDR_VAL_FEED,feedrate_percentage);
	// 	LGT_feedrate = (uint16_t)(MMS_SCALED(feedrate_mm_s) * 10);
	// 	if (LGT_feedrate > 3000)
	// 		LGT_feedrate = 3000;
	// 	LGT_Send_Data_To_Screen(ADDR_VAL_CUR_FEED, LGT_feedrate);
	// 	break;
	// case eMENU_TUNE_FLOW:
	// 	LGT_Send_Data_To_Screen(ADDR_VAL_FLOW,planner.flow_percentage[0]);
	// 	break;
	// case eMENU_UTILI_FILA:
	// case eMENU_HOME_FILA:
	// 	LGT_Send_Data_To_Screen(ADDR_VAL_CUR_E, (int16_t)thermalManager.current_temperature[0]);
	// 	LGT_Send_Data_To_Screen(ADDR_VAL_CUR_B, (int16_t)thermalManager.current_temperature_bed);
	// 	break;
	// case eMENU_PRINT_HOME:
	// 	progress_percent = card.percentDone();
	// 	if(progress_percent>0)
	// 		LGT_Send_Data_To_Screen(ADDR_VAL_HOME_PROGRESS, (uint16_t)progress_percent);
	// 	else
	// 		LGT_Send_Data_To_Screen(ADDR_VAL_HOME_PROGRESS, (uint16_t)recovery_percent);

	// 	Duration_Time = (print_job_timer.duration()) + recovery_time;
	// 	Duration_Time.toDigital(total_time);
	// 	LGT_Send_Data_To_Screen(ADDR_TXT_HOME_ELAP_TIME,total_time);
	// 	//delay(10);
	// 	LGT_Send_Data_To_Screen(ADDR_VAL_HOME_Z_HEIGHT, (int16_t)((current_position[Z_AXIS] + recovery_z_height) * 10));  //Current Z height
	// 	LGT_Send_Data_To_Screen(ADDR_VAL_CUR_E, (int16_t)thermalManager.current_temperature[0]);
	// 	LGT_Send_Data_To_Screen(ADDR_VAL_CUR_B, (int16_t)thermalManager.current_temperature_bed);
	// 	break;
	default:
		break;
	}
}

void LGT_SCR::LGT_DW_Setup()
{
	// if (eeprom_read_byte((const uint8_t*)(EEPROM_INDEX + 5)) != 0)
	// {
	// 	eeprom_write_dword((uint32_t*)EEPROM_INDEX, 0);
	// 	eeprom_write_byte((uint8_t *)(EEPROM_INDEX + 5), 0);
	// }
	// total_print_time = eeprom_read_dword((const uint32_t*)EEPROM_INDEX);

	LGT_Send_Data_To_Screen1(ADDR_TXT_ABOUT_MODEL, MAC_MODEL);
	LGT_Send_Data_To_Screen1(ADDR_TXT_ABOUT_SIZE, MAC_SIZE);
	LGT_Send_Data_To_Screen1(ADDR_TXT_ABOUT_FW_BOARD, BOARD_FW_VER);
}

#endif // LGT_LCD_DW