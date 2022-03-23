#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAXSZ 512

enum state{
    S_WAIT,             // 等待参数
    S_ARG,              // 参数内
    S_AGR_END,          // 参数结束
    S_ARG_LINE_END,     // 左侧有参数的换行     
    S_LINE_END,         // 左侧有空格的换行
    S_END               // 结束
};

enum char_type{
    C_SPACE,
    C_CHAR,
    C_LINE_END
};

enum char_type get_char_type(char c)
{
    switch (c)
    {
    case ' ':
        return C_SPACE;
    case '\n':
        return C_LINE_END;
    default:
        return C_CHAR;
    }
}

enum state transform_state(enum state cur, enum char_type ct)
{
    switch (cur)
    {
    case S_WAIT:
        if (ct == C_SPACE)      return S_WAIT;
        if (ct == C_CHAR)       return S_ARG;
        if (ct == C_LINE_END)   return S_LINE_END;
    case S_ARG:
        if (ct == C_SPACE)      return S_AGR_END;
        if (ct == C_CHAR)       return S_ARG;
        if (ct == C_LINE_END)   return S_ARG_LINE_END;
    case S_AGR_END:
    case S_ARG_LINE_END:
    case S_LINE_END:
        if (ct == C_SPACE)      return S_WAIT;
        if (ct == C_LINE_END)   return S_LINE_END;
        if (ct == C_CHAR)       return S_ARG;
    default:
        break;
    }
    return S_AGR_END;
}

void clear_argv(char *x_argv[MAXARG], int beg)
{
    for (int i = beg; i < MAXARG; i++)
        x_argv[i] = 0;
}

int
main(int argc, char *argv[])
{
    if (argc - 1 >= MAXARG) {
        fprintf(2, "xargs: too many arguments.\n");
        exit(1);    
    }
    
    char lines[MAXSZ];
    char *p = lines;
    char *x_argv[MAXARG] = {0};

    for (int i = 1; i < argc; i++)
    {
        x_argv[i - 1] = argv[i];
    }
    int arg_beg = 0;
    int arg_end = 0;
    int arg_cur = argc - 1;
    enum state st = S_WAIT;

    while (st != S_END)
    {
        if (read(0, p, sizeof(char)) != sizeof(char))
        {
            st = S_END;
        } else {
            st = transform_state(st, get_char_type(*p));
        }

        if (++arg_end >= MAXSZ)
        {
            fprintf(2, "xargs: arguments too long\n");
            exit(1);
        }
        
        switch (st)
        {
        case S_WAIT:
            ++arg_beg;
            break;
        case S_AGR_END:
            x_argv[arg_cur++] = &lines[arg_beg];
            arg_beg = arg_end;
            *p = '\0';
            break;
        case S_ARG_LINE_END:
            x_argv[arg_cur++] = &lines[arg_beg];
        case S_LINE_END:
            arg_beg = arg_end;
            *p = '\0';
            if (fork() == 0)
                exec(argv[1], x_argv);
            arg_cur = argc - 1;
            clear_argv(x_argv, arg_cur);
            wait(0);
            break;
        default:
            break;
        }
        ++p;
    }

    exit(0);
}