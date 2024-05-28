#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/*all months in a leap year*/
int leap[13]={0,31,29,31,30,31,30,31,31,30,31,30,31};
/*all months in a non-leap year*/
int nonleap[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
/*"leap year checking" subroutine*/
int leapyear(int year)
{
  return ((year % 4 == 0 && year % 100 != 0) || ( year % 400 == 0));
}
/*dust_header*/
struct dusthdr{
    unsigned char start_byte;           /*1 byte*/
    unsigned char packet_length;        /*1 byte*/
    unsigned char ID;                   /*1 byte*/
    union{
        unsigned int time_val;  
        struct{
            unsigned char time1 : 8;    /*1 byte*/
            unsigned char time2 : 8;    /*1 byte*/
            unsigned char time3 : 8;    /*1 byte*/
            unsigned char time4 : 8;    /*1 byte*/
        }time4bytes;
    }Time;                              /*4 bytes*/
    union{
        unsigned int pm25_val;
        struct{
            unsigned char pm251 : 8;    /*1 byte*/
            unsigned char pm252 : 8;    /*1 byte*/
            unsigned char pm253 : 8;    /*1 byte*/
            unsigned char pm254 : 8;    /*1 byte*/
        }pm254bytes;
    }PM25;                              /*4 bytes*/
    union{
        unsigned short int aqi_val;
        struct{
            unsigned char aqil : 8;     /*1 byte*/
            unsigned char aqih : 8;     /*1 byte*/
        }aqihl;
    }AQI;                               /*2 bytes*/
    union{
        unsigned char checksum_val;
        struct{
            unsigned char checksuml : 8; /*1 byte*/
            unsigned char checksumh : 8; /*1 byte*/
        }checksum2bytes;
    }Checksum;                          /*2 bytes*/
    unsigned char stop_byte;            /*1 byte*/
};
dusthdr ds[10001];
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
/*"IEEE754 separate" subroutine*/
dust AV[10001];
union rawfloat{
    float f;
    struct{
        unsigned int mantissa : 23;     /*23 bits*/
        unsigned int exponent : 8;      /*8 bits*/
        unsigned int sign : 1;          /* 1 bit */
    }raw;
};
/*"UNIX time converter" subroutine*/
unsigned int unix_time(int y, int mn, int d, int h, int m, int s){
    int count = 0;
    for(int i = 1970; i < y; i++){
        if(leapyear(i)) 
            count++;
    }
    int tyd = 0;   
    for(int i = 1; i < mn; i++){
        if(leapyear(y))
            tyd += leap[i];
        else
            tyd += nonleap[i];
    }
    unsigned int sec = ((count * 366) + (y - 1970 - count) * 365 + tyd + d - 1) * 24 * 3600 + h * 3600 + m * 60 + s; /*4 bytes*/
    return sec;
}
/*"n-bit binary number in string" subroutine*/
void printBinary(int n, int i, char *iee)
{   
    int k;
    for (k = i - 1; k >= 0; k--) {
        if ((n >> k) & 1)
            strcat(iee, "1");
        else
            strcat(iee, "0");
    }
}
/*"IEEE754 convert" subroutine*/
void IEEE754(rawfloat var, char *iee)
{
    if(var.f > 0)
        strcat(iee, "0");
    else
        strcat(iee, "1"); 
    printBinary(var.raw.exponent, 8, iee);
    printBinary(var.raw.mantissa, 23, iee);
}
/*"Binary in string to number" subroutine*/
unsigned int bintohex(char *bin){
    char *a = bin;
    unsigned int num = 0;
    do{
        int b = *a == '1' ? 1 : 0;
        num = (num << 1) | b;
        a++;
    } while(*a);
    return num;
}
void convert(int argc, char *argv[]){
    /*Working with file*/
    FILE *fptrin;
    FILE *fptrout;
    FILE *fptrerror;
   	fptrerror = fopen("task3.log", "w");
   	if(fptrerror == NULL){
          printf("Error!");
          exit(1);
   	}
    if((argc <= 1)){
        printf("command line: %s [data_filename.csv] [hex_filename.dat]\n", argv[0]);
        exit(0);
    }
    if((argc < 3) && (argc != 1)){
        printf("command line: %s [data_filename.csv] [hex_filename.dat]\n", argv[0]);
        fprintf(fptrerror, "Error 00: invalid command");
        exit(1);
    }
    if(argc > 3){
        printf("command line: %s [data_filename.csv] [hex_filename.dat]\n", argv[0]);
        fprintf(fptrerror, "Error 00: invalid command");
        exit(1);
    }
    if(argc == 3){
   	    fptrin = fopen(argv[1], "rt");
        if(fptrin == NULL){
            fprintf(fptrerror, "Error 01: file not found or cannot be accessed");
            exit(1);
   	    }
        if(strcmp(strrchr(argv[1], '.'), ".csv" )){
            fprintf(fptrerror, "Error 02: invalid csv file");
            exit(1);
        }
        fptrout = fopen(argv[2], "w");
   	    if(fptrout == NULL){
            fprintf(fptrerror, "Error 04: cannot override hex_filename.dat");
            exit(1);
   	    }
    }
    /*Input*/
    int reads = 0;
    int records = 0;
    reads = fscanf(fptrin,"%s",AV[records].title);
    if(reads == 1)      
        records++;
    if(reads != 1 && !feof(fptrin)){
        fprintf(fptrerror, "Error 02: invalid csv file\n"); 
        fprintf(fptrerror, "Error 03: data is missing at line %d\n", records);    
        exit(1);
    }
    do{
        fflush stdin;
        reads = fscanf(fptrin,"%d,%d:%d:%d %d:%d:%d,%lf,%lf,%69[^\n]\n",&AV[records].id, &AV[records].year, &AV[records].month, 
                                                                        &AV[records].day, &AV[records].hour, &AV[records].min, 
                                                                        &AV[records].sec, &AV[records].avr, &AV[records].aqi, AV[records].pol);
        if(reads == 10){
            if((AV[records].id < 0) 
               || (AV[records].sec < 0) 
               || (AV[records].sec >= 60) 
               || (AV[records].min < 0) || (AV[records].min >= 60) 
               || (AV[records].hour < 0) || (AV[records].hour >= 24) 
               || (AV[records].day < 0) || (AV[records].month < 0) 
               || (AV[records].month >= 12) || (AV[records].year < 0) || (AV[records].aqi < 0)){
                fprintf(fptrerror, "Error 03: data is missing at line %d", records);
                exit(1);
            }
            if(leapyear(AV[records].year)){
                if(AV[records].day > leap[AV[records].month]){
                    fprintf(fptrerror, "Error 03: data is missing at line %d", records);
                    exit(1);
                }
            }
            if(!leapyear(AV[records].year)){
                if(AV[records].day > nonleap[AV[records].month]){
                    fprintf(fptrerror, "Error 03: data is missing at line %d", records);
                    exit(1);
                } 
            }
            if(ferror(fptrin)){
                fprintf(fptrerror, "Error 02: invalid csv file");
                exit(1);
   	        }      
            records ++;
        }
            if(reads != 10 && !feof(fptrin)){
            fprintf(fptrerror, "Error 02: invalid csv file\n");
            fprintf(fptrerror, "Error 03: data is missing at line %d", records);
            exit(1);
        }
        if(ferror(fptrin)){
            fprintf(fptrerror, "Error 02: invalid csv file");
            exit(1);
   	    }
    } while(!feof(fptrin));
    if(records < 2){
        fprintf(fptrerror, "Error 02: invalid csv file\n");
        fprintf(fptrerror, "Error 03: data is missing at line %d\n", records);
        exit(1);
    }
    fclose(fptrin);
    char iee[69];
    strcpy(iee,"");
    rawfloat var;
    /*Output*/  
    for(int i = 0; i < records - 1; i ++){
        var.f = AV[i + 1].avr;
        strcpy(iee, "");
        IEEE754(var, iee);
        ds[i + 1].PM25.pm25_val = bintohex(iee);
        ds[i + 1].start_byte = 0;
        ds[i + 1].stop_byte = 255;
        ds[i + 1].packet_length = 15;
        ds[i + 1].AQI.aqi_val = AV[i + 1].aqi;
        ds[i + 1].ID = AV[i + 1].id;
        ds[i + 1].Time.time_val = unix_time(AV[i + 1].year, AV[i + 1].month, AV[i + 1].day, AV[i + 1].hour, AV[i + 1].min, AV[i + 1].sec);
        ds[i + 1].Checksum.checksum_val = ds[i + 1].packet_length + ds[i + 1].ID + ds[i + 1].Time.time4bytes.time4 
                                          + ds[i + 1].Time.time4bytes.time3 + ds[i + 1].Time.time4bytes.time2 + ds[i + 1].Time.time4bytes.time1 
                                          + ds[i + 1].PM25.pm254bytes.pm254 + ds[i + 1].PM25.pm254bytes.pm253 + ds[i + 1].PM25.pm254bytes.pm252 
                                          + ds[i + 1].PM25.pm254bytes.pm251 + ds[i + 1].AQI.aqihl.aqih + ds[i + 1].AQI.aqihl.aqil;
        fprintf(fptrout,"%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n", 
                        ds[i + 1].start_byte, ds[i + 1].packet_length, ds[i + 1].ID, ds[i + 1].Time.time4bytes.time4, 
                        ds[i + 1].Time.time4bytes.time3, ds[i + 1].Time.time4bytes.time2, ds[i + 1].Time.time4bytes.time1, 
                        ds[i + 1].PM25.pm254bytes.pm254, ds[i + 1].PM25.pm254bytes.pm253, ds[i + 1].PM25.pm254bytes.pm252, 
                        ds[i + 1].PM25.pm254bytes.pm251, ds[i + 1].AQI.aqihl.aqih, ds[i + 1].AQI.aqihl.aqil, 
                        256 - ds[i + 1].Checksum.checksum2bytes.checksuml, ds[i + 1].stop_byte);
    }
    fclose(fptrout);
    fclose(fptrerror);
}
int main(int argc, char *argv[])
{    
    convert(argc, argv);
    return(0);
}
