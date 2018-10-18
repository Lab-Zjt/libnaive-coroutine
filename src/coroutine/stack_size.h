#ifndef CORONET_STACK_SIZE_H
#define CORONET_STACK_SIZE_H
// Stack Size Literal
constexpr size_t operator "" GB(unsigned long long size) {return size * 1024 * 1024 * 1024;}
constexpr size_t operator "" MB(unsigned long long size) {return size * 1024 * 1024;}
constexpr size_t operator "" KB(unsigned long long size) {return size * 1024;}
constexpr size_t operator "" B(unsigned long long size) {return size;}
#endif
