#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Command.h"
#include "common.h"

const char *kCommandShopList = "shop list";
const char *kCommandOrder = "order";
const char *kCommandConfirm = "confirm";
const char *kCommandCancel = "cancel";
const char *kCommandYes = "Yes";
const char *kCommandNo = "No";

CommandPtr createCommand(char *buffer)
{
    int commandLength = 0;
    CommandPtr command = malloc(sizeof(Command));
    if (command == NULL)
    {
        return NULL;
    }

    command->type = kInvalid;
    command->arg = NULL;

    if (strcmp(buffer, kCommandShopList) == 0)
    {
        command->type = kShopList;
        commandLength = strlen(kCommandShopList);
    }
    else if (startWith(buffer, kCommandOrder))
    {
        command->type = kOrder;
        commandLength = strlen(kCommandOrder);
    }
    else if (strcmp(buffer, kCommandConfirm) == 0)
    {
        command->type = kConfirm;
        commandLength = strlen(kCommandConfirm);
    }
    else if (strcmp(buffer, kCommandCancel) == 0)
    {
        command->type = kCancel;
        commandLength = strlen(kCommandCancel);
    }
    else if (strcmp(buffer, kCommandYes) == 0)
    {
        command->type = kYes;
        commandLength = strlen(kCommandYes);
    }
    else if (strcmp(buffer, kCommandNo) == 0)
    {
        command->type = kNo;
        commandLength = strlen(kCommandNo);
    }

    if (command->type != kInvalid)
    {
        command->arg = malloc(strlen(buffer + commandLength) + 1);
        if (command->arg == NULL)
        {
            free(command);
            return NULL;
        }

        strcpy(command->arg, buffer + commandLength);
    }

    return command;
}

void destroyCommand(CommandPtr command)
{
    if (command->arg != NULL)
    {
        free(command->arg);
    }
    free(command);
}
