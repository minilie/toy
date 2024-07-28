/*************************************************************************
	> File Name: Log.h
	> Author:Xin 
	> Mail:2923262959@qq.com 
	> Created Time: Sun 28 Jul 2024 06:31:59 PM CST
 ************************************************************************/

#ifndef _LOG_H
#define _LOG_H

#define BEGINS(x) namespace x { // start of namespace x
#define ENDS(x) } // end of namespace x

#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <thread>
#include <unordered_map>
#include <sstream>

BEGINS(toy)
class Logger;
class LoggerStream;
class BaseInfo;
class Info;
class Debug;
class Warn;
class Error;

class BaseInfo {
public :
    class Ivisitor  {
    public :
        virtual void visit(BaseInfo *, std::ostringstream &) = 0;
        virtual void visit(Info *, std::ostringstream &) = 0;
        virtual void visit(Debug *, std::ostringstream &) = 0;
        virtual void visit(Warn *, std::ostringstream &) = 0;
        virtual void visit(Error *, std::ostringstream &) = 0;
    };
    virtual void Accept(Ivisitor *vis, std::ostringstream &a) {
        vis->visit(this, a);
        return ;
    }
    virtual ~BaseInfo() {}
};

class Info : public BaseInfo {
public :
    void Accept(Ivisitor *vis, std::ostringstream &a) override {
        vis->visit(this, a);
        return ;
    }
};

class Debug : public BaseInfo {
public :
    void Accept(Ivisitor *vis, std::ostringstream &a) override {
        vis->visit(this, a);
        return ;
    }
};

class Warn : public BaseInfo {
public :
    void Accept(Ivisitor *vis, std::ostringstream &a) override {
        vis->visit(this, a);
        return ;
    }
};

class Error : public BaseInfo {
public :
    void Accept(Ivisitor *vis, std::ostringstream &a) override {
        vis->visit(this, a);
        return ;
    }
};

class Logger {
    class LoggerStream : public std::ostringstream {
    public :
        class OutPutVisitor : public BaseInfo::Ivisitor {
        public :
            virtual void visit(BaseInfo *, std::ostringstream &a) {}
            virtual void visit(Info *, std::ostringstream &a) {
                a << "\033[34;1m [Info] \033[0m";
            }
            virtual void visit(Debug *, std::ostringstream &a) {
                a << "\033[32;1m [Debug] \033[0m";
            }
            virtual void visit(Warn *, std::ostringstream &a) {
                a << "\033[33;1m [Warn] \033[0m";
            }
            virtual void visit(Error *, std::ostringstream &a) {
                a << "\033[31;1m [Error] \033[0m";
            }
        };
        LoggerStream(Logger &raw_obj, const char *filename, const char *funcname, int linenumber, BaseInfo *base) :
        raw_obj(raw_obj), linenumber(linenumber), Lvisitor(new OutPutVisitor()) {
            std::ostringstream &now = *this;
            now << "[" << filename << " project]";
            now << " [" << funcname << "] ";
            base->Accept(this->Lvisitor, *this);
        }
        LoggerStream(const LoggerStream &ls) : raw_obj(ls.raw_obj) {}
        ~LoggerStream() {
            output();
        }
    private :
        void output() {
            std::unique_lock<std::mutex> lock(raw_obj.m_mutex);
            std::ostringstream &now = *this;
            now << " (" << linenumber << ")" << std::endl;
            std::cout << this->str();
            std::cout.flush();
            return ;
        }
        Logger::LoggerStream::OutPutVisitor *Lvisitor;
        Logger &raw_obj;
        int linenumber;
    };
public :
    LoggerStream operator()(const char *filename, const char *funcname, int linenumber, BaseInfo *base) {
        return LoggerStream(*this, filename, funcname, linenumber, base);
    }
private :
    std::mutex m_mutex;
};

Logger toy_log;
#define TLOG toy_log(__FILE__, __func__, __LINE__, new BaseInfo())
#define __TLOG(a) toy_log(__FILE__, __func__, __LINE__, a)
#define TLOG_Info __TLOG(new Info())
#define TLOG_Debug __TLOG(new Debug())
#define TLOG_Warn __TLOG(new Warn())
#define TLOG_Error __TLOG(new Error())

ENDS(toy)
#endif
