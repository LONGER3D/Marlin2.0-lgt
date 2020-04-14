#include "../inc/MarlinConfigPre.h"

#if ENABLED(LGT_LCD_TFT)
#include "lgtsdcard.h"
#include "../sd/cardreader.h"

LgtSdCard lgtCard;

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
}

bool LgtSdCard::isDir()
{
    return card.flag.filenameIsDir;
}

const char *LgtSdCard::filename(uint16_t i)
{
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


#endif // LGT_LCD_TFT
