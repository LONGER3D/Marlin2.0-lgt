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
        m_currentItem(0),
        m_currentFile(0)
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

    inline uint8_t nextPage()
    {
        if (m_currentPage < m_pageCount - 1) {
            m_currentPage++;
            return 1;
        } else {
            return 0;
        }

    }

    inline uint8_t previousPage()
    {
        if (m_currentPage > 0) {
            m_currentPage--;
            return 1;
        } else {
            return 0;
        }
    }

    bool setItem(uint16_t item); // page_index_num

    inline uint16_t item() {return m_currentItem;}

    inline bool isReverseList() {return m_isReverseList;}

    bool isDir();
    const char *filename(uint16_t i);
    const char *filename();
    inline uint16_t fileIndex() {return m_currentFile;} // get current selected file

    inline bool isFileSelected() {return m_isSelectFile;}
    uint16_t selectedPage();



private:
    uint16_t m_fileCount;
    uint16_t m_pageCount;
    uint16_t m_currentPage;
    uint16_t m_currentItem;
    uint16_t  m_currentFile;

    bool m_isReverseList = false;
    bool m_isSelectFile = false;

    // uint16_t page_index_max;
    // uint16_t page_index;
    // uint16_t file_count;
    // uint16_t choose_file_page;

};

extern LgtSdCard lgtCard;
