#pragma once
#include <stdint.h>

#define LIST_ITEM_MAX 5u
#define LIST_PAGE_MAX

class LgtSdCard {
public:

    LgtSdCard() :
        m_fileCount(0),
        m_pageCount(0),
        m_currentPage(0),
        m_currentItem(0)
    {

    }

    uint16_t count();
    void clear();

    inline uint16_t fileCount() // file_count
    {
        return m_fileCount;
    }
    
    inline uint16_t pageCount() // page_index_max                 
    {
        return m_pageCount;
    }

    inline void setPage(uint16_t page) // page_index
    {
        if (page < m_pageCount)
            m_currentPage = page;
    }

    inline int16_t page(void)
    {
        return m_currentPage;
    }

    inline void setItem(uint16_t item) // page_index_num
    {
        if (item < LIST_ITEM_MAX)
            m_currentItem = item;
    }

    bool isDir();
    const char *filename(uint16_t i);


private:
    uint16_t m_fileCount;
    uint16_t m_pageCount;
    uint16_t m_currentPage;
    uint16_t m_currentItem;
    // uint16_t page_index_max;
    // uint16_t page_index;
    // uint16_t file_count;
    // uint16_t choose_file_page;

};

extern LgtSdCard lgtCard;
