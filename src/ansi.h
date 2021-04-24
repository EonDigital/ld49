#ifndef ANSI_H_
#define ANSI_H_

#define COLOR( c ) "\x1b[3" #c "m"
#define C_RESET "\x1b[0m"

#define C_BLACK   COLOR(0)
#define C_RED     COLOR(1)
#define C_GREEN   COLOR(2)
#define C_YELLOW  COLOR(3)
#define C_BLUE    COLOR(4)
#define C_MAGENTA COLOR(5)
#define C_CYAN    COLOR(6)
#define C_WHITE   COLOR(7)

#endif
