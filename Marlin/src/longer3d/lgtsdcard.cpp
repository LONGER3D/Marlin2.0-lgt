#include "../inc/MarlinConfigPre.h"

#if ENABLED(LGT_LCD_TFT)
#include "lgtsdcard.h"
#include "../sd/cardreader.h"
#include "../module/printcounter.h"

LgtSdCard lgtCard;

LgtSdCard::LgtSdCard()
{
    clear();
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

void LgtSdCard::clear()
{
    m_fileCount = 0;
    m_pageCount = 0;

    m_currentPage = 0;
    m_currentItem = 0;
    m_currentFile = 0;

    m_isReverseList = false;
    m_isSelectFile = false;
}

bool LgtSdCard::isDir()
{
    return card.flag.filenameIsDir;
}

const char *LgtSdCard::shortFilename()
{
    if (m_fileCount == 0 || !m_isSelectFile)
        return nullptr;
    card.getfilename_sorted(m_currentFile);
    return card.filename;
}

const char *LgtSdCard::filename(uint16_t i)
{
    if (m_fileCount == 0)
        return nullptr;
    card.getfilename_sorted(i);
    char *fn = card.longFilename[0] ? card.longFilename : card.filename;
    uint16_t len = strlen(fn);
    if (len > 25) {
        const char *s = "...\0\0";
        strncpy_P(fn + 22, s, sizeof(s));
        // fn[25] = '\0';
        // fn[26] = '\0';
    }
    return fn;
}

/**
 * get selected longfilename if any
 */
const char *LgtSdCard::filename()
{
    if (m_isSelectFile) {
        return filename(m_currentFile);
    } else {
        return nullptr;
    }
}

uint8_t LgtSdCard::setItem(uint16_t item)
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

uint8_t LgtSdCard::dirDepth()
{
    return card.getDirDepth();
}

bool LgtSdCard::isMaxDirDepth()
{
    return (dirDepth() >= MAX_DIR_DEPTH);
}

bool LgtSdCard::isRootDir()
{
    return card.flag.workDirIsRoot;
}

void LgtSdCard::changeDir(const char *relpath)
{
    card.cd(relpath);
}

int8_t LgtSdCard::upDir()
{
    return card.cdup();
}

void LgtSdCard::upTime(char *p)
{
    uint16_t h, m, s;
	h = print_job_timer.duration() / 3600;
	m = print_job_timer.duration() % 3600 / 60;
	s = print_job_timer.duration() % 60;
    sprintf(p,"%02d:%02d:%02d", h, m, s);
}

void LgtSdCard::downTime(char *p)
{
    // uint32_t Elapsed_time;
    // if(print_times==0)  return;
    // Elapsed_time= print_times*(1.0 - card.percentfloatDone());
    // Remaining_hours=Elapsed_time/60;
    // Remaining_minutes=Elapsed_time %60;
    
}

#endif // LGT_LCD_TFT
