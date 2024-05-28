#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
struct dust{
    int id;
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
    double val;
};
/*local time*/
time_t now = time(NULL);
tm *t = localtime(&now);
/*all months in a leap year*/
int leap[13]={0,31,29,31,30,31,30,31,31,30,31,30,31};
/*all months in a non-leap year*/
int nonleap[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
/*"leap year checking" subroutine*/
int leapyear(int year)
{
  return ((year % 4 == 0 && year % 100 != 0) || ( year % 400 == 0));
}
/*"converting from day of the year to day of the month" subroutine*/
void day(int m, int c){
    int sum = 0, presum, i = 1;
    if(leapyear(1900 + c)){
        while(sum < m){
            presum = sum;
            sum = sum + leap[i];
            i++;
        }

    }
    else{
        while(sum < m){
            presum = sum;
            sum = sum + nonleap[i];
            i++;
        }

    }
            t->tm_mday = t->tm_yday-presum;
            t->tm_mon = i-2;
}
/*"randomising between x and y for double" subroutine*/
double random (double x, double y){
    double t;
    if (x < y) 
        t = x + (double) rand() / RAND_MAX * (y - x);
    else     
        t = y + (double) rand() / RAND_MAX * (x - y);
    return t;
}
/*output subroutine*/
void result(int argc, char *argv[]){
    FILE *fptrerror;
   	fptrerror = fopen("task1.log", "w");
   	if(fptrerror == NULL){
        printf("Error!");
        exit(1);
   	}
    int a[100];
    /*ERROR and default setting*/
    if(argc > 7){
        fprintf(fptrerror, "Error 01: invalid command & argument");
        exit(1);
    }
    if (argc == 0){
        fprintf(fptrerror, "Error 01: invalid command & argument");
        exit(1);
    }
    else{
        if(argc == 1){
            printf("Default value -n 1 -st 30 -si 24\n");
            a[2] = 1;
            a[4] = 30;
            a[6] = 24;
        }
        else if(!(strcmp(argv[1], "-n"))){
            if(atoi(argv[2]) < 1){
                fprintf(fptrerror, "Error 01: invalid command & argument");
                exit(1);
            }
            else{
                a[2] = atoi(argv[2]);
                if(argc < 4){
                    printf("Default value -st 30 -si 24\n");
                    a[4] = 30;
                    a[6] = 24;
                }
                else if(!(strcmp(argv[3], "-st"))){
                    if(atoi(argv[4]) < 1){
                        fprintf(fptrerror, "Error 01: invalid command & argument");
                        exit(1);
                    }
                    else{
                        a[4] = atoi(argv[4]);
                        if(argc < 6){
                            printf("Default value -si 24\n");
                            a[6] = 24;
                            }
                        else{
                            if(!(strcmp(argv[5], "-si"))){
                                if(argc == 6){
                                    fprintf(fptrerror, "Error 01: invalid command & argument");
                                    exit(1);
                                }
                                else if(atoi(argv[6]) < 1){
                                    fprintf(fptrerror, "Error 01: invalid command & argument");
                                    exit(1);
                                }
                                else if(atoi(argv[6]) >= 1){
                                    a[6] = atoi(argv[6]);
                                }
                            }
                            else{
                                fprintf(fptrerror, "Error 01: invalid command & argument");
                                exit(1);
                            }
                        }    
                    }
                }
                else if(!(strcmp(argv[3], "-si"))){
                    if(atoi(argv[4]) < 1){
                        fprintf(fptrerror, "Error 01: invalid command & argument");
                        exit(1);
                    }
                    else{
                        a[6] = atoi(argv[4]);
                        if(argc <= 5){
                            printf("Default value -st 30\n");
                            a[4] = 30;
                        }
                        else{
                            fprintf(fptrerror, "Error 01: invalid command & argument");
                            exit(1);
                        }
                    }    
                }
                else{
                    fprintf(fptrerror, "Error 01: invalid command & argument");
                    exit(1);
                }
            }   
        }
        else if((strcmp(argv[1], "-n"))){
            if(!(strcmp(argv[1], "-st"))){
                if(argc > 5){
                    fprintf(fptrerror, "Error 01: invalid command & argument");
                    exit(1);
                }
                if(atoi(argv[2]) < 1){
                    fprintf(fptrerror, "Error 01: invalid command & argument");
                    exit(1);
                }
                else{
                    a[4] = atoi(argv[2]);
                    if(argc < 4){
                        printf("Default value -n 1 -si 24\n");
                        a[2] = 1;
                        a[6] = 24;
                    }
                    else{
                        if(!(strcmp(argv[3], "-si"))){
                            if(argc == 4){
                                fprintf(fptrerror, "Error 01: invalid command & argument");
                                exit(1);
                            }
                            else if((atoi(argv[4]) < 1)){
                                fprintf(fptrerror, "Error 01: invalid command & argument");
                                exit(1);
                            }
                            else if(atoi(argv[4]) >= 1){
                                printf("Default value -n 1\n");
                                a[2] = 1;
                                a[6] = atoi(argv[4]);
                            }
                        }
                        else{
                            fprintf(fptrerror, "Error 01: invalid command & argument");
                            exit(1);
                        }
                    }
                }
            }
            else if(!(strcmp(argv[1], "-si"))){
                if(argc > 3){
                    fprintf(fptrerror, "Error 01: invalid command & argument");
                    exit(1);
                }
                if(atoi(argv[2]) < 1){
                        fprintf(fptrerror, "Error 01: invalid command & argument");
                        exit(1);
                }
                else{
                    a[6] = atoi(argv[2]);
                    if(argc < 4){
                        printf("Default value -n 1 -st 30\n");
                        a[2] = 1;
                        a[4] = 30;
                    }
                }
            }
            else{
                fprintf(fptrerror, "Error 01: invalid command & argument");
                exit(1);
            }
        }
    }
    /*working with files*/  
	FILE *fptr;
   	fptr = fopen("dust_sensor.csv", "w");
   	if(fptr == NULL){
        fprintf(fptrerror, "Error 03: denied access dust_sensor.csv");
        exit(1);
   	}
    fclose(fptrerror);
    /*first line for titles*/
    fflush(stdin);
    fprintf(fptr, "id,time,value\n");
    a[0]=a[6] * 3600 / a[4];
    /*starting time*/
    /*starting hour*/
    t -> tm_hour -= (a[6] % 24);
    /*standardising the hour*/
    if (t->tm_hour < 0){
        t->tm_hour += 24;
        t->tm_mday--;
        t->tm_yday--;
    }
    t->tm_mday -= (a[6] / 24);
    t->tm_yday++;
    t->tm_yday -= (a[6] / 24); 
    /*calculating the day*/
    if (t->tm_yday <= 0){
        /*standardising the day and calculating the year*/
        if (leapyear(t->tm_year - 1 + 1900)) 
            t->tm_yday += 366;
        else                                   
            t->tm_yday += 365; 
        t->tm_year-- ;
    } 
    /*calculating the month*/
    day(t->tm_yday, t->tm_year);
    /*initializing random number generators*/
    srand((unsigned) time(NULL));
    /*time loop*/
    for (int k = 0; k <= a[0]; k++){
        if (k >= 1){
            /*calculating the second*/
            t->tm_sec += a[4] % 60; 
            /*standardising the second*/
            if(t->tm_sec >= 60){
                t -> tm_sec %= 60;
                t -> tm_min ++;
            }
            /*calculating the minute*/
            t -> tm_min += (a[4] / 60) % 60;
            /*standardising the minute*/
            if(t->tm_min >= 60){
                t->tm_min %= 60;
                t->tm_hour++;
            }
            /*calculating the hour*/
            t->tm_hour += ((a[4] / 60) / 60) % 24;
            /*standardising the hour*/
            if(t->tm_hour >= 24){
                t->tm_hour %= 24;
                t->tm_mday++;
                t->tm_yday++;
            }
            /*calculating the day*/
            t->tm_yday += ((a[4] / 60) / 60) / 24;
            /*standardising the day and calculating the year*/
            if (leapyear(t->tm_year + 1900)){          
                if (t->tm_yday > 366){
                t->tm_yday %= 366;
                t->tm_year++;
                }
            }
            else {
                if (t->tm_yday > 365){
                t->tm_yday %= 365;
                t->tm_year++;
                }
            }
            /*calculating the month*/
            day(t->tm_yday, t->tm_year);
        }
        /*output*/
        dust DS[10000];
        for (int i = 0; i < a[2]; i++){
            DS[i + 1].id = i + 1;
            DS[i + 1].year = 1900 + t->tm_year;
            DS[i + 1].month = 1 + t->tm_mon;
            DS[i + 1].day = t->tm_mday;
            DS[i + 1].hour = t->tm_hour;
            DS[i + 1].min = t->tm_min;
            DS[i + 1].sec = t->tm_sec;
            DS[i + 1].val = random(0,600);
		    fflush(stdin);
            fprintf(fptr,"%d,%02d:%02d:%02d %02d:%02d:%02d,%.1lf\n", DS[i + 1].id, DS[i + 1].year, DS[i + 1].month, DS[i + 1].day, 
                                                                     DS[i + 1].hour, DS[i + 1].min, DS[i + 1].sec, DS[i + 1].val);
        }
    }
   	fclose(fptr);
}
int main(int argc, char* argv[])
{ 
    
    result(argc, argv);
    return(0);
}
