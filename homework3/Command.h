#pragma once

struct Command;
typedef struct Command Command;
typedef struct Command* CommandPtr;

enum CommandType
{
    kInvalid = -1,
    kShopList,
    kOrder,
    kConfirm,
    kCancel,
    kYes,
    kNo
};

struct Command
{
    enum CommandType type;
    char *arg;
};

CommandPtr createCommand(char *buffer);
void destroyCommand(CommandPtr command);
