#pragma once
#include <stdint.h>

#define LIST_ITEM_MAX 5u
#define GCOMMENT_SIZE 64

class LgtSdCard {
public:

    LgtSdCard();

    uint16_t count();
    void clear();

    inline uint16_t fileCount() { return m_fileCount; }
    
    inline uint16_t pageCount() { return m_pageCount; }

    inline void setPage(uint16_t page) 
    {
        if (page < m_pageCount)
            m_currentPage = page;
    }

    inline int16_t page(void) { return m_currentPage; }

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

    uint8_t setItem(uint16_t item); // page_index_num

    inline uint16_t item() {return m_currentItem;}

    inline bool isReverseList() {return m_isReverseList;}

    bool isDir();
    const char *shortFilename();
    const char *filename(uint16_t i);
    const char *filename();
    inline uint16_t fileIndex() {return m_currentFile;} // get current selected file

    inline bool isFileSelected() {return m_isSelectFile;}
    uint16_t selectedPage();

    uint8_t dirDepth();
    void changeDir(const char *relpath);
    int8_t upDir();
    bool isMaxDirDepth();
    bool isRootDir();

    void downTime(char *);
    void upTime(char *);

    inline void writeComment(char c)
    {
        if (indexGc < GCOMMENT_SIZE)
            gComment[indexGc++] = c;
    }

    inline void endComment()
    {
        gComment[indexGc] = '\0';
        indexGc = 0;
    }

    void parseComment();

    void setPrintTime(uint16_t t) { m_printTime = t; }
    uint16_t printTime() { return m_printTime; }

private:
    void parseCura();
    void parseLegacyCura();

private:
    uint16_t m_fileCount;
    uint16_t m_pageCount;
    uint16_t m_currentPage;     // current page
    uint16_t m_currentItem;     // select item index
    uint16_t  m_currentFile;    // select file index

    bool m_isReverseList;   // if reverse list
    bool m_isSelectFile;    // if select file

    static char gComment[GCOMMENT_SIZE];
    static uint8_t indexGc;

    uint16_t m_printTime;   // minute unit

};

extern LgtSdCard lgtCard;
