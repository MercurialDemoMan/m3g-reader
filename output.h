//color codes for unix terminal

#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>

//console font effects
#define FNT_RESET   "\033[0m"

//clear console
#define my_clear() printf("\033[H\033[J")

//normal font
#define FNT_BLACK   "\033[0;30m"
#define FNT_RED     "\033[0;31m"
#define FNT_GREEN   "\033[0;32m"
#define FNT_YELLOW  "\033[0;33m"
#define FNT_BLUE    "\033[0;34m"
#define FNT_PURPLE  "\033[0;35m"
#define FNT_CYAN    "\033[0;36m"
#define FNT_WHITE   "\033[0;37m"

//bold font
#define FNT_BBLACK  "\033[1;30m"
#define FNT_BRED    "\033[1;31m"
#define FNT_BGREEN  "\033[1;32m"
#define FNT_BYELLOW "\033[1;33m"
#define FNT_BBLUE   "\033[1;34m"
#define FNT_BPURPLE "\033[1;35m"
#define FNT_BCYAN   "\033[1;36m"
#define FNT_BWHITE  "\033[1;37m"

//underline font
#define FNT_UBLACK  "\033[4;30m"
#define FNT_URED    "\033[4;31m"
#define FNT_UGREEN  "\033[4;32m"
#define FNT_UYELLOW "\033[4;33m"
#define FNT_UBLUE   "\033[4;34m"
#define FNT_UPURPLE "\033[4;35m"
#define FNT_UCYAN   "\033[4;36m"
#define FNT_UWHITE  "\033[4;37m"

//background font
#define FNTB_BLACK  "\033[40m"
#define FNTB_RED    "\033[41m"
#define FNTB_GREEN  "\033[42m"
#define FNTB_YELLOW "\033[43m"
#define FNTB_BLUE   "\033[44m"
#define FNTB_PURPLE "\033[45m"
#define FNTB_CYAN   "\033[46m"
#define FNTB_WHITE  "\033[47m"

//high intensity normal font
#define FNT_IBLACK   "\033[0;90m"
#define FNT_IRED     "\033[0;91m"
#define FNT_IGREEN   "\033[0;92m"
#define FNT_IYELLOW  "\033[0;93m"
#define FNT_IBLUE    "\033[0;94m"
#define FNT_IPURPLE  "\033[0;95m"
#define FNT_ICYAN    "\033[0;96m"
#define FNT_IWHITE   "\033[0;97m"

//high intensity bold font
#define FNT_BIBLACK  "\033[1;90m"
#define FNT_BIRED    "\033[1;91m"
#define FNT_BIGREEN  "\033[1;92m"
#define FNT_BIYELLOW "\033[1;93m"
#define FNT_BIBLUE   "\033[1;94m"
#define FNT_BIPURPLE "\033[1;95m"
#define FNT_BICYAN   "\033[1;96m"
#define FNT_BIWHITE  "\033[1;97m"

//high intensity background font
#define FNTBI_BLACK   "\033[0;100m"
#define FNTBI_RED     "\033[0;101m"
#define FNTBI_GREEN   "\033[0;102m"
#define FNTBI_YELLOW  "\033[0;103m"
#define FNTBI_BLUE    "\033[0;104m"
#define FNTBI_PURPLE  "\033[0;105m"
#define FNTBI_CYAN    "\033[0;106m"
#define FNTBI_WHITE   "\033[0;107m"

#endif
