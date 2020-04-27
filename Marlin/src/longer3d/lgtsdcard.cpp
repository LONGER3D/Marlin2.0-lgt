// #include "../inc/MarlinConfigPre.h"
#include "lgtsdcard.h"

#if ENABLED(LGT_LCD_TFT)
#include "../module/printcounter.h"
#include "../HAL/STM32F1/sdio.h"
#include "lgtstore.h"

LgtSdCard lgtCard;

// init class static variable
char LgtSdCard::gComment[GCOMMENT_SIZE];
uint8_t LgtSdCard::indexGc = 0;

// define static variable and function
static char *CodePointer = nullptr;

static inline bool codeSeen(char *p,char code)
{
    CodePointer = strchr(p,code);
    return(CodePointer != nullptr);
}

static inline uint16_t codeValue()
{
    return (strtoul(CodePointer + 1, nullptr, 10));
}

static inline uint16_t codeValue2()
{
    return (strtoul(CodePointer + 17, nullptr, 10));
}

// class function definition
LgtSdCard::LgtSdCard()
{
    clear();
    m_printTime = 0;
    ZERO(parentSelectFile);
}

uint16_t LgtSdCard::count()
{
    if (card.isMounted()) {
        m_fileCount = card.get_num_Files();
        m_pageCount = m_fileCount / LIST_ITEM_MAX;
        if(m_fileCount % LIST_ITEM_MAX > 0)
            m_pageCount++;
    } else {
        m_fileCount = 0;
        m_pageCount = 0;
    }
    return m_fileCount;
}

/**
 * clear part of file variable when init card
 */
void LgtSdCard::_clear()
{
    m_fileCount = 0;
    m_pageCount = 0;

    m_currentPage = 0;
    m_currentItem = 0;
    m_currentFile = 0;

    m_isSelectFile = false;   
}

/**
 * clear all file variable when init card
 */
void LgtSdCard::clear()
{
    _clear();

    indexGc = 0;
    
    m_dirDepth = 0;
    
}

bool LgtSdCard::isDir()
{
    return card.flag.filenameIsDir;
}

/**
 * filename for open file
 */
const char *LgtSdCard::shortFilename()
{
    if (!m_isSelectFile)
        return nullptr;
    return card.filename;
}

/**
 * longfilename for showing
 */
const char *LgtSdCard::longFilename()
{
    // if (!m_isSelectFile)
        // return nullptr;
    return card.longFilename;
}

/**
 * get trimmed longfilename(no more than 25 char) for showing in lcd
 */
const char *LgtSdCard::filename(uint16_t i)
{
    #define MAX_TRIM_FILENAME_LEN 25
    if (m_fileCount == 0)
        return nullptr;
    card.getfilename_sorted(i);
    char *fn = card.longFilename[0] ? card.longFilename : card.filename;
    uint16_t len = strlen(fn);
    if (len > MAX_TRIM_FILENAME_LEN) {
        // const char *suffix = "...";
        // strncpy_P(fn + MAX_TRIM_FILENAME_LEN - 3, suffix, sizeof(suffix));
        fn[MAX_TRIM_FILENAME_LEN - 3] = '.';
        fn[MAX_TRIM_FILENAME_LEN - 2] = '.';
        fn[MAX_TRIM_FILENAME_LEN - 1] = '.';       
        fn[MAX_TRIM_FILENAME_LEN    ] = '\0';
        fn[MAX_TRIM_FILENAME_LEN + 1] = '\0';
    }
    return fn;
}

/**
 * get trimmed longfilename if select a file
 */
const char *LgtSdCard::filename()
{
    if (m_isSelectFile) {
        return filename(m_currentFile);
    } else {
        return nullptr;
    }
}

/**
 * try to select a file in file list
 * return 0: failed to set when exceed file index scope
 */
uint8_t LgtSdCard::selectFile(uint16_t item)
{
    if (item < LIST_ITEM_MAX) {
        if (m_isReverseList) {
            uint16_t n = m_currentPage * LIST_ITEM_MAX + item + 1;
            if ( n <= m_fileCount) {
                m_currentItem = item;
                m_currentFile =  m_fileCount - n;
                m_isSelectFile = true;
                return 1;   // success to set
            }
        } else {
            uint16_t n = m_currentPage * LIST_ITEM_MAX + item;
            if (n < m_fileCount) {
                m_currentItem = item;
                m_currentFile = n;
                m_isSelectFile = true;
                return 1;   // success to set
            }
        }
    }
    return 0;   // fail to set
}

/**
 * get the page of selected file
 */
uint16_t LgtSdCard::selectedPage()
{
    if (!m_isSelectFile)
        return 0;
    if (m_isReverseList) {
        return (m_fileCount - 1 - m_currentFile) / LIST_ITEM_MAX;
    } else {
        return m_currentFile / LIST_ITEM_MAX;
    }   
}

/**
 * get the page of selected file
 */
uint16_t LgtSdCard::selectedItem()
{
    if (!m_isSelectFile)
        return 0;
    if (m_isReverseList) {
        return (m_fileCount - 1 - m_currentFile) % LIST_ITEM_MAX;
    } else {
        return m_currentFile % LIST_ITEM_MAX;
    }   
}

bool LgtSdCard::isMaxDirDepth()
{
    return (dirDepth() >= MAX_DIR_DEPTH);
}

bool LgtSdCard::isRootDir()
{
    return dirDepth() == 0;/* card.flag.workDirIsRoot */; // 
}

void LgtSdCard::changeDir(const char *relpath)
{
    card.cd(relpath);
}

int8_t LgtSdCard::upDir()
{
    return card.cdup();
}

/**
 * get up time form lcd
 */
void LgtSdCard::upTime(char *p)
{
    uint16_t h, m, s;
	h = print_job_timer.duration() / 3600;
	m = print_job_timer.duration() % 3600 / 60;
	s = print_job_timer.duration() % 60;
    sprintf(p,"%02d:%02d:%02d", h, m, s);
}

/** 
 * get down time for lcd
 */
void LgtSdCard::downTime(char *p)
{
    if (m_printTime == 0)
        return;
    uint16_t remain, h, m;
    remain= uint16(ceil(float(m_printTime) * card.ratioNotDone()));
    h = remain / 60;
    m = remain % 60;
    // SERIAL_ECHOLNPAIR_F("remain ratio: ", card.ratioNotDone());
    // SERIAL_ECHOLNPAIR("remain: ", remain);
    sprintf(p, "%d H %d M", h, m);
}

/**
 * parse and get total print time from gcode comment
 */
void LgtSdCard::parseComment()
{
    if (strstr(gComment, "TIME:") != nullptr) {
        SERIAL_ECHOLNPAIR("comment:", gComment);
        parseCura();
        lgtStore.saveRecovery();
    }
    else if(strstr(gComment,"Print time") != nullptr)
    {
        SERIAL_ECHOLNPAIR("comment:", gComment);
        parseLegacyCura();
        lgtStore.saveRecovery();
    }
}

/**
 * parse cura(after v2.0) gcode comment
 */
void LgtSdCard::parseCura()
{
	uint32_t second = 0;
    char *p = strchr(gComment, ':');
	if(p != nullptr){
		second = uint32_t(strtol(p + 1, nullptr, 10));
		m_printTime = second / 60;
        SERIAL_ECHOLNPAIR("printTime:", m_printTime);
	}    
}

/**
 * parse old cura(before v2.0) gocde comment
 */
void LgtSdCard::parseLegacyCura()
{
    uint16_t hour = 0;
	uint16_t minute = 0;	
	if (strstr(gComment,"hour") != nullptr) {   // x hour(s)
		if (codeSeen(gComment,':'))
			hour = codeValue();
		if (hour == 1) {
			if (codeSeen(gComment,'r'))  // 1 hour
                minute = codeValue2(); 
        } else if (codeSeen(gComment,'s')) { // x hours	
            minute = codeValue() ; 
        }  
	} else if(codeSeen(gComment,':')) { // minute(s)
        minute = codeValue();
	}
	m_printTime = hour * 60 + minute;
    SERIAL_ECHOLNPAIR("hour:", hour);
    SERIAL_ECHOLNPAIR("minute:", minute);
    SERIAL_ECHOLNPAIR("printTime:", m_printTime);
}

CardUpdate LgtSdCard::update()
{
	SDIO_Init();
	const bool state = (SDIO_GetCardState() != SDIO_CARD_ERROR);    // true: sd ok
	if (state != m_cardState) {
		m_cardState = state;
		if (state) {
			card.mount();
			if (card.isMounted()) {
				SERIAL_ECHOLNPAIR("card mount ok");// ExtUI::onMediaInserted();
				return CardUpdate::MOUNTED;
			} else {
				SERIAL_ECHOLNPAIR("card mount error");// ExtUI::onMediaError();
				return CardUpdate::ERROR;
			}
		} else {
			const bool ok = card.isMounted();
			card.release();
			if (ok) {
				SERIAL_ECHOLNPAIR("card removed");// ExtUI::onMediaRemoved();
                return CardUpdate::REMOVED;
			}
		}
	} 
	return CardUpdate::NOCHANGED;
}


#endif // LGT_LCD_TFT
