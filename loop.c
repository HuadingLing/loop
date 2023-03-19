#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<time.h>

/*
loop小工具用于命令行中执行一段容易发生错误的一段命令
*/

//平台适配
#if defined(__linux__)
	#include<unistd.h>
#elif defined(_WIN32) || defined(__WIN32__) || defined(__WIN64__)
	#include<windows.h>
#endif

//错误信息
#define ProgramErrorMsgPrefix "Param Error: "

#define UNICASE(l, u, i, s) \
    case l : \
    case u : \
        if(check_dhms(i, cur_dhms_flag)){ \
            sum = sum + s; \
        }else{ \
            printf(ProgramErrorMsgPrefix "%s\n", cur_param); \
            return 1; \
        } \
        break; 

/*
loop -h
loop -H
loop --help
loop 100 10s 1h20m10s git clone xxx
loop "git clone xxx > xx.txt"
*/

//命令行帮助文档
void print_help()
{
    
    printf("loop -h\nloop --help\nloop 100 10s 1h20m10s git clone xxx\nloop \"git clone xxx > xx.txt\"");
}

// void debug(bool n_flag, bool sleep_flag, bool limit_flag, int n_try, int sleep_time, int limit_run_time, int cmd_id, const char* cmd_args){
//     char c;

//     if(n_flag==true){
//         c = 'T';
//     }else{
//         c = 'F';
//     }
//     printf("n_flag: %c %d\n", c, n_try);


//     if(sleep_flag==true){
//         c = 'T';
//     }else{
//         c = 'F';
//     }
//     printf("sleep_flag: %c %d\n", c, sleep_time);

//     if(limit_flag==true){
//         c = 'T';
//     }else{
//         c = 'F';
//     }
//     printf("limit_flag: %c %d\n", c, limit_run_time);

//     printf("cmd_id: %d\n", cmd_id);
//     printf("cmd: %s\n", cmd_args);

// }


//检测天、小时、分钟、秒
bool check_dhms(int i, bool *dhms_flag)
{
    

    for(int j = i; j < 4; j++){
        if(dhms_flag[j]){
            return false;
        }
    }
    return true;
}






int main(int argc, char *argv[])
{
    //printf("%d\n", argc);

    //重复次数标志位
    bool n_flag = false;
    //间隔时间标志位
    bool sleep_flag = false;
    //限制时间标志位
    bool limit_flag = false;
    int n_try = 0;
    int sleep_time = 0;
    int limit_run_time = 0;

    //命令字符串的下标位置
    int cmd_id = 0;
    

    bool time_dhms_flag[4] = {false, false, false, false};
    bool sleep_dhms_flag[4] = {false, false, false, false};

    //当前参数字符串
    char *cur_param;
    //现在参数字符串的首个字符
    char s;
    //命令字符串的长度，用于申请内存空间
    int len;

    int sum;
    int sub_sum;
    char *c;
    bool is_num;
    bool is_time;
    bool *cur_dhms_flag;

    //------------------第一步，获取命令以及循环执行配置--------------------

    //遍历传入参数的序列号，第一个参数，i为1
    for(int i=1; i<argc; i++)
    {
        //printf("%d: %s\n", i, argv[i]);

        //现在的参数
        cur_param = argv[i];

        //现在参数字符串的首个字符
        s = cur_param[0];
        //printf("%d: %s\n", i, cur_param);
        if('1'<=s && s<='9')
        {
            is_num = true;
            // if(n_flag == true){
            //     is_num = false;
            // }else{
            //     is_num = true;
            // }
            sum = 0;
            sub_sum = s-'0';
            //参数的第二个字符
            c = cur_param + 1;
            while(*c)
            {
                //获取当前参数的第x个字符(x为2到末尾)
                s = *c;
                if ('0'<=s && s<='9')
                {
                    //当前字符为0，并且当前数字总和为零，用来排除数字以00开头的情况
                    if(s=='0' && sub_sum==0)
                    {
                        printf("Param Error: %s\n", cur_param);
                        return 1;
                    }
                    //更新总和数字
                    sub_sum = sub_sum * 10 + s-'0';
                }
                else//当前字符不是数字
                {
                    //更新标志位
                    if(is_num == true)
                    {
                        is_num = false;
                        if(sleep_flag == false)
                        {
                            sleep_flag = true;
                            cur_dhms_flag = sleep_dhms_flag;
                        }else{
                            if(limit_flag == false)
                            {
                                limit_flag = true;
                                cur_dhms_flag = time_dhms_flag;
                            }else
                            {

                            }
                        }
                    }
                    switch(s){
                        UNICASE('s', 'S', 3, sub_sum)
                        UNICASE('m', 'M', 2, sub_sum*60)
                        UNICASE('h', 'H', 1, sub_sum*3600)
                        UNICASE('d', 'D', 0, sub_sum*24*3600)
                        default :
                            printf(ProgramErrorMsgPrefix "%s\n", cur_param);
                            return 1;
                    }
                    //数字重置
                    sub_sum = 0;
                }
                //下一个字符
                c++;
            }
            if(is_num){
                if(n_flag==true){
                    printf(ProgramErrorMsgPrefix "more than one n flag exist\n");
                    return 1;
                }
                n_flag = true;
                //获取尝试次数
                n_try = sub_sum;
            }else{
                if(limit_flag){
                    limit_run_time = sum;
                }else{
                    sleep_time = sum;
                }
            }
        }
        else//参数开头不是1-9的数字
        {
            //参数开头是数字0
            if(s=='0')
            {
                s = cur_param[1];

                //参数为0s和0S的情况
                if(strlen(cur_param) == 2 && (s=='s' || s=='S'))
                {
                    if(sleep_flag==false)
                    {
                        sleep_flag = true;
                        sleep_time = 0;
                    }else
                    {
                        cmd_id = i;
                        break;
                    }
                }else
                {
                    cmd_id = i;
                    break;
                }
            }else//参数开头不是数字
            {
                if(s=='-')
                {
                    len = strlen(cur_param);
                    if(len>2)
                    {
                        //情况loop --help
                        if(len==6 && strcmp(cur_param+1, "-help")==0)
                        {
                            print_help();
                            return 0;
                        }
                        else
                        {
                            cmd_id = i;
                            break;
                        }

                        // if(len!=6){
                        //     //printf(ProgramErrorMsgPrefix "%s", cur_param);
                        //     //return 1;
                        //     cmd_id = i;
                        //     //if(c_flag && n_flag && sleep_flag && limit_flag){
                        //     //    break;
                        //     //}
                        // }else{
                        //     if(strcmp(cur_param+1, '-help')==0){
                        //         print_help()
                        //     }
                        // }
                    }else
                    {
                        if(len==2)
                        {
                            s = cur_param[1];
                            if(s=='h' || s=='H')
                            {
                                print_help();
                                return 0;
                            }else
                            {
                                cmd_id = i;
                                break;
                                // if(s=='c' || s=='c'){
                                //     if(c_flag==true){
                                //         printf(ProgramErrorMsgPrefix "more than one \"-c\" or \"-C\" flag exist.");
                                //         return 1;
                                //     }
                                //     c_flag = true;
                                // }else{
                                //     cmd_id = i;
                                //     break;
                                //     //printf(ProgramErrorMsgPrefix "%s", cur_param);
                                //     //return 1
                                // }
                                
                            }
                        }else//参数为单个不为数字的字符串
                        {
                            cmd_id = i;
                            break;
                            //printf(ProgramErrorMsgPrefix "%s", cur_param);
                            //return 1
                        }
                    }
                }else//参数开头不是数字不是"-"
                {
                    cmd_id = i;
                    break;
                    //if(c_flag && n_flag && sleep_flag && limit_flag){
                    //    break;
                    //}
                }
            }
        }

        
    }

    if(cmd_id==0)
    {
        printf(ProgramErrorMsgPrefix "no cmd detected.\n");
        return 2;
    }



    //执行命令的字符串
    char* cmd_args;
    //系统执行命令的结果返回码
    int exitcode = 1;
    if(cmd_id==argc-1)
    {
        // if(cmd_id != argc-1){
        //     printf("Param cmd Error: when use \"-c\" or \"-C\" flag the cmd must in \"\" pair as a string, and not more than one cmd string.");
        //     return 2;
        // }
        // // len = strlen(argv[cmd_id]) - 2;
        // // if(len<=0){
        // //     printf(ProgramErrorMsgPrefix "empty cmd.");
        // // }
        // // cmd_args = (char*)malloc(len);
        // // strncpy(cmd_args, argv[cmd_id]+1, len);
        // // //cmd_args[len-1]='\0';
        cmd_args = argv[cmd_id];

    }else
    {
        len = 0;
        for(int i=cmd_id;i<argc;i++)
        {
            len = len + strlen(argv[i]) + 1;  // 给malloc函数的参数加1是为了容纳空格/NUL字符
        }

        cmd_args = (char*)malloc(len);

        //int id = 0;
        for(int i=cmd_id;i<argc;i++)
        {
            if(i == cmd_id)
            {
                strcpy(cmd_args, argv[i]);
                
            }else
            {
                //if(i < argc-1){
                //    cmd_args[i] = ' ';
                //    id = id + 1;
                //}
                strcat(cmd_args, " ");
                strcat(cmd_args, argv[i]);
            }
            //id = id + strlen(argv[i]);
        }
        //cmd_args[len-1]='\0';
    }

    //debug(n_flag, sleep_flag, limit_flag, n_try, sleep_time, limit_run_time, cmd_id, cmd_args);
//win平台睡眠时间转毫秒
    #if defined(_WIN32) || defined(__WIN32__) || defined(__WIN64__)
    if(sleep_flag==true && sleep_time>0)
    {
        sleep_time = sleep_time * 1000;
    }
    #endif

    //------------------第二步，执行命令以及循环--------------------


    //time_t timep;
    //程序开始运行时间戳
    int start = time(NULL);  // 1970年到当前的秒数
    //程序当前运行时间戳
    int end;
    while(1){
        //执行命令
        exitcode = system(cmd_args);
        //printf("return: %d\n", exitcode);

        //结束条件判断
        if(exitcode==0)
        {
            break;
        }
        if(n_flag==true)
        {
            n_try--;
            if(n_try==0)
            {
                break;
            }
        }
        if(limit_flag==true)
        {
            end = time(NULL);
            if(limit_run_time <= difftime(end, start))
            {
                break;
            }
        }

        //判断是否睡眠
        if(sleep_flag==true && sleep_time>0)
        {
            //printf("start sleep\n");

            //等待时间准备下次重试
            #if defined(__linux__)
	        sleep(sleep_time);
            #elif defined(_WIN32) || defined(__WIN32__) || defined(__WIN64__)
	        Sleep(sleep_time);
            #endif
            //printf("end sleep\n");
        }
    }


    //释放命令字符串内存
    if(cmd_id!=argc-1)
    {
        free(cmd_args);
    }
    
        
    
    return 0;
}