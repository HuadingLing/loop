#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<time.h>

#if defined(__linux__)
	#include<unistd.h>
#elif defined(_WIN32) || defined(__WIN32__) || defined(__WIN64__)
	#include<windows.h>
#endif


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

void print_help(){
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

bool check_dhms(int i, bool *dhms_flag){
    for(int j = i; j < 4; j++){
        if(dhms_flag[j]){
            return false;
        }
    }
    return true;
}






int main(int argc, char *argv[]){
    //printf("%d\n", argc);

    bool n_flag = false;
    bool sleep_flag = false;
    bool limit_flag = false;
    int n_try = 0;
    int sleep_time = 0;
    int limit_run_time = 0;
    int cmd_id = 0;
    

    bool time_dhms_flag[4] = {false, false, false, false};
    bool sleep_dhms_flag[4] = {false, false, false, false};

    
    char *cur_param;
    char s;
    int len;

    int sum;
    int sub_sum;
    char *c;
    bool is_num;
    bool is_time;
    bool *cur_dhms_flag;

    
    for(int i=1; i<argc; i++){
        //printf("%d: %s\n", i, argv[i]);
        cur_param = argv[i];
        s = cur_param[0];
        //printf("%d: %s\n", i, cur_param);
        if('1'<=s && s<='9'){
            is_num = true;
            // if(n_flag == true){
            //     is_num = false;
            // }else{
            //     is_num = true;
            // }
            sum = 0;
            sub_sum = s-'0';
            c = cur_param + 1;
            while(*c){
                s = *c;
                if ('0'<=s && s<='9'){
                    if(s=='0' && sub_sum==0){
                        printf("Param Error: %s\n", cur_param);
                        return 1;
                    }
                    sub_sum = sub_sum * 10 + s-'0';
                }else{
                    if(is_num == true){
                        is_num = false;
                        if(sleep_flag == false){
                            sleep_flag = true;
                            cur_dhms_flag = sleep_dhms_flag;
                        }else{
                            if(limit_flag == false){
                                limit_flag = true;
                                cur_dhms_flag = time_dhms_flag;
                            }else{

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
                    sub_sum = 0;
                }
                c++;
            }
            if(is_num){
                if(n_flag==true){
                    printf(ProgramErrorMsgPrefix "more than one n flag exist\n");
                    return 1;
                }
                n_flag = true;
                n_try = sub_sum;
            }else{
                if(limit_flag){
                    limit_run_time = sum;
                }else{
                    sleep_time = sum;
                }
            }
        }else{
            if(s=='0'){
                s = cur_param[1];
                if(strlen(cur_param) == 2 && (s=='s' || s=='S')){
                    if(sleep_flag==false){
                        sleep_flag = true;
                        sleep_time = 0;
                    }else{
                        cmd_id = i;
                        break;
                    }
                }else{
                    cmd_id = i;
                    break;
                }
            }else{
                if(s=='-'){
                    len = strlen(cur_param);
                    if(len>2){
                        if(len==6 && strcmp(cur_param+1, "-help")==0){
                            print_help();
                            return 0;
                        }else{
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
                    }else{
                        if(len==2){
                            s = cur_param[1];
                            if(s=='h' || s=='H'){
                                print_help();
                                return 0;
                            }else{
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
                        }else{
                            cmd_id = i;
                            break;
                            //printf(ProgramErrorMsgPrefix "%s", cur_param);
                            //return 1
                        }
                    }
                }else{
                    cmd_id = i;
                    break;
                    //if(c_flag && n_flag && sleep_flag && limit_flag){
                    //    break;
                    //}
                }
            }
        }

        
    }


    if(cmd_id==0){
        printf(ProgramErrorMsgPrefix "no cmd detected.\n");
        return 2;
    }

    
    char* cmd_args;
    
    int exitcode = 1;
    if(cmd_id==argc-1){
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

    }else{
        len = 0;
        for(int i=cmd_id;i<argc;i++){
            len = len + strlen(argv[i]) + 1;  // 给malloc函数的参数加1是为了容纳空格/NUL字符
        }

        cmd_args = (char*)malloc(len);

        //int id = 0;
        for(int i=cmd_id;i<argc;i++){
            if(i == cmd_id){
                strcpy(cmd_args, argv[i]);
                
            }else{
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
#if defined(_WIN32) || defined(__WIN32__) || defined(__WIN64__)
    if(sleep_flag==true && sleep_time>0){
        sleep_time = sleep_time * 1000;
    }
#endif
    //time_t timep;
    int start = time(NULL);  // 1970年到当前的秒数
    int end;
    while(1){
        exitcode = system(cmd_args);
        //printf("return: %d\n", exitcode);
        if(exitcode==0){
            break;
        }
        if(n_flag==true){
            n_try--;
            if(n_try==0){
                break;
            }
        }
        if(limit_flag==true){
            end = time(NULL);
            if(limit_run_time <= difftime(end, start)){
                break;
            }
        }
        if(sleep_flag==true && sleep_time>0){
            //printf("start sleep\n");
#if defined(__linux__)
	        sleep(sleep_time);
#elif defined(_WIN32) || defined(__WIN32__) || defined(__WIN64__)
	        Sleep(sleep_time);
#endif
            //printf("end sleep\n");
        }
    }

    if(cmd_id!=argc-1){
        free(cmd_args);
    }
    
        
    
    return 0;
}