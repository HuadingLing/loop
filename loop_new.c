#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<time.h>
#include<getopt.h>
//平台适配
#if defined(__linux__)
	#include<unistd.h>
#elif defined(_WIN32) || defined(__WIN32__) || defined(__WIN64__)
	#include<windows.h>
#endif

typedef struct simpletime
{
    int day;
	int hour;
	int minute;
	int second;
}simpletime;


/*
option:
    -h  help(帮助)
    -v  version(版本)
    -c  cycles(循环次数)
    -g  gap(间隔时间)
    -m  maxtime(重复失败最大时间)
    ""  cmd(命令以及命令的参数一定要加上引号，
        不然可能会丢失命令的参数)
example:
    loop -h
    loop -v
    loop -c 100 -g 10 -m 0d1h20m10s "git clone xxx"
    loop "git clone xxx > xx.txt"(死循环)
*/

//命令行帮助文档
void print_help()
{
    printf(
"option:\n\
\t-h\thelp(帮助)\n\
\t-v\tversion(版本)\n\
\t-c\tcycles(循环次数)\n\
\t-g\tgap(间隔时间)\n\
\t-m\tmaxtime(重复失败最大时间)\n\
\t\"\"\tcmd(命令以及命令的参数一定要加上引号,\n\
\t\t不然可能会丢失命令的参数)\n\
example:\n\
\tloop -h\n\
\tloop -v\n\
\tloop -c 100 -g 10 -m 0d1h20m10s \"git clone xxx\"\n\
\tloop \"git clone xxx > xx.txt\"(死循环)\n"\
    );
}
//命令行版本信息
void print_version()
{
    printf("loop version 1.2\n");
}
//执行命令的函数，cmd为空什么都不做，max_time_second
int loop(char* cmd,int max_time_second,int gap,int cycles){
    if(strcmp(cmd,"")==0)
    {
        printf("cmd is null\n");
        return 0;
    }
    //win平台睡眠时间转毫秒
    #if defined(_WIN32) || defined(__WIN32__) || defined(__WIN64__)
    if(gap > 0)
    {
        gap = gap * 1000;
    }
    #endif
    int exit_code = -1;//退出码
    int start = time(NULL);//开始时间
    int end = start;//当前运行时间
    //开始主要的命令循环，循环次数为-1时，无限循环
    while (cycles == -1 || cycles >0)
    {   
        //如果不是死循环，减掉次数
        if(cycles != -1)cycles--;
        //执行命令
        exit_code = system(cmd);
        if(exit_code == 0)break;
        //睡眠，适配平台
        if(gap>0){
            #if defined(__linux__)
            sleep(gap);
            #elif defined(_WIN32) || defined(__WIN32__) || defined(__WIN64__)
            Sleep(gap);
            #endif
        }
        //超时检查
        end = time(NULL);
        if(max_time_second != -1 && max_time_second <= difftime(end, start)) break;
    }
    return exit_code;
}
//输出用户输入的选项
void print_options(int option,char* optarg ){
    printf("option:%c\n",option);
    printf("optarg:%s\n",optarg);
}
//解析输入时间，变成总秒数
int resolve_time(char* optarg ,simpletime* times){
    int len = strlen(optarg);
    times->day = 0;
    times->hour = 0;
    times->minute = 0;
    times->second = 0;
    int sub_sum = 0;
    int total_sum = 0;
    char* c= optarg;
    char curentchar  = '0';
    for (int i = 0; i < len; i++)
    {
        curentchar = *c;
        c++;
        if('0'<=curentchar && curentchar<='9')
        {
            sub_sum = sub_sum*10 + (curentchar-'0');
            continue;
        }
        else if('a'<=curentchar && curentchar<='z')
        {
            switch (curentchar) 
            {
                case 'd':
                    times->day = sub_sum;
                    break;
                case 'h':
                    times->hour = sub_sum;
                    break;
                case 'm':
                    times->minute = sub_sum;
                    break;
                case 's':
                    times->second = sub_sum;
                    break;
                default:
                    printf("error time format\n");
                    exit(1);
                    return -1;
            }
            sub_sum = 0;
        }else
        {
            printf("error time format\n");
            exit(1);
        }
    }
    total_sum = times->day*24*60*60 + times->hour*60*60 + times->minute*60 + times->second;
    return total_sum;

}
//命令行参数解析
int parse_args(int argc,char* argv[],simpletime* times,char* cmd,int* total_max_time,int* gap,int* cycles){
    //默认参数
    strcpy(cmd,"");
    //解析命令行参数
    int opt;
    while((opt = getopt(argc,argv,"hvm:c:g:"))!=-1)
    {
        //print_options(opt,optarg);//调试代码，实际使用可以删除
        switch (opt) {
            case 'h':
            case '-h':
                /* 打印帮助信息 */
                print_help();
                exit(1);
                break;
            case 'v':
            case '-v':
                /* 处理-v选项 */
                print_version();
                exit(1);
                break;
            case 'm':
            case '-m':
                /* 处理-m选项 */
                *total_max_time = resolve_time(optarg,times);
                break;
            case 'g':
            case '-g':
                /* 处理-g选项 */
                *gap = atoi(optarg);
                break;
            case 'c':
            case '-c':
                /* 处理-c选项 */
                *cycles = atoi(optarg);
                break;
            default:
                /* 处理未知的选项 */
                printf("unknown option:%c,val:%s\n",opt,optarg);
                exit(1);
                break;
        }
        strcpy(cmd,argv[optind]);//optind是第一个非选项参数，循环里面可能会变
    }
    if(opt == -1 && argc == 2)//没有选项，直接跟命令的情况
    {
        strcpy(cmd,argv[optind]);
    }
    return 0;
}


int main(int argc,char* argv[]){
    //设置控制台输出编码为utf-8
    SetConsoleOutputCP(65001);
    simpletime times={0,0,0,0};
    //执行命令的字符串
    char cmd_args[100]={0};
    //最大等待时间
    int total_max_time = -1;
    int gap = -1;
    int cycles = -1;
    parse_args(argc,argv,&times,cmd_args,&total_max_time,&gap,&cycles);
    //测试得到的参数
    printf("cmd_args:%s\n",cmd_args);
    printf("day:%d\n",times.day);
    printf("hour:%d\n",times.hour);
    printf("minute:%d\n",times.minute);
    printf("second:%d\n",times.second);
    printf("total_max_time:%d\n",total_max_time);
    printf("gap:%d\n",gap);
    printf("cycles:%d\n",cycles);

    return loop(cmd_args,total_max_time,gap,cycles);
}
