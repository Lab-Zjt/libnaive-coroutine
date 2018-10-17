#ifndef CORONET_CORODEF_H
#define CORONET_CORODEF_H

#define MAX_THREAD_NUM 7

#define NORMAL_STACK_SIZE 4096

#define CHECK_ENDING_INTERVAL 50000000 // Unit:ns

// Stack Size Literal
constexpr size_t operator "" GB(unsigned long long size) {return size * 1024 * 1024 * 1024;}
constexpr size_t operator "" MB(unsigned long long size) {return size * 1024 * 1024;}
constexpr size_t operator "" KB(unsigned long long size) {return size * 1024;}
constexpr size_t operator "" B(unsigned long long size) {return size;}

#endif
