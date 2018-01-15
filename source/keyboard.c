#include "keyboard.h"
#include "stringutils.h"

char * get_input(unsigned int max)
{
    char * input = calloc(max+2, sizeof(char));

    if(input != NULL)
    {
        SwkbdState swkbd;
        swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 2, max);

        swkbdSetHintText(&swkbd, "Select a name for the file.");

        swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, max);

        SwkbdButton button = swkbdInputText(&swkbd, input, max);
        if(button == SWKBD_BUTTON_CONFIRM)
        {
            cleanPath(input);
        }
        else
        {
            free(input);
            input = NULL;
        }
    }

    return input;
}