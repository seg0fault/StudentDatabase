#ifndef COMMAND_ENUM_H
#define COMMAND_ENUM_H

#define ERR_CMD -1
#define ERR_INSERT_SYNTAX -2
#define ERR_SELECT_SYNTAX -3
#define ERR_DELETE_SYNTAX -4

enum cmd_type
{
    CMD_NONE = 0,
    CMD_QUIT = -1,
    CMD_STOP = -2,
    CMD_SELECT = 1,
    CMD_INSERT = 2,
    CMD_DELETE = 3
};

enum cond_type
{
    COND_NONE = 0,
    COND_EQ = 1,
    COND_NE = -1,
    COND_GT = 2,
    COND_LT = -2,
    COND_GE = 3,
    COND_LE = -3,
    COND_LIKE = 4
};

enum op_type
{
    OP_NONE = 0,
    OP_AND = 1,
    OP_OR = 2
};

#endif
