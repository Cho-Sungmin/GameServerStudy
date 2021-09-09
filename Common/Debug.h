#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <time.h>
#include <fstream>
#include "Header.h"
#include <cassert>
#include <mutex>

#define __DEBUG__

using namespace std;

class LOG
{
    fstream m_file;
    string m_fileName = "";
    mutex m_mutex;
    static LOG *m_pInstance;

    LOG(const string &_fileName)
    {
        m_fileName = _fileName;
        assert(m_fileName != "");

        if (!m_file.is_open())
            m_file.open(m_fileName + "_" + getCurrentDate() + ".txt", ios::out | ios::app);
    }

public:
    enum TYPE
    {
        RECV = 0,
        SEND = 1
    };

    ~LOG()
    {
        if (m_file.is_open())
            m_file.close();
    }

    static LOG *getInstance(const string &fileName = "")
    {
        if (m_pInstance == nullptr)
            m_pInstance = new LOG(fileName);

        return m_pInstance;
    }

    //--- For all ---//
    void printLOG(const string &tagStr, const string &stateStr, const string &logStr);
    void writeLOG(const string &tagStr, const string &stateStr, const string &logStr);
    //--- For TCP ---//
    void printLOG(InputByteStream &packet, int type);
    void writeLOG(InputByteStream &packet, int type);

    void close();
    //static void sighandler(int sig);

private:
    string getHeaderString(const Header &header);
    const string getLog(InputByteStream &packet);
    string getHexaString(const char *bytes, int size);
    string getCurrentTime();
    string getCurrentDate();
    const string getTypeString(int8_t type);
    const string getFuncString(int16_t func);
    void nolocks_localtime(struct tm *tmp, time_t t, time_t tz, int dst);
    int is_leap_year(int year);
};

#endif