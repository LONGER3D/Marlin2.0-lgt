#include "../inc/MarlinConfigPre.h"

#if ENABLED(LGT_LCD_TFT)
#include "lgtsdcard.h"
#include "../sd/cardreader.h"

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
    m_currentFile;

    m_isReverseList = false;
    m_isSelectFile = false;
}

bool LgtSdCard::isDir()
{
    return card.flag.filenameIsDir;
}

const char *LgtSdCard::filename(uint16_t i)
{
    if (m_fileCount == 0)
        return nullptr;
     card.getfilename_sorted(i);
     return card.longFilename[0] ? card.longFilename : card.filename;
    //  uint16_t len = card.filename();
    // filenameIsFolde=DIR_IS_SUBDIR(card.filename);
    // if(filenameIsFolder)
    // {
    //     return card.filename;
    // }
    // else
    // {
    //     uint8_t filenamelen=strlen(card.longFilename);
    //     if(filenamelen>25)
    //     {
    //         card.longFilename[25]='\0';
    //         card.longFilename[26]='\0';
    //     }
    //     return card.longFilename;
    // }
}

const char *LgtSdCard::filename()
{
    if (m_isSelectFile) {
        return filename(m_currentFile);
    } else {
        return nullptr;
    }
}

bool LgtSdCard::setItem(uint16_t item)
{
    if (item < LIST_ITEM_MAX) {
        if (m_isReverseList) {
            uint16_t n = m_currentPage * LIST_ITEM_MAX + item + 1;
            if ( n <= m_fileCount) {
                m_currentItem = item;
                m_currentFile =  m_fileCount - n;
                m_isSelectFile = true;
                return true;   // success to set
            }
        } else {
            uint16_t n = m_currentPage * LIST_ITEM_MAX + item;
            if (n < m_fileCount) {
                m_currentItem = item;
                m_currentFile = n;
                m_isSelectFile = true;
                return true;   // success to set
            }
        }
    }
    return false;   // fail to set
}

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

#endif // LGT_LCD_TFT
