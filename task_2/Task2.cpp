#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <conio.h>
struct dm{
    int day;
    int month;
};
dm d;
struct dust{
     int id;
     int year;
     int month;
     int day;
     int hour;
     int min;
     int sec;
     double val;
     char title[69];
     double avr;
     char pol[69];
     double aqi;
};
dust DS[10001];
dust AV[10001];
dust ST[10001];
dust SM[10001];
/*all months in a leap year*/
int leap[13]={0,31,29,31,30,31,30,31,31,30,31,30,31};
/*all months in a non-leap year*/
int nonleap[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
/*"leap year checking" subroutine*/
int leapyear(int year)
{
  return ((year % 4 == 0 && year % 100 != 0) || (year % 40 == 0));
}
/*"converting from day of the year to day of the month" subroutine*/
void ngay(int m, int c){
    int sum = 0, presum, i = 1;
    if(leapyear(c)){
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
          d.day = m - presum;
          d.month = i-1;
}
/*"converting from day of the month to day of the year" subroutine*/
int day2(int a, int b, int c){
     int day = a;
     if(leapyear(c)){
        for(int i = 0; i < b; i++){
          day += leap[i];
        }
     }
     else{
        for(int i = 0; i < b; i++){
          day += nonleap[i];
        }
     }
     return day;
}
double map(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void aqi(double x, int i){
	if((x >= 0) && (x < 12)){
		AV[i + 1].aqi = round(map(x, 0, 12, 0, 50));
		strcpy(AV[i + 1].pol, "Good");
	}
	if((x >= 12) && (x < 35.5)){
		AV[i + 1].aqi = round(map(x, 12, 35.5, 50, 100));
		strcpy(AV[i + 1].pol, "Moderate");
	}
	if((x >= 35.5) && (x < 55.5)){
		AV[i + 1].aqi = round(map(x, 35.5, 55.5, 100, 150));
		strcpy(AV[i + 1].pol, "Slightly unhealthy");
	}
	if((x >= 55.5) && (x < 150.5)){
		AV[i + 1].aqi = round(map(x, 55.5, 150.5, 150, 200));
		strcpy(AV[i + 1].pol, "Unhealthy");
	}
	if((x >= 150.5) && (x < 250.5)){
		AV[i + 1].aqi = round(map(x, 150.5, 250.5, 200, 300));
		strcpy(AV[i + 1].pol, "Very unhealthy");
	}
	if((x >= 250.5) && (x < 350.5)){
		AV[i + 1].aqi = round(map(x, 250.5, 350.5, 300, 400));
		strcpy(AV[i + 1].pol, "Hazardous");
	}
	if((x >= 350.5) && (x <= 550.5)){
		AV[i + 1].aqi = round(map(x, 350.5, 550.5, 400, 500));
		strcpy(AV[i + 1].pol, "Extremely hazardous");
	}
}
void statistics(int k, int rec, int n, const char *s){
     for (int j = 0; j < n; j++){
          ST[7 * j + k].hour = 0;
          for(int i = 0; i < rec - 1; i++){
               if (AV[i + 1].id == j + 1){
                    if(!(strcmp(AV[i + 1].pol, s))){
                         ST[7 * j + k].hour++;
                         strcpy(ST[7 * j + k].pol, s);
                         ST[7 * j + k].id = j + 1;
                    }
               }
               if (ST[7 * j + k].hour == 0){
                    strcpy(ST[7 * j + k].pol, s);
                    ST[7 * j + k].id = j + 1;
               }
          }
     }
}
void process(int argc, char *argv[]){
     /*Working with file*/
     FILE *fptrin;
     FILE *fptrerror;
   	fptrerror = fopen("task2.log", "w");
   	if(fptrerror == NULL){
          printf("Error!");
          exit(1);
   	}
     if(argc < 1){
          printf("command line: %s [data_filename.csv]\n", argv[0]);
          exit(0);
     }
     if(argc > 2){
          printf("command line: %s [data_filename.csv]\n", argv[0]);
          fprintf(fptrerror, "Error 00: invalid command");
          exit(1);
     }
     if(argc == 1){
   	     fptrin = fopen("dust_sensor.csv", "rt");
   	     if(fptrin == NULL){
               fprintf(fptrerror, "Error 01: file not found or cannot be accessed");
               exit(1);
   	     }
     }
     if(argc == 2){
   	     fptrin = fopen(argv[1], "rt");
   	     if(fptrin == NULL){
               fprintf(fptrerror, "Error 01: file not found or cannot be accessed");
               exit(1);
   	     }
          if(strcmp(strrchr(argv[1], '.'), ".csv" )){
               fprintf(fptrerror, "Error 02: invalid csv file");
               exit(1);
          }
     } 
     FILE *fptrout;
   	fptrout = fopen("dust_outliers.csv", "w");
   	if(fptrout == NULL){
          fprintf(fptrerror, "Error 04: denied access dust_outliers.csv");
          exit(1);
   	}
     FILE *fptraqi;
   	fptraqi = fopen("dust_aqi.csv", "w");
   	if(fptraqi == NULL){
          fprintf(fptrerror, "Error 05: denied access dust_aqi.csv");
          exit(1);
   	}
     FILE *fptrsta;
   	fptrsta = fopen("dust_statistics.csv", "w");
   	if(fptrsta == NULL){
          fprintf(fptrerror, "Error 06: denied access dust_statistics.csv");
          exit(1);
   	}
     FILE *fptrsum;
   	fptrsum = fopen("dust_summary.csv", "w");
   	if(fptrsum == NULL){
          fprintf(fptrerror, "Error 07: denied access dust_summary.csv");
          exit(1);
   	}
     /*Input*/
     int reads = 0;
     int records = 0;
     fflush stdin;
     reads = fscanf(fptrin,"%s",DS[records].title);
     if(reads == 1)      
          records++;
     if(reads != 1 && !feof(fptrin)){
          fprintf(fptrerror, "Error 02: invalid csv file\n"); 
          fprintf(fptrerror, "Error 03: data is missing at line %d\n", records);    
          exit(1);
     }
     do{
          fflush stdin;
          reads = fscanf(fptrin,"%d,%d:%d:%d %d:%d:%d,%lf", &DS[records].id, &DS[records].year, &DS[records].month, &DS[records].day, 
                                                            &DS[records].hour, &DS[records].min, &DS[records].sec, &DS[records].val);
                                                                                                                 
          if(reads == 8){
               if((DS[records].id < 0) 
                  || (DS[records].sec < 0) 
                  || (DS[records].sec >= 60) 
                  || (DS[records].min < 0) || (DS[records].min >= 60) 
                  || (DS[records].hour < 0) || (DS[records].hour >= 24) 
                  || (DS[records].day < 0) || (DS[records].month < 0) || (DS[records].month >= 12) || (DS[records].year < 0)){
                    fprintf(fptrerror, "Error 03: data is missing at line %d\n", records);
                    exit(1);
               }
               if(leapyear(DS[records].year)){
                    if(DS[records].day > leap[DS[records].month]){
                         fprintf(fptrerror, "Error 03: data is missing at line %d\n", records);
                         exit(1);
                    }
               }
               if(!leapyear(DS[records].year)){
                    if(DS[records].day > nonleap[DS[records].month]){
                         fprintf(fptrerror, "Error 03: data is missing at line %d\n", records);
                         exit(1);
                    } 
               }
               if(ferror(fptrin)){
                    fprintf(fptrerror, "Error 02: invalid csv file\n");
                    exit(1);
   	          }
               records++;
          }
          if(reads != 8 && !feof(fptrin)){
               fprintf(fptrerror, "Error 02: invalid csv file\n");
               fprintf(fptrerror, "Error 03: data is missing at line %d\n", records);
               exit(1);
          }
     } while(!feof(fptrin));
     if(records < 2){
          fprintf(fptrerror, "Error 02: invalid csv file\n");
          fprintf(fptrerror, "Error 03: data is missing at line %d\n", records);
          exit(1);
     }
     int out = 0;
     for(int i = 0; i < records - 1; i++){
          if((DS[i + 1].val < 5) || (DS[i + 1].val > 550.5))
               out++;       
     /*dust_outliers aka Task2.1*/}
     fprintf(fptrout,"number of outliers: %d\n", out);
     fprintf(fptrout, "id,time,value\n");
     for(int i = 0; i < records - 1; i++){
          if((DS[i + 1].val < 5) || (DS[i + 1].val > 550.5))
               fprintf(fptrout,"%d,%02d:%02d:%02d %02d:%02d:%02d,%.1lf\n", DS[i + 1].id, DS[i + 1].year, DS[i + 1].month, DS[i + 1].day, 
                                                                           DS[i + 1].hour, DS[i + 1].min, DS[i + 1].sec, DS[i + 1].val);
     }
     fclose(fptrout);
     /*dust_aqi aka Task2.1*/
     int year, day, month, hour, id;
     double sum, sum2, max, min;
     int count, count2;
     int line = 0, buffer;
     int i;
     int n = DS[records - 1].id;
     int rec = 1;
     /*Working with time and value*/
     year = DS[1].year;
     month  = DS[1].month;
     day = DS[1].day;       
     day = day2(day, month, year);
     while(day != 366){
          ngay(day, year);
          hour = 1;
          while(hour != 24){ 
               id = 1;
               while(id <= n){
                    sum = 0;
                    count = 0;
                    for(i = line; i < records - 1; i++){
                         if((DS[i + 1].val >= 5) 
                            && (DS[i + 1].val <= 550.5) 
                            && (DS[i + 1].hour == hour - 1) 
                            && (DS[i + 1].id == id) && (DS[i + 1].year == year) 
                            && (DS[i + 1].day == d.day) && (DS[i + 1].month == d.month)){
                              sum += DS[i + 1].val;
                              count ++;
                              buffer = i + 1;
                         } 
                    }
                    if(count == 0){
                         id++;
                         continue;
                    }
                    AV[rec].avr = sum / count;
                    AV[rec].id = id;
                    AV[rec].min = 0;
                    AV[rec].sec = 0;
                    AV[rec].hour = hour;
                    AV[rec].day = d.day;
                    AV[rec].month = d.month;
                    AV[rec].year = year;
                    id ++;
                    rec++;
               }
               hour++;
               if (hour == 24){
                    id = 1;
                    while(id <= n){
                         sum = 0;
                         count = 0;
                         for(int i = line; i < records - 1; i ++){
                              if((DS[i + 1].val >= 5) 
                                 && (DS[i + 1].val <= 550.5) 
                                 && (DS[i + 1].hour == 23) && (DS[i + 1].id == id) 
                                 && (DS[i + 1].year == year) && (DS[i + 1].day == d.day) && (DS[i + 1].month == d.month)){
                                   sum += DS[i + 1].val;
                                   count++;
                                   buffer = i + 1;
                              } 
                         }
                         if(count == 0){
                              id++;
                              continue;
                         }
                         AV[rec].avr = sum / count;
                         AV[rec].id = id;
                         AV[rec].min = 0;
                         AV[rec].sec = 0;
                         AV[rec].hour = 0;
                         day ++;
                         if (leapyear(year)){          
                              if (day > 366){
                                   day %= 366;
                                   year++;
                              }
                         }
                         else{
                              if (day > 365){
                                   day %= 365;
                                   year++;
                              }
                         }
                         ngay(day, year);
                         AV[rec].day = d.day;
                         AV[rec].month = d.month;
                         AV[rec].year = year;
                         id++;
                         rec++;
                         day --;
                         if (day <= 0){
                              if (leapyear(year - 1))
                                   day += 366;
                              else                                   
                                   day += 365; 
                              year-- ;
                         }
                         ngay(day, year);
                    }
                    line = buffer;
               }
          }
          day ++;
          if (leapyear(year)){          
               if (day > 366){
                    day %= 366;
                    year++;
                    }
               }
          else{
               if (day > 365){
                    day %= 365;
                    year++;
               }
          }
     }
     /*Output for Task2.2*/
     fflush(stdin);
     fprintf(fptraqi, "id,time,value,aqi,pollution\n");    
     for(int i = 0; i < rec - 1; i++){
          aqi(AV[i + 1].avr, i);
          fprintf(fptraqi,"%d,%02d:%02d:%02d %02d:%02d:%02d,%.1lf,%.0lf,%s\n", AV[i + 1].id, AV[i + 1].year, AV[i + 1].month, AV[i + 1].day, AV[i + 1].hour, 
                                                                               AV[i + 1].min, AV[i + 1].sec, AV[i + 1].avr, AV[i + 1].aqi, AV[i + 1].pol);
     }
     fclose(fptraqi);
     fclose(fptrin);
     /*dust_statistics aka Task2.4*/
     /*Output for Task2.4*/
     fflush(stdin);
     fprintf(fptrsta, "id,pollution,duration\n"); 
     statistics(1, rec, n, "Good");
     statistics(2, rec, n, "Moderate");
     statistics(3, rec, n, "Slightly unhealthy");
     statistics(4, rec, n, "Unhealthy");
     statistics(5, rec, n, "Very unhealthy");
     statistics(6, rec, n, "Hazardous");
     statistics(7, rec, n, "Extremely hazardous");
     for(int i = 0; i < 7 * n; i++){
          fprintf(fptrsta,"%d,%s,%d\n", ST[i + 1].id, ST[i + 1].pol, ST[i + 1].hour);
     }
     fclose(fptrsta);
     /*dust_summary aka Task2.3*/
     /*Calculating time interval*/
     int carry = 0;
     double secbuf = DS[1 + n].sec - DS[1].sec;
     if(secbuf < 0){
          secbuf += 60;
          carry = 1;
     }
     else 
          carry = 0;
     double minbuf = DS[1 + n].min - DS[1].min - carry;
     if(minbuf < 0){
          minbuf += 60;
          carry = 1;
     }
     else 
          carry = 0;
     int hourbuf = DS[1 + n].hour - DS[1].hour - carry;
     if(hourbuf < 0){
          minbuf += 24;
          carry = 1;
     }
     else
          carry = 0;
     double interval = ceil((((records - 1) / n ) - 1) * (hourbuf + minbuf / 60 + secbuf / 3600));
     /*Searching for max, min and mean*/
     for (int j = 0; j < n; j++){
          max = 0;
          min = 600;
          sum2 = 0;
          count2 = 0;
          for(int i = 0; i < records - 1; i++){
               if ((DS[i + 1].id == j + 1) && (DS[i + 1].val >= 5) && (DS[i + 1].val <= 550.5)){
                    sum2 += DS[i + 1].val;
                    count2++;
                    if(max < DS[i + 1].val) {
                         max = DS[i + 1].val;
                         SM[3 * j + 1].year = DS[i + 1].year;
                         SM[3 * j + 1].month = DS[i + 1].month;
                         SM[3 * j + 1].day = DS[i + 1].day;
                         SM[3 * j + 1].hour = DS[i + 1].hour;
                         SM[3 * j + 1].min = DS[i + 1].min;
                         SM[3 * j + 1].sec = DS[i + 1].sec;
                         SM[3 * j + 1].id = DS[i + 1].id;
                         strcpy(SM[3 * j + 1].title, "max");
                    }
                    if(min > DS[i + 1].val) {
                         min = DS[i + 1].val;
                         SM[3 * j + 2].year = DS[i + 1].year;
                         SM[3 * j + 2].month = DS[i + 1].month;
                         SM[3 * j + 2].day = DS[i + 1].day;
                         SM[3 * j + 2].hour = DS[i + 1].hour;
                         SM[3 * j + 2].min = DS[i + 1].min;
                         SM[3 * j + 2].sec = DS[i + 1].sec;
                         SM[3 * j + 2].id = DS[i + 1].id;
                         strcpy(SM[3 * j + 2].title, "min");
                    }
               }
          }
          SM[3 * j + 3].val = sum2 / count2;
          SM[3 * j + 1].val = max;
          SM[3 * j + 2].val = min;
          SM[3 * j + 3].id = j + 1;
          strcpy(SM[3 * j + 3].title, "mean");
     }
     /*Output for Task2.3*/
     fflush(stdin);
     fprintf(fptrsum, "id,parameters,time,value\n"); 
     for(int i = 0; i < 3 * n; i++){
          if (SM[i + 1].year == 0)
               fprintf(fptrsum,"%d,%s,%.0lf:00:00,%.1lf\n", SM[i + 1].id, SM[i + 1].title,interval, SM[i + 1].val); 
          else
               fprintf(fptrsum,"%d,%s,%02d:%02d:%02d %02d:%02d:%02d,%.1lf\n", SM[i + 1].id, SM[i + 1].title, SM[i + 1].year, SM[i + 1].month, SM[i + 1].day,
                                                                           SM[i + 1].hour, SM[i + 1].min, SM[i + 1].sec, SM[i + 1].val);
     }
     fclose(fptrsum); 
     fclose(fptrerror);
}
int main(int argc, char *argv[])
{    
     process(argc, argv);
     return(0);
}