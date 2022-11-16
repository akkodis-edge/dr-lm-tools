#ifndef HALLIBTEST_COMMON_H_
#define HALLIBTEST_COMMON_H_


#include <string>


#define P_S(CHOICE, DATA_STRING, FUNC)  printf("  %2d) %-35s: %s\n", menu_[CHOICE].choice, menu_[CHOICE].text.c_str(), DATA_STRING[halLib->FUNC])
#define P_N(CHOICE, DESC)  printf("  %2d) %-35s: %s\n", menu_[CHOICE].choice, menu_[CHOICE].text.c_str(), DESC)


int getChoice(const char *message, char &letter);
void anyKey(void);


const char on_str_[2][20] =
{
        "OFF", "ON"
};


typedef enum
{
    ASCII = -2,
    ESC = -1,
} Choices_common_t;


#endif // HALLIBTEST_COMMON_H_
