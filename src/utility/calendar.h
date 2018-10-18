#ifndef CORONET_CALENDAR_H
#define CORONET_CALENDAR_H

#include <string>

namespace srlib {
  struct Calendar {
    static int day_each_month[13];
    int year;
    int month;
    int day;
    Calendar() : year(1970), month(1), day(1) {}
    Calendar(int y, int m, int d) : year(y), month(m), day(d) {}
    Calendar(const std::string &str) {
      sscanf(str.c_str(), "%d-%02d-%02d", &year, &month, &day);
    }
    std::string toString() const {
      char buf[16];
      sprintf(buf, "%d-%02d-%02d", year, month, day);
      return std::string(buf);
    }
    bool operator==(const Calendar &rhs) const {
      return year == rhs.year && month == rhs.month && day == rhs.day;
    }
    bool operator!=(const Calendar &rhs) const {
      return !(rhs == *this);
    }
    Calendar &operator++() {
      day++;
      if (day > day_each_month[month]) {
        // Not Leap Month
        if (month != 2 || (year % 4 != 0 || (year % 100 == 0 && year % 400 != 0))) {
          month++, day = 1;
          if (month > 12) {
            year++, month = 1;
          }
        } else {
          // Leap Month
          if (day > 29)month++, day = 1;
        }
      }
      return *this;
    }
    Calendar &operator++(int) {
      return ++(*this);
    }
    Calendar &operator--() {
      day--;
      if (day < 1) {
        --month;
        if (month < 1) {
          --year, month = 12;
        }
        day = day_each_month[month];
        if (month == 2 && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) {
          day++;
        }
      }
      return *this;
    }
    Calendar &operator--(int) {
      return --(*this);
    }
  };
  
  int Calendar::day_each_month[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
}

#endif
