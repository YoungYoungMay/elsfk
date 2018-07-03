#include "keyboard.h"
#include <stdio.h>

int main()
{
    init_keyboard();
    while(1)
    {
        int c = get_key();
        if(is_up(c))
            printf("up\n");
        if(is_down(c))
            printf("down\n");
        if(is_esc(c))
        {
            printf("esc\n");
            break;
        }
        if(is_left(c))
            printf("left\n");
        if(is_right(c))
            printf("right\n");
    }
    return 0;
}
