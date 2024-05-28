#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

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
    int realcount2;
    char all2[69];
    char title2[69];
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
    }Checksum;
    union{
        unsigned char cchecksum_val;
        struct{
            unsigned char cchecksuml : 8; /*1 byte*/
            unsigned char cchecksumh : 8; /*1 byte*/
        }cchecksum2bytes;
    }CheckChecksum;                          /*2 bytes*/
    unsigned char stop_byte;            /*1 byte*/
};
dusthdr ds[10001];

struct dust{
    int realcount;
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
    char all[69];
};
dust AV[10001];

union rawfloat{
    float f;
    struct{
        unsigned int mantissa : 23;     /*23 bits*/
        unsigned int exponent : 8;      /*8 bits*/
        unsigned int sign : 1;          /*1 bit*/
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
    unsigned int sec = ((count * 366) + (y - 1970 - count) * 365 + tyd + d - 1) * 24 * 3600 + h * 3600 + m * 60 + s - 7 * 3600; /*4 bytes*/
    return sec;
}

/*"n-bit binary number in string" subroutine*/
void printBinary(int n, int i, char *iee)
{   
    int k;
    for (k = i - 1; k >= 0; k--){
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
    }while(*a);
    return num;
}

/*"IEEE754 convert 2" subroutine*/
unsigned int convertToFloat(char *iee, int low, int high)
{
    unsigned int f = 0, i;
    for (i = high; i >= low; i--){
        f = f + (iee[i] - 48) * pow(2, high - i);
    }
    return f;
}

double map(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void aqi(double x, int i){
	if((x >= 0) && (x < 12)){
		AV[i].aqi = round(map(x, 0, 12, 0, 50));
		strcpy(AV[i].pol, "Good");
	}
	if((x >= 12) && (x < 35.5)){
		AV[i].aqi = round(map(x, 12, 35.5, 50, 100));
		strcpy(AV[i].pol, "Moderate");
	}
	if((x >= 35.5) && (x < 55.5)){
		AV[i].aqi = round(map(x, 35.5, 55.5, 100, 150));
		strcpy(AV[i].pol, "Slightly unhealthy");
	}
	if((x >= 55.5) && (x < 150.5)){
		AV[i].aqi = round(map(x, 55.5, 150.5, 150, 200));
		strcpy(AV[i].pol, "Unhealthy");
	}
	if((x >= 150.5) && (x < 250.5)){
		AV[i].aqi = round(map(x, 150.5, 250.5, 200, 300));
		strcpy(AV[i].pol, "Very unhealthy");
	}
	if((x >= 250.5) && (x < 350.5)){
		AV[i].aqi = round(map(x, 250.5, 350.5, 300, 400));
		strcpy(AV[i].pol, "Hazardous");
	}
	if((x >= 350.5) && (x <= 550.5)){
		AV[i].aqi = round(map(x, 350.5, 550.5, 400, 500));
		strcpy(AV[i].pol, "Extremely hazardous");
	}
}
  
void swapint(int* x, int* y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

void swapdouble(double *x, double *y)
{
    double temp = *x;
    *x = *y;
    *y = temp;
}
/*Bubble Sort Algorithm*/
void bubbleSort_int_up(int arr[], int a[], int b[], double c[], int n)
{
    int i, j;
    for (i = 0; i < n - 1; i++)
        for (j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j + 1]){
                swapint(&b[j], &b[j + 1]);
                swapint(&arr[j], &arr[j + 1]);
                swapint(&a[j], &a[j + 1]);
                swapdouble(&c[j], &c[j + 1]);
            }
}

void bubbleSort_double_up(double arr[], int a[], int b[], int c[], int n)
{
    int i, j;
    for (i = 0; i < n - 1; i++)
        for (j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j + 1]){
                swapint(&b[j], &b[j + 1]);
                swapdouble(&arr[j], &arr[j + 1]);
                swapint(&a[j], &a[j + 1]);
                swapint(&c[j], &c[j + 1]);
            }
}

void bubbleSort_int_down(int arr[], int a[], int b[], double c[], int n)
{
    int i, j;
    for (i = 0; i < n - 1; i++)
        for (j = 0; j < n - i - 1; j++)
            if (arr[j] < arr[j + 1]){
                swapint(&b[j], &b[j + 1]);
                swapint(&arr[j], &arr[j + 1]);
                swapint(&a[j], &a[j + 1]);
                swapdouble(&c[j], &c[j + 1]);
            }
}

void bubbleSort_double_down(double arr[], int a[], int b[], int c[], int n)
{
    int i, j;
    for (i = 0; i < n - 1; i++)
        for (j = 0; j < n - i - 1; j++)
            if (arr[j] < arr[j + 1]){
                swapint(&b[j], &b[j + 1]);
                swapdouble(&arr[j], &arr[j + 1]);
                swapint(&a[j], &a[j + 1]);
                swapint(&c[j], &c[j + 1]);
            }
}

/*Selection Sort Algorithm*/
void selectionSort_int_up(int arr[], int a[], int b[], double c[], int n){ 
    int i, j, min_idx; 
    for(i = 0; i < n-1; i++){
        min_idx = i; 
        for(j = i + 1; j < n; j++){
            if(arr[j] < arr[min_idx]) 
                min_idx = j;
        }
        if(min_idx != i){
            swapint(&b[min_idx], &b[i]);
            swapint(&arr[min_idx], &arr[i]);
            swapint(&a[min_idx], &a[i]);
            swapdouble(&c[min_idx], &c[i]);
        }
    }
}

void selectionSort_double_up(double arr[], int a[], int b[], int c[], int n){ 
    int i, j, min_idx; 
    for (i = 0; i < n-1; i++){
        min_idx = i; 
        for(j = i + 1; j < n; j++){
            if(arr[j] < arr[min_idx]) 
                min_idx = j;
        }
        if(min_idx != i){
            swapint(&b[min_idx], &b[i]);
            swapdouble(&arr[min_idx], &arr[i]);
            swapint(&a[min_idx], &a[i]);
            swapint(&c[min_idx], &c[i]);
        }
    }
}

void selectionSort_int_down(int arr[], int a[], int b[], double c[], int n){ 
    int i, j, max_idx; 
    for(i = 0; i < n-1; i++){
        max_idx = i; 
        for(j = i + 1; j < n; j++){
            if(arr[j] > arr[max_idx]) 
                max_idx = j;
        }
        if(max_idx != i){
            swapint(&b[max_idx], &b[i]);
            swapint(&arr[max_idx], &arr[i]);
            swapint(&a[max_idx], &a[i]);
            swapdouble(&c[max_idx], &c[i]);
        }
    }
}

void selectionSort_double_down(double arr[], int a[], int b[], int c[], int n){ 
    int i, j, max_idx; 
    for (i = 0; i < n-1; i++){
        max_idx = i; 
        for(j = i + 1; j < n; j++){
            if(arr[j] > arr[max_idx]) 
                max_idx = j;
        }
        if(max_idx != i){
            swapint(&b[max_idx], &b[i]);
            swapdouble(&arr[max_idx], &arr[i]);
            swapint(&a[max_idx], &a[i]);
            swapint(&c[max_idx], &c[i]);
        }
    }
}

void convert(int argc, char *argv[]){
    /*Working with file*/
    FILE *fptrin;
    FILE *fptrout;
    FILE *fptrerror;
    FILE *fptrrun;
    fptrrun = fopen("dust_convert_run.log", "w");
   	if(fptrrun == NULL){
        printf("Error!");
        exit(1);
   	}
   	fptrerror = fopen("dust_convert_error.log", "w");
   	if(fptrerror == NULL){
        printf("Error!");
        exit(1);
   	}
    int reads = 0;
    int records = 0;
    int rerecords = 0;
    int errorcount = 0;
    int dup;
    char line[1000];
    time_t epch;
    tm *ut;
    time_t now = time(NULL);
    tm *t = localtime(&now);
    char ieee[69];
    if((argc <= 1)){
        printf("command line: %s [input_filename] [output_filename]\n", argv[0]);
        exit(0);
    }
    if((argc < 3) && (argc != 1)){
        printf("command line: %s [input_filename] [output_filename]\n", argv[0]);
        fprintf(fptrerror, "Error 01: invalid command");
        exit(1);
    }
    if(argc >= 3){
        fptrin = fopen(argv[1], "rt");
        if(fptrin == NULL){
            fprintf(fptrerror, "Error 02: denied access %s", argv[1]);
            exit(1);
   	    }
        fptrout = fopen(argv[2], "w");
   	    if(fptrout == NULL){
            fprintf(fptrerror, "Error 02: denied access %s", argv[2]);
            exit(1);
   	    }
        /*Text to Hex*/
        if(!strcmp(strrchr(argv[1], '.'), ".csv")){
            if(argc >3){
                printf("command line: %s [data_filename.csv] [hex_filename.dat]\n", argv[0]);
                fprintf(fptrerror, "Error 01: invalid command");
                exit(1);
            }
            while(fgets(line, sizeof(line), fptrin) != NULL){
                char *buf = line;
                sscanf(buf,"%69[^\n]\n",AV[rerecords].all);
                reads = sscanf(buf,"%d,%d:%d:%d %d:%d:%d,%lf,%lf,%69[^\n]\n",&AV[records].id, &AV[records].year, &AV[records].month, 
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
                            fprintf(fptrerror, "Error 04: data is missing at line %d\n", rerecords);
                            AV[rerecords].realcount = 1;
                            rerecords++;
                            errorcount++;
                            continue;
                    }
                    if(leapyear(AV[records].year)){
                        if(AV[records].day > leap[AV[records].month]){
                            fprintf(fptrerror, "Error 04: data is missing at line %d\n", rerecords);
                            AV[rerecords].realcount = 1;
                            rerecords++;
                            errorcount++;
                            continue;
                        }
                    }
                    if(!leapyear(AV[records].year)){
                        if(AV[records].day > nonleap[AV[records].month]){
                            fprintf(fptrerror, "Error 04: data is missing at line %d\n", rerecords);
                            AV[rerecords].realcount = 1;
                            rerecords++;
                            errorcount++;
                            continue;
                        }    
                    }
                    double fakekaqi;
                    if((AV[records].avr >= 0) && (AV[records].avr < 12))
		                fakekaqi = round(map(AV[records].avr, 0, 12, 0, 50));
	                if((AV[records].avr >= 12) && (AV[records].avr < 35.5))
		                fakekaqi = round(map(AV[records].avr, 12, 35.5, 50, 100));
	                if((AV[records].avr >= 35.5) && (AV[records].avr < 55.5))
		                fakekaqi = round(map(AV[records].avr, 35.5, 55.5, 100, 150));
	                if((AV[records].avr >= 55.5) && (AV[records].avr < 150.5))
		                fakekaqi = round(map(AV[records].avr, 55.5, 150.5, 150, 200));
	                if((AV[records].avr >= 150.5) && (AV[records].avr < 250.5))
		                fakekaqi = round(map(AV[records].avr, 150.5, 250.5, 200, 300));
                    if((AV[records].avr >= 250.5) && (AV[records].avr < 350.5))
                        fakekaqi = round(map(AV[records].avr, 250.5, 350.5, 300, 400));
                    if((AV[records].avr >= 350.5) && (AV[records].avr <= 550.5))
                        fakekaqi = round(map(AV[records].avr, 350.5, 550.5, 400, 500));
                    if(AV[records].avr > 550.5){
                        fprintf(fptrerror, "Error 04: data is missing at line %d\n", rerecords);
                        AV[rerecords].realcount = 1;
                        rerecords++;
                        errorcount++;
                        continue;
                    }
                    if(abs(AV[records].aqi - fakekaqi) >= 1.001){
                        fprintf(fptrerror, "Error 04: data is missing at line %d\n", rerecords);
                        AV[rerecords].realcount = 1;
                        rerecords++;
                        errorcount++;
                        continue;
                    } 
                    if(ferror(fptrin)){
                        fprintf(fptrerror, "Error 03: invalid fileformat");
                        exit(1);
   	                }     
                    sscanf(buf,"%69[^\n]\n",AV[records].title);
                    AV[rerecords].realcount = 0;
                    records++;
                    rerecords++;    
                }
                if((reads != 10) && !feof(fptrin)){
                    if(rerecords != 0){
                        fprintf(fptrerror, "Error 04: data is missing at line %d\n", rerecords);
                        errorcount++;
                    }
                    AV[rerecords].realcount = 1;
                    rerecords++;                  
                    continue;
                }
                if(ferror(fptrin)){
                    fprintf(fptrerror, "Error 03: invalid fileformat");
                    exit(1);
   	            }
            }
            if(rerecords < 2){
                fprintf(fptrerror, "Error 04: data is missing at line %d\n", rerecords);
                exit(1);
            }
            fclose(fptrin);
            char iee[69];
            strcpy(iee,"");
            rawfloat var;
            /*Output to DAT file*/  
            dup = 0;
            for(int i = 0; i < rerecords - 1; i++){
                if(AV[i + 1].realcount == 1)
                    continue;
                for(int j = 0; j < i ; j++){
                    if(!strcmp(AV[j + 1].all, AV[i + 1].all)){
                        fprintf(fptrerror, "Error 05: duplicated data at lines %d %d\n", j + 1, i + 1);
                        dup++; 
                        errorcount++;       
                        break;             
                    }
                }
                if(dup != 0){
                    continue;
                }
            }         
            for(int i = -1; i < records - 1; i ++){
                dup = 0;
                for(int j = -1; j < i ; j++){
                    if(!strcmp(AV[j + 1].title, AV[i + 1].title)){
                        dup++;                     
                    }
                }
                if(dup != 0){
                    continue;
                }         
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
                ds[i + 1].Checksum.checksum_val = 65536 - ds[i + 1].Checksum.checksum_val;
                fprintf(fptrout,"%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n", 
                                                ds[i + 1].start_byte, ds[i + 1].packet_length, ds[i + 1].ID, ds[i + 1].Time.time4bytes.time4, 
                                                ds[i + 1].Time.time4bytes.time3, ds[i + 1].Time.time4bytes.time2, ds[i + 1].Time.time4bytes.time1, 
                                                ds[i + 1].PM25.pm254bytes.pm254, ds[i + 1].PM25.pm254bytes.pm253, ds[i + 1].PM25.pm254bytes.pm252, 
                                                ds[i + 1].PM25.pm254bytes.pm251, ds[i + 1].AQI.aqihl.aqih, ds[i + 1].AQI.aqihl.aqil, 
                                                ds[i + 1].Checksum.checksum2bytes.checksuml, ds[i + 1].stop_byte);
            }
            fclose(fptrout);
            fclose(fptrerror);
            fprintf(fptrrun, "Total number of rows: %d\n", rerecords);
            fprintf(fptrrun, "Succesfully converted rows %d\n", rerecords - errorcount - 1);
            fprintf(fptrrun, "Error rows %d\n", errorcount);
            fclose(fptrrun);
        }
        else if(!strcmp(strrchr(argv[1], '.'), ".dat")){
            if(argc > 4){
                if(!strcmp(argv[3], "-s")){
                    if(argc == 5){
                        if(((strcmp(argv[4], "-asc")) && (strcmp(argv[4], "-dsc")))){
                            printf("command line: %s [hex_filename.dat] [data_filename.csv] -s [params] [-asc/-dsc]\n", argv[0]);
                            fprintf(fptrerror, "Error 01: invalid command");
                            exit(1);
                        }
                    }
                    else if(argc > 5){
                        if(argc == 6){
                            if((strcmp(argv[5], "-asc")) && (strcmp(argv[5], "-dsc"))){
                                printf("command line: %s [hex_filename.dat] [data_filename.csv] -s [params] [-asc/-dsc]\n", argv[0]);
                                fprintf(fptrerror, "Error 01: invalid command");
                                exit(1);
                            }
                            if((strcmp(argv[4], "time")) && (strcmp(argv[4], "values")) && (strcmp(argv[4], "id"))
                                && (strcmp(argv[4], "id,values")) && (strcmp(argv[4], "time,id") && (strcmp(argv[4], "time,values")))
                                && (strcmp(argv[4], "values,id")) && (strcmp(argv[4], "values,time")) && (strcmp(argv[4], "id,time"))
                                && (strcmp(argv[4], "time,id,values")) && (strcmp(argv[4], "time,values,id")) && (strcmp(argv[4], "values,id,time"))
                                && (strcmp(argv[4], "values,time,id")) && (strcmp(argv[4], "id,time,values")) && (strcmp(argv[4], "id,values,time"))){
                                printf("command line: %s [hex_filename.dat] [data_filename.csv] -s [params] [-asc/-dsc]\n", argv[0]);
                                fprintf(fptrerror, "Error 01: invalid command");
                                exit(1);
                            }
                        }
                        if(argc > 6){
                            printf("command line: %s [hex_filename.dat] [data_filename.csv] -s [params] [-asc/-dsc]\n", argv[0]);
                            fprintf(fptrerror, "Error 01: invalid command");
                            exit(1);
                        }
                    }
                }
                else{
                    printf("command line: %s [hex_filename.dat] [data_filename.csv] -s [params] [-asc/-dsc]\n", argv[0]);
                    fprintf(fptrerror, "Error 01: invalid command");
                    exit(1);
                }           
            }
            else if(argc == 4){
                printf("command line: %s [hex_filename.dat] [data_filename.csv] -s [params] [-asc/-dsc]\n", argv[0]);
                fprintf(fptrerror, "Error 01: invalid command");
                exit(1);
            }
            records = 1;
            unsigned char tmp1[10001];
            unsigned char tmp2[10001];
            unsigned char tmp3[10001];
            unsigned char tmp4[10001];
            unsigned char tmp5[10001];
            unsigned char tmp6[10001];
            unsigned char tmp7[10001];
            unsigned char tmp8[10001];
            unsigned char tmp9[10001];
            unsigned char tmp10[10001];
            unsigned char tmp11[10001];
            int idtmp[10001];
            int idtmp2[10001];
            int idtmp0[10001] = {};
            int timetmp[10001];
            int timetmp2[10001];
            int timetmp0[10001] = {};
            double valtmp[10001];
            double valtmp2[10001];
            double valtmp0[10001] = {};
            int countmp[10001];
            int countmp2[10001];
            char p1[69];
            char p2[69];
            char p3[69];
            while(fgets(line, sizeof(line), fptrin) != NULL){
                char *buf = line;
                sscanf(buf,"%100[^\n]\n",ds[rerecords].all2);
                reads = sscanf(buf,"%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", 
                                                &ds[records].start_byte, &ds[records].packet_length, &ds[records].ID, &tmp1[records], 
                                                &tmp2[records], &tmp3[records], &tmp4[records], 
                                                &tmp5[records], &tmp6[records], &tmp7[records], 
                                                &tmp8[records], &tmp9[records], &tmp10[records], 
                                                &tmp11[records], &ds[records].stop_byte);
                if(reads == 15){
                    if((ds[records].start_byte != 0) || (ds[records].stop_byte != 255) || (ds[records].packet_length != 15)){
                        fprintf(fptrerror, "Error 06: invalid data packet at line %d\n", rerecords + 1);
                        rerecords++;
                        errorcount++;
                        continue;
                    } 
                    if(ferror(fptrin)){
                        fprintf(fptrerror, "Error 03: invalid fileformat");
                        exit(1);
   	                }
                    sscanf(buf,"%100[^\n]\n",ds[records].title2);
                    ds[records].Time.time4bytes.time4 = tmp1[records];
                    ds[records].Time.time4bytes.time3 = tmp2[records];
                    ds[records].Time.time4bytes.time2 = tmp3[records];
                    ds[records].Time.time4bytes.time1 = tmp4[records];
                    ds[records].PM25.pm254bytes.pm254 = tmp5[records];
                    ds[records].PM25.pm254bytes.pm253 = tmp6[records];
                    ds[records].PM25.pm254bytes.pm252 = tmp7[records];
                    ds[records].PM25.pm254bytes.pm251 = tmp8[records];
                    ds[records].AQI.aqihl.aqih = tmp9[records];
                    ds[records].AQI.aqihl.aqil = tmp10[records];
                    ds[records].Checksum.checksum2bytes.checksuml = tmp11[records];
                    ds[records].CheckChecksum.cchecksum_val = ds[records].packet_length + ds[records].ID + ds[records].Time.time4bytes.time4 
                                + ds[records].Time.time4bytes.time3 + ds[records].Time.time4bytes.time2 + ds[records].Time.time4bytes.time1 
                                + ds[records].PM25.pm254bytes.pm254 + ds[records].PM25.pm254bytes.pm253 + ds[records].PM25.pm254bytes.pm252 
                                + ds[records].PM25.pm254bytes.pm251 + ds[records].AQI.aqihl.aqih + ds[records].AQI.aqihl.aqil;
                    ds[records].CheckChecksum.cchecksum_val = 65536 - ds[records].CheckChecksum.cchecksum_val;
                    if(ds[records].CheckChecksum.cchecksum2bytes.cchecksuml != ds[records].Checksum.checksum2bytes.checksuml){
                        fprintf(fptrerror, "Error 06: invalid data packet at line %d\n", rerecords + 1);
                        rerecords++;
                        errorcount++;
                        continue;
                    }
                    if(ds[records].Time.time_val > unix_time(1900 + t->tm_year, 1+ t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec)){
                        fprintf(fptrerror, "Error 06: invalid data packet at line %d\n", rerecords + 1);
                        rerecords++;
                        errorcount++;
                        continue;
                    }
                    ds[records].realcount2 = rerecords;
                    records++;
                    rerecords++;    
                }
                if((reads != 15) && !feof(fptrin)){
                    fprintf(fptrerror, "Error 06: invalid data packet at line %d\n", rerecords + 1);
                    rerecords++;
                    errorcount++;
                    continue;
                }
                if(ferror(fptrin)){
                    fprintf(fptrerror, "Error 03: invalid fileformat");
                    exit(1);
   	            }
            }
            if(rerecords < 2){
                fprintf(fptrerror, "Error 06: invalid data packet at line %d\n", rerecords + 1);
                exit(1);
            }
            fclose(fptrin);
            fflush stdin;
            fprintf(fptrout, "id,time,values,aqi,pollution\n");
            for(int i = 1; i < records; i ++){
                dup = 0;
                for(int j = 0; j < i ; j++){
                    if(!strcmp(ds[j].title2, ds[i].title2)){
                        dup++; 
                        fprintf(fptrerror, "Error 05: duplicated data at lines %d %d\n", ds[j].realcount2 + 1, ds[i].realcount2 + 1);    
                        errorcount++;
                        break;
                    }
                    
                }
                if(dup != 0){
                    continue;
                }
                strcpy(ieee, "");
                printBinary(ds[i].PM25.pm25_val,32,ieee);
                rawfloat bien;
                bien.raw.mantissa = convertToFloat(ieee, 9, 31);
                bien.raw.exponent = convertToFloat(ieee, 1, 8);
                bien.raw.sign = ieee[0] - 48;
                AV[i].avr = bien.f;
                aqi(AV[i].avr, i);
                if(abs(AV[i].aqi - ds[i].AQI.aqi_val) >= 1.001){
                    fprintf(fptrerror, "Error 06: invalid data packet at line %d\n", ds[i].realcount2 + 1);
                    errorcount++;
                    AV[i].id = 0;
                    continue;
                }   
                AV[i].id = ds[i].ID;
                epch = ds[i].Time.time_val;
                ut = localtime(&epch);
                AV[i].year = 1900 + ut->tm_year;
                AV[i].month = 1 + ut->tm_mon;
                AV[i].day = ut->tm_mday;
                AV[i].hour = ut->tm_hour;
                AV[i].min = ut->tm_min;
                AV[i].sec = ut->tm_sec;
                idtmp[i] = AV[i].id;
                idtmp2[i] = AV[i].id;
                valtmp[i] = AV[i].avr;
                valtmp2[i] = AV[i].avr;
                timetmp[i] = unix_time(AV[i].year, AV[i].month, AV[i].day, AV[i].hour, AV[i].min, AV[i].sec);
                timetmp2[i] = timetmp[i];
                countmp[i] = i;
                countmp2[i] = i;
                if(argc == 3)
                    fprintf(fptrout,"%d,%02d:%02d:%02d %02d:%02d:%02d,%.1lf,%.0lf,%s\n", AV[i].id, AV[i].year, AV[i].month, AV[i].day, AV[i].hour, 
                                                                                         AV[i].min, AV[i].sec, AV[i].avr, AV[i].aqi, AV[i].pol);
            }
            clock_t start1, start2, end1, end2;
            if(argc == 5){
                start1 = clock();
                bubbleSort_double_up(valtmp, idtmp, timetmp, countmp, records);
                bubbleSort_int_up(timetmp, idtmp, countmp, valtmp, records);
                bubbleSort_int_up(idtmp, timetmp, countmp, valtmp, records);
                end1 = clock(); 
                if(!strcmp(argv[4], "-asc")){
                    printf("Default: %s %s %s -s id,time,values %s\n", argv[0], argv[1], argv[2], argv[4]);
                    for(int i = 0; i <= records; i++){
                        if(AV[countmp[i]].id == 0)
                            continue;
                        fprintf(fptrout,"%d,%02d:%02d:%02d %02d:%02d:%02d,%.1lf,%.0lf,%s\n", AV[countmp[i]].id, AV[countmp[i]].year, AV[countmp[i]].month, AV[countmp[i]].day, AV[countmp[i]].hour, AV[countmp[i]].min, AV[countmp[i]].sec, AV[countmp[i]].avr, AV[countmp[i]].aqi, AV[countmp[i]].pol); 
                    }                      
                }
                if(!strcmp(argv[4], "-dsc")){
                    printf("Default: %s %s %s -s id,time,values %s\n", argv[0], argv[1], argv[2], argv[4]);
                    for(int i = records; i >= 0; i--){
                        if(AV[countmp[i]].id == 0)
                            continue;
                        fprintf(fptrout,"%d,%02d:%02d:%02d %02d:%02d:%02d,%.1lf,%.0lf,%s\n", AV[countmp[i]].id, AV[countmp[i]].year, AV[countmp[i]].month, AV[countmp[i]].day, AV[countmp[i]].hour, AV[countmp[i]].min, AV[countmp[i]].sec, AV[countmp[i]].avr, AV[countmp[i]].aqi, AV[countmp[i]].pol); 
                    }
                }
            }
            if(argc == 6){
                reads = sscanf(argv[4],"%69[^,],%69[^,],%69[^,]", p1, p2, p3); 
                start1 = clock();          
                if(!strcmp(argv[5], "-asc")){
                    if(reads == 1){
                        if(!strcmp(p1, "id"))
                            bubbleSort_int_up(idtmp, timetmp0, countmp, valtmp0, records);
                        if(!strcmp(p1, "time"))
                            bubbleSort_int_up(timetmp, idtmp0, countmp, valtmp0, records);
                        if(!strcmp(p1, "values"))
                            bubbleSort_double_up(valtmp, idtmp0, timetmp0, countmp, records);
                    }
                    if(reads == 2){
                        if(!strcmp(p1, "id") && !strcmp(p2, "time")){
                            bubbleSort_int_up(timetmp, idtmp, countmp, valtmp0, records);
                            bubbleSort_int_up(idtmp, timetmp, countmp, valtmp0, records);                          
                        }
                        if(!strcmp(p1, "id") && !strcmp(p2, "values")){
                            bubbleSort_double_up(valtmp, idtmp, timetmp0, countmp, records);
                            bubbleSort_int_up(idtmp, timetmp0, countmp, valtmp, records);                          
                        }
                        if(!strcmp(p1, "time") && !strcmp(p2, "id")){
                            bubbleSort_int_up(idtmp, timetmp, countmp, valtmp0, records);
                            bubbleSort_int_up(timetmp, idtmp, countmp, valtmp0, records);                          
                        }
                        if(!strcmp(p1, "time") && !strcmp(p2, "values")){
                            bubbleSort_double_up(valtmp, idtmp0, timetmp, countmp, records);
                            bubbleSort_int_up(timetmp, idtmp0, countmp, valtmp, records);                         
                        }
                        if(!strcmp(p1, "values") && !strcmp(p2, "id")){
                            bubbleSort_int_up(idtmp, timetmp0, countmp, valtmp, records); 
                            bubbleSort_double_up(valtmp, idtmp, timetmp0, countmp, records);                          
                        }
                        if(!strcmp(p1, "values") && !strcmp(p2, "time")){
                            bubbleSort_int_up(timetmp, idtmp0, countmp, valtmp, records); 
                            bubbleSort_double_up(valtmp, idtmp0, timetmp, countmp, records);                       
                        }
                    }
                    if(reads == 3){
                        if(!strcmp(p1, "id") && !strcmp(p2, "time") && !strcmp(p3, "values")){
                            bubbleSort_double_up(valtmp, idtmp, timetmp, countmp, records);
                            bubbleSort_int_up(timetmp, idtmp, countmp, valtmp, records);
                            bubbleSort_int_up(idtmp, timetmp, countmp, valtmp, records);
                        }
                        if(!strcmp(p1, "id") && !strcmp(p2, "values") && !strcmp(p3, "time")){
                            bubbleSort_int_up(timetmp, idtmp, countmp, valtmp, records);
                            bubbleSort_double_up(valtmp, idtmp, timetmp, countmp, records);                           
                            bubbleSort_int_up(idtmp, timetmp, countmp, valtmp, records);
                        }
                        if(!strcmp(p1, "time") && !strcmp(p2, "values") && !strcmp(p3, "id")){
                            bubbleSort_int_up(idtmp, timetmp, countmp, valtmp, records);
                            bubbleSort_double_up(valtmp, idtmp, timetmp, countmp, records);
                            bubbleSort_int_up(timetmp, idtmp, countmp, valtmp, records);
                        }
                        if(!strcmp(p1, "time") && !strcmp(p2, "id") && !strcmp(p3, "values")){
                            bubbleSort_double_up(valtmp, idtmp, timetmp, countmp, records);
                            bubbleSort_int_up(idtmp, timetmp, countmp, valtmp, records);
                            bubbleSort_int_up(timetmp, idtmp, countmp, valtmp, records);
                        }
                        if(!strcmp(p1, "values") && !strcmp(p2, "id") && !strcmp(p3, "time")){
                            bubbleSort_int_up(timetmp, idtmp, countmp, valtmp, records);
                            bubbleSort_int_up(idtmp, timetmp, countmp, valtmp, records);
                            bubbleSort_double_up(valtmp, idtmp, timetmp, countmp, records);                                                   
                        }
                        if(!strcmp(p1, "values") && !strcmp(p2, "time") && !strcmp(p3, "id")){
                            bubbleSort_int_up(idtmp, timetmp, countmp, valtmp, records);                           
                            bubbleSort_int_up(timetmp, idtmp, countmp, valtmp, records);
                            bubbleSort_double_up(valtmp, idtmp, timetmp, countmp, records);
                        }
                    }
                }
                if(!strcmp(argv[5], "-dsc")){
                    if(reads == 1){
                        if(!strcmp(p1, "id"))
                            bubbleSort_int_down(idtmp, timetmp0, countmp, valtmp0, records);
                        if(!strcmp(p1, "time"))
                            bubbleSort_int_down(timetmp, idtmp0, countmp, valtmp0, records);
                        if(!strcmp(p1, "values"))
                            bubbleSort_double_down(valtmp, idtmp0, timetmp0, countmp, records);
                    }
                    if(reads == 2){
                        if(!strcmp(p1, "id") && !strcmp(p2, "time")){
                            bubbleSort_int_down(timetmp, idtmp, countmp, valtmp0, records);
                            bubbleSort_int_down(idtmp, timetmp, countmp, valtmp0, records);                          
                        }
                        if(!strcmp(p1, "id") && !strcmp(p2, "values")){
                            bubbleSort_double_down(valtmp, idtmp, timetmp0, countmp, records);
                            bubbleSort_int_down(idtmp, timetmp0, countmp, valtmp, records);                          
                        }
                        if(!strcmp(p1, "time") && !strcmp(p2, "id")){
                            bubbleSort_int_down(idtmp, timetmp, countmp, valtmp0, records);
                            bubbleSort_int_down(timetmp, idtmp, countmp, valtmp0, records);                          
                        }
                        if(!strcmp(p1, "time") && !strcmp(p2, "values")){
                            bubbleSort_double_down(valtmp, idtmp0, timetmp, countmp, records);
                            bubbleSort_int_down(timetmp, idtmp0, countmp, valtmp, records);                         
                        }
                        if(!strcmp(p1, "values") && !strcmp(p2, "id")){
                            bubbleSort_int_down(idtmp, timetmp0, countmp, valtmp, records); 
                            bubbleSort_double_down(valtmp, idtmp, timetmp0, countmp, records);                          
                        }
                        if(!strcmp(p1, "values") && !strcmp(p2, "time")){
                            bubbleSort_int_down(timetmp, idtmp0, countmp, valtmp, records); 
                            bubbleSort_double_down(valtmp, idtmp0, timetmp, countmp, records);                       
                        }
                    }
                    if(reads == 3){
                        if(!strcmp(p1, "id") && !strcmp(p2, "time") && !strcmp(p3, "values")){
                            bubbleSort_double_down(valtmp, idtmp, timetmp, countmp, records);
                            bubbleSort_int_down(timetmp, idtmp, countmp, valtmp, records);
                            bubbleSort_int_down(idtmp, timetmp, countmp, valtmp, records);
                        }
                        if(!strcmp(p1, "id") && !strcmp(p2, "values") && !strcmp(p3, "time")){
                            bubbleSort_int_down(timetmp, idtmp, countmp, valtmp, records);
                            bubbleSort_double_down(valtmp, idtmp, timetmp, countmp, records);                           
                            bubbleSort_int_down(idtmp, timetmp, countmp, valtmp, records);
                        }
                        if(!strcmp(p1, "time") && !strcmp(p2, "values") && !strcmp(p3, "id")){
                            bubbleSort_int_down(idtmp, timetmp, countmp, valtmp, records);
                            bubbleSort_double_down(valtmp, idtmp, timetmp, countmp, records);
                            bubbleSort_int_down(timetmp, idtmp, countmp, valtmp, records);
                        }
                        if(!strcmp(p1, "time") && !strcmp(p2, "id") && !strcmp(p3, "values")){
                            bubbleSort_double_down(valtmp, idtmp, timetmp, countmp, records);
                            bubbleSort_int_down(idtmp, timetmp, countmp, valtmp, records);
                            bubbleSort_int_down(timetmp, idtmp, countmp, valtmp, records);
                        }
                        if(!strcmp(p1, "values") && !strcmp(p2, "id") && !strcmp(p3, "time")){
                            bubbleSort_int_down(timetmp, idtmp, countmp, valtmp, records);
                            bubbleSort_int_down(idtmp, timetmp, countmp, valtmp, records);
                            bubbleSort_double_down(valtmp, idtmp, timetmp, countmp, records);                                                   
                        }
                        if(!strcmp(p1, "values") && !strcmp(p2, "time") && !strcmp(p3, "id")){
                            bubbleSort_int_down(idtmp, timetmp, countmp, valtmp, records);                           
                            bubbleSort_int_down(timetmp, idtmp, countmp, valtmp, records);
                            bubbleSort_double_down(valtmp, idtmp, timetmp, countmp, records);
                        }
                    }
                }
                end1 = clock();                
                for(int i = 0; i <= records; i++){
                    if(AV[countmp[i]].id == 0)
                        continue;
                    fprintf(fptrout,"%d,%02d:%02d:%02d %02d:%02d:%02d,%.1lf,%.0lf,%s\n", AV[countmp[i]].id, AV[countmp[i]].year, AV[countmp[i]].month, AV[countmp[i]].day, AV[countmp[i]].hour, AV[countmp[i]].min, AV[countmp[i]].sec, AV[countmp[i]].avr, AV[countmp[i]].aqi, AV[countmp[i]].pol); 
                }
            }
            if(argc == 5){
                start2 = clock(); 
                if(!strcmp(argv[4], "-asc")){
                    selectionSort_double_up(valtmp2, idtmp2, timetmp2, countmp2, records);
                    selectionSort_int_up(timetmp2, idtmp2, countmp2, valtmp2, records);
                    selectionSort_int_up(idtmp2, timetmp2, countmp2, valtmp, records);
                    printf("Default: %s %s %s -s id,time,values %s\n", argv[0], argv[1], argv[2], argv[4]);                     
                }
                if(!strcmp(argv[4], "-dsc")){
                    selectionSort_double_down(valtmp2, idtmp2, timetmp2, countmp2, records);
                    selectionSort_int_down(timetmp2, idtmp2, countmp2, valtmp2, records);
                    selectionSort_int_down(idtmp2, timetmp2, countmp2, valtmp2, records);
                    printf("Default: %s %s %s -s id,time,values %s\n", argv[0], argv[1], argv[2], argv[4]);
                }
                end2 = clock();
            }
            if(argc == 6){
                reads = sscanf(argv[4],"%69[^,],%69[^,],%69[^,]", p1, p2, p3); 
                start2 = clock();          
                if(!strcmp(argv[5], "-asc")){
                    if(reads == 1){
                        if(!strcmp(p1, "id"))
                            selectionSort_int_up(idtmp2, timetmp0, countmp2, valtmp0, records);
                        if(!strcmp(p1, "time"))
                            selectionSort_int_up(timetmp2, idtmp0, countmp2, valtmp0, records);
                        if(!strcmp(p1, "values"))
                            selectionSort_double_up(valtmp2, idtmp0, timetmp0, countmp2, records);
                    }
                    if(reads == 2){
                        if(!strcmp(p1, "id") && !strcmp(p2, "time")){
                            selectionSort_int_up(timetmp2, idtmp2, countmp2, valtmp0, records);
                            selectionSort_int_up(idtmp2, timetmp2, countmp2, valtmp0, records);                          
                        }
                        if(!strcmp(p1, "id") && !strcmp(p2, "values")){
                            selectionSort_double_up(valtmp2, idtmp2, timetmp0, countmp2, records);
                            selectionSort_int_up(idtmp2, timetmp0, countmp2, valtmp2, records);                          
                        }
                        if(!strcmp(p1, "time") && !strcmp(p2, "id")){
                            selectionSort_int_up(idtmp2, timetmp2, countmp2, valtmp0, records);
                            selectionSort_int_up(timetmp2, idtmp2, countmp2, valtmp0, records);                          
                        }
                        if(!strcmp(p1, "time") && !strcmp(p2, "values")){
                            selectionSort_double_up(valtmp2, idtmp0, timetmp2, countmp2, records);
                            selectionSort_int_up(timetmp2, idtmp0, countmp2, valtmp2, records);                         
                        }
                        if(!strcmp(p1, "values") && !strcmp(p2, "id")){
                            selectionSort_int_up(idtmp2, timetmp0, countmp2, valtmp2, records); 
                            selectionSort_double_up(valtmp2, idtmp2, timetmp0, countmp2, records);                          
                        }
                        if(!strcmp(p1, "values") && !strcmp(p2, "time")){
                            selectionSort_int_up(timetmp2, idtmp0, countmp2, valtmp2, records); 
                            selectionSort_double_up(valtmp2, idtmp0, timetmp2, countmp2, records);                       
                        }
                    }
                    if(reads == 3){
                        if(!strcmp(p1, "id") && !strcmp(p2, "time") && !strcmp(p3, "values")){
                            selectionSort_double_up(valtmp2, idtmp2, timetmp2, countmp2, records);
                            selectionSort_int_up(timetmp2, idtmp2, countmp2, valtmp2, records);
                            selectionSort_int_up(idtmp2, timetmp2, countmp2, valtmp2, records);
                        }
                        if(!strcmp(p1, "id") && !strcmp(p2, "values") && !strcmp(p3, "time")){
                            selectionSort_int_up(timetmp2, idtmp2, countmp2, valtmp2, records);
                            selectionSort_double_up(valtmp2, idtmp2, timetmp2, countmp2, records);                           
                            selectionSort_int_up(idtmp2, timetmp2, countmp2, valtmp2, records);
                        }
                        if(!strcmp(p1, "time") && !strcmp(p2, "values") && !strcmp(p3, "id")){
                            selectionSort_int_up(idtmp2, timetmp2, countmp2, valtmp2, records);
                            selectionSort_double_up(valtmp2, idtmp2, timetmp2, countmp2, records);
                            selectionSort_int_up(timetmp2, idtmp2, countmp2, valtmp2, records);
                        }
                        if(!strcmp(p1, "time") && !strcmp(p2, "id") && !strcmp(p3, "values")){
                            selectionSort_double_up(valtmp2, idtmp2, timetmp2, countmp2, records);
                            selectionSort_int_up(idtmp2, timetmp2, countmp2, valtmp2, records);
                            selectionSort_int_up(timetmp2, idtmp2, countmp2, valtmp2, records);
                        }
                        if(!strcmp(p1, "values") && !strcmp(p2, "id") && !strcmp(p3, "time")){
                            selectionSort_int_up(timetmp2, idtmp2, countmp2, valtmp2, records);
                            selectionSort_int_up(idtmp2, timetmp2, countmp2, valtmp2, records);
                            selectionSort_double_up(valtmp2, idtmp2, timetmp2, countmp2, records);                                                   
                        }
                        if(!strcmp(p1, "values") && !strcmp(p2, "time") && !strcmp(p3, "id")){
                            selectionSort_int_up(idtmp2, timetmp2, countmp2, valtmp2, records);                           
                            selectionSort_int_up(timetmp2, idtmp2, countmp2, valtmp2, records);
                            selectionSort_double_up(valtmp2, idtmp2, timetmp2, countmp2, records);
                        }
                    }
                }
                if(!strcmp(argv[5], "-dsc")){
                    if(reads == 1){
                        if(!strcmp(p1, "id"))
                            selectionSort_int_down(idtmp2, timetmp0, countmp2, valtmp0, records);
                        if(!strcmp(p1, "time"))
                            selectionSort_int_down(timetmp2, idtmp0, countmp2, valtmp0, records);
                        if(!strcmp(p1, "values"))
                            selectionSort_double_down(valtmp2, idtmp0, timetmp0, countmp2, records);
                    }
                    if(reads == 2){
                        if(!strcmp(p1, "id") && !strcmp(p2, "time")){
                            selectionSort_int_down(timetmp2, idtmp2, countmp2, valtmp0, records);
                            selectionSort_int_down(idtmp2, timetmp2, countmp2, valtmp0, records);                          
                        }
                        if(!strcmp(p1, "id") && !strcmp(p2, "values")){
                            selectionSort_double_down(valtmp2, idtmp2, timetmp0, countmp2, records);
                            selectionSort_int_down(idtmp2, timetmp0, countmp2, valtmp2, records);                          
                        }
                        if(!strcmp(p1, "time") && !strcmp(p2, "id")){
                            selectionSort_int_down(idtmp2, timetmp2, countmp2, valtmp0, records);
                            selectionSort_int_down(timetmp2, idtmp2, countmp2, valtmp0, records);                          
                        }
                        if(!strcmp(p1, "time") && !strcmp(p2, "values")){
                            selectionSort_double_down(valtmp2, idtmp0, timetmp2, countmp2, records);
                            selectionSort_int_down(timetmp2, idtmp0, countmp2, valtmp2, records);                         
                        }
                        if(!strcmp(p1, "values") && !strcmp(p2, "id")){
                            selectionSort_int_down(idtmp2, timetmp0, countmp2, valtmp2, records); 
                            selectionSort_double_down(valtmp2, idtmp2, timetmp0, countmp2, records);                          
                        }
                        if(!strcmp(p1, "values") && !strcmp(p2, "time")){
                            selectionSort_int_down(timetmp2, idtmp0, countmp2, valtmp2, records); 
                            selectionSort_double_down(valtmp2, idtmp0, timetmp2, countmp2, records);                       
                        }
                    }
                    if(reads == 3){
                        if(!strcmp(p1, "id") && !strcmp(p2, "time") && !strcmp(p3, "values")){
                            selectionSort_double_down(valtmp2, idtmp2, timetmp2, countmp2, records);
                            selectionSort_int_down(timetmp2, idtmp2, countmp2, valtmp2, records);
                            selectionSort_int_down(idtmp2, timetmp2, countmp2, valtmp2, records);
                        }
                        if(!strcmp(p1, "id") && !strcmp(p2, "values") && !strcmp(p3, "time")){
                            selectionSort_int_down(timetmp2, idtmp2, countmp2, valtmp2, records);
                            selectionSort_double_down(valtmp2, idtmp2, timetmp2, countmp2, records);                           
                            selectionSort_int_down(idtmp2, timetmp2, countmp2, valtmp2, records);
                        }
                        if(!strcmp(p1, "time") && !strcmp(p2, "values") && !strcmp(p3, "id")){
                            selectionSort_int_down(idtmp2, timetmp2, countmp2, valtmp2, records);
                            selectionSort_double_down(valtmp2, idtmp2, timetmp2, countmp2, records);
                            selectionSort_int_down(timetmp2, idtmp2, countmp2, valtmp2, records);
                        }
                        if(!strcmp(p1, "time") && !strcmp(p2, "id") && !strcmp(p3, "values")){
                            selectionSort_double_down(valtmp2, idtmp2, timetmp2, countmp2, records);
                            selectionSort_int_down(idtmp2, timetmp2, countmp2, valtmp2, records);
                            selectionSort_int_down(timetmp2, idtmp2, countmp2, valtmp2, records);
                        }
                        if(!strcmp(p1, "values") && !strcmp(p2, "id") && !strcmp(p3, "time")){
                            selectionSort_int_down(timetmp2, idtmp2, countmp2, valtmp2, records);
                            selectionSort_int_down(idtmp2, timetmp2, countmp2, valtmp2, records);
                            selectionSort_double_down(valtmp2, idtmp2, timetmp2, countmp2, records);                                                   
                        }
                        if(!strcmp(p1, "values") && !strcmp(p2, "time") && !strcmp(p3, "id")){
                            selectionSort_int_down(idtmp2, timetmp2, countmp2, valtmp2, records);                           
                            selectionSort_int_down(timetmp2, idtmp2, countmp2, valtmp2, records);
                            selectionSort_double_down(valtmp2, idtmp2, timetmp2, countmp2, records);
                        }
                    }
                }
                end2 = clock();                
            }
            fclose(fptrout);
            fclose(fptrerror);
            fprintf(fptrrun, "Total number of rows: %d\n", rerecords);
            fprintf(fptrrun, "Succesfully converted rows %d\n", rerecords - errorcount);
            fprintf(fptrrun, "Error rows %d\n", errorcount);
            if(argc > 3){
                fprintf(fptrrun, "Sorting algorithm bubble [ms]: %lf\n", (double)(end1 - start1) * 1000 / CLOCKS_PER_SEC);
                fprintf(fptrrun, "Sorting algorithm selection [ms]: %lf\n", (double)(end2 - start2) * 1000 / CLOCKS_PER_SEC);
            }
            fclose(fptrrun);
        }
        else{
            fprintf(fptrerror, "Error 03: invalid fileformat");
            exit(1);
        }
    }   
}
int main(int argc, char *argv[])
{    
    convert(argc, argv);
    return(0);
}
