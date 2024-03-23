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
loop -h
loop 100 10s 1h20m10s git clone xxx
loop "git clone xxx > xx.txt"

help
version
cycles(循环次数)
gap(间隔时间)
maxtime(重复失败最大时间)
ls
cmd(命令一定要加上引号，不然可能会丢失命令的参数)
*/

//命令行帮助文档
void print_help()
{
    printf("loop -h\nloop --help\nloop 100 10s 1h20m10s git clone xxx\nloop \"git clone xxx > xx.txt\"");
}
//命令行版本信息
void print_version()
{
    printf("loop version 1.2\n");
}

void loop(char* cmd,int time_second,int gap,int cycles){

}

void print_options(int option,char* optarg ){
    printf("option:%c\n",option);
    printf("optarg:%s\n",optarg);
}
//解析输入时间，变成总秒数
int resolve_time(char* optarg ,simpletime* times){

    int len = strlen(optarg);
    int sub_sum = 0;
    int total_sum = 0;
    int carry = 0;
    char* c= optarg;
    char curentchar  = '0';
    for (int i = 0; i <= len; i++)
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
                    carry = 24;
                    break;
                case 'h':
                    times->hour = sub_sum;
                    carry = 60;
                    break;
                case 'm':
                    times->minute = sub_sum;
                    carry = 60;
                    break;
                case 's':
                    times->second = sub_sum;
                    carry = 1;
                    break;
                default:
                    printf("error time format\n");
                    exit(1);
                    return -1;
            }
            //TODO 目前这个有逻辑漏洞，如果用户输入1d2s会因为进制转换算错
            //需要分开循环，先存储结构体，再计算秒数
            total_sum = (total_sum + sub_sum) * carry;
            sub_sum = 0;
        }
        
        
    }
    return total_sum;
    
    
}
//命令行参数解析
int parse_args(int argc,char* argv[],simpletime* times,char* cmd,int* total_max_time){
    //默认参数
    strcpy(cmd,"");
    //解析命令行参数
    int opt;
    while((opt = getopt(argc,argv,"hvm:"))!=-1)
    {
        // printf("index of opt:%d\n",optind);
        print_options(opt,optarg);
        switch (opt) {
            case 'h':
                /* 打印帮助信息 */
                print_help();
                exit(1);
                break;
            case 'v':
                /* 处理-v选项 */
                print_version();
                exit(1);
                break;
            case 'm':
                /* 处理-m选项 */
                *total_max_time = resolve_time(optarg,times);
                break;
            case 'g'://TODO
            case 'c':
            default:
                /* 处理未知的选项 */
                printf("unknown option:%c,val:%s\n",opt,optarg);
                exit(1);
                break;
        }
        // printf("argv[optind]:%s\n",argv[optind]);
        strcpy(cmd,argv[optind]);
    }
    
}


int main(int argc,char* argv[]){
    simpletime times={0,0,0,0};
    //执行命令的字符串
    char cmd_args[100];
    //最大等待时间
    int total_max_time = -1;
    parse_args(argc,argv,&times,cmd_args,&total_max_time);
    printf("cmd:%s\n",cmd_args);
    printf("day:%d\n",times.day);
    printf("hour:%d\n",times.hour);
    printf("minute:%d\n",times.minute);
    printf("second:%d\n",times.second);
    printf("total_max_time:%d\n",total_max_time);
    return 0;
}
