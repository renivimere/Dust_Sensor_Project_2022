# Dust_Sensor_Project_2022
EE3491 HUST  
Programming Techniques  
Le Minh Quan

## Overview
The program uses the C language with appropriate functions and data structures to simulate PM2.5 dust sensor data, measuring the concentration of dust particles smaller than 2.5 microns in the air.

- **Measurement range:** 0 ÷ 600 𝜇g/m³
- **Resolution:** 0.1 𝜇g/m³

## Features

### [Task 1: Data Generation Program](/task_1)

A program capable of outputting a dataset including the sensor ID, simulated measurement timestamp, and simulated sensor values. The simulation start time is the current time obtained from the system clock (the computer's current time) minus the simulation duration.

#### Command-line Usage

The user needs to run the program on the command-line with the following format:

```console
dust_sim –n [num_sensors] –st [sampling] –si [interval]
```



Where:
- **`dust_sim`** is the name of the compiled program file.
- **`-n [num_sensors]`** is a pair of input parameters to specify the number of sensors. `[num_sensors]` should be replaced by a specific positive integer. **The program will not run if only one of these two parameters is provided or if the specified number of sensors is invalid. If both parameters are missing, the program will default to 1 sensor and display a message on the screen.**
- **`-st [sampling]`** is a pair of parameters to specify the sampling time with `[sampling]` replaced by a positive integer in seconds. The minimum allowed sampling time is 1 second. **The program will not run and will display an error message if only one of these two parameters is provided. If both parameters are missing, the program will default to a sampling frequency of 30 seconds.**
- **`-si [interval]`** is a pair of parameters to specify the measurement interval with `[interval]` replaced by a positive integer in hours. The minimum simulation duration is 1 hour. **The program will not run and will display an error message if only one of these two parameters is provided. If both parameters are missing, the program will default to a measurement interval of 24 hours.**
- **If the user provides incorrect syntax, invalid parameters, or errors that do not fall into the aforementioned default cases, the program will not run and will display an error message. The error code will be logged into the file `task1.log` as `Error 01: invalid command & argument`.**

Example usage:

```console
dust_sim -n 5 -st 10 -si 2
```

This command will run the program with 5 sensors, a sampling time of 10 seconds, and a measurement interval of 2 hours.

#### Data Generation

If the user provides a valid command, the program will execute and generate a dataset consisting of the sensor ID, simulated measurement timestamp, and simulated sensor values. The simulation start time is the current system time minus the specified simulation duration.

- Sensor IDs range from 1 to `num_sensors`, where `num_sensors` is the number of sensors provided by the user in the command-line.
- The simulation timestamp follows the format `YYYY:MM:DD hh:mm:ss`.
- Simulated measurement values are randomly generated floating-point numbers with one decimal place of precision.
- The simulated data will be saved to a file named `dust_sensor.csv`. If the file already exists, it will overwrite the existing file. The file follows the CSV format, where fields are separated by commas.
- **If the program encounters an error while attempting to overwrite an existing file named `dust_sensor.csv`, it will log the error into `task1.log` as `Error 03: denied access dust_sensor.csv`.**

### [Task 2: Data Processing Program](/task_2)

This program is designed to process data obtained from dust sensors. It performs various operations on the input data to filter outliers, calculate average dust concentrations, convert them to Air Quality Index (AQI), and provide pollution level warnings.

#### Command-line Usage

The user needs to run the program on the command-line with the following format:

```console
dust_process [data_filename.csv]
```

Where:
- **`dust_process`** represents the compiled program filename.
- **`[data_filename.csv]`** denotes the CSV file containing dust sensor data, structured similarly to the `dust_sensor.csv` file from Task 1.
- **If the user fails to provide a filename and only enters `C:\\dust_process`, the program will default to using the filename `dust_sensor.csv`, which was generated by the Task 1 program.**
- **In case of invalid command syntax, excluding the default case, the program won't execute and will log an error into `task2.log` with error code `Error 00: invalid command`.**
- **If the input file `data_filename.csv` does not exist or cannot be accessed, the program will log an error into `task2.log` with error code `Error 01: file not found or cannot be accessed`.**

- **If the input file `data_filename.csv` does not have the extension CSV or does not have the same format as the `dust_sensor.csv` file in Task 1 as specified, the program will log an error into `task2.log` with error code `Error 02: invalid csv file`.**

#### Data Processing

Upon receiving a valid command and a valid input file, the program will proceed with data processing. Here's how the processing will be done:

**1. Data Validation and Error Handling:**
   - The program will read each line of the input CSV file.
   - If there are any data errors (such as empty fields or invalid data), the program will terminate and log errors into `task2.log` with appropriate error codes:
     - `Error 02: invalid csv file`
     - `Error 03: data is missing at line X` (X represents the line number with the error)

**2. Capacity Limitation:**
   - The program can handle up to 10000 data points, corresponding to 10000 lines in the input file.

**3. Outlier Removal:**
   - Values outside the range from 5 to 550.5 𝜇𝑔/𝑚³ will be filtered out.
   - Filtered values will be saved into a file named `dust_outliers.csv`.
   - **If `dust_outliers.csv` exists but doesn't allow overwriting, an error will be logged into `task2.log` as "Error 04: denied access dust_outliers.csv".**

**4. Data Processing:**
   - Valid measurements will be utilized to calculate the average dust concentration per hour for each sensor.
   - The concentrations will be converted to Air Quality Index (AQI) values and pollution level warnings will be provided based on a predefined table.
   - Processed data will be saved to the file `dust_aqi.csv`.
   - **If `dust_aqi.csv` already exists but doesn't allow overwriting, an error will be logged into `task2.log` as `Error 05: denied access dust_aqi.csv`.**

**5. Data Aggregation and Statistics:**
   - Based on the average values, the program will aggregate and summarize the total number of hours at each pollution level detected by the sensor nodes.
   - The results will be saved to a file named `dust_statistics.csv`.
   - **If `dust_statistics.csv` already exists but doesn't allow overwriting, an error will be logged into `task2.log` as `Error 06: denied access dust_statistics.csv`.**

**6. Summary Calculation:**
   - The program will determine the maximum (max), minimum (min), and average dust concentration values measured at each sensor node throughout the simulation period.
   - The results will be saved to a file named `dust_summary.csv`.
   - The corresponding timestamps for the maximum and minimum values in the file will indicate the earliest time these values appeared in the input file.
   - The timestamp corresponding to the mean value represents the simulated duration measured in hours.
   - **If `dust_summary.csv` already exists but doesn't allow overwriting, an error will be logged into `task2.log` as `Error 07: denied access dust_summary.csv`.**
  
### [Task 3: Data Conversion Program](/task_3)

This program involves converting the processed CSV data into a specific hexadecimal format. The program reads a CSV file (generated in Task 2), converts each data point into a structured packet of bytes, and writes these packets to a DAT file. This conversion ensures the data can be utilized in systems requiring hexadecimal data formats.

#### Command-line Usage

Users are required to execute the program from the command line using the following format:

```console
dust_convert [data_filename.csv] [hex_filename.dat]
```

Where:
- **`dust_convert`** is the compiled program filename.
- **`[data_filename.csv]`** is the input file in CSV format, following the structure of the `dust_aqi.csv` file generated in Task 2.
- **`[hex_filename.dat]`** is the output file in text format with the extension `.dat`.
- **If the user enters an incorrect command syntax, the program will not execute and will log an error into `task3.log` with error code `Error 00: invalid command`.**
- **If the input file `data_filename.csv` does not exist or exists but cannot be accessed, the program will log an error into `task3.log` with error code `Error 01: file not found or cannot be accessed`.**
- **If the input file `data_filename.csv` does not have the extension CSV or does not have the same format as the `dust_aqi.csv` file generated in Task 2, the program will log an error into `task3.log` with error code `Error 02: invalid csv file`.**
- **If the output file `hex_filename.dat` already exists and does not allow overwriting, the program will log an error into `task3.log` as `Error 04: cannot override hex_filename.dat`.**

#### Data Conversion

- If the user provides a valid command and a valid input file, the program will execute and read each line of the input CSV file. 

- **If there are any data errors (such as empty fields, invalid data, or format discrepancies compared to the `dust_aqi.csv` file in Task 2), the program will terminate and log errors into `task3.log` with the error code `Error 02: invalid csv file` and `Error 03: data is missing at line X`, where X represents the line number in the input file (with the header line considered as line 0, and subsequent lines incremented by one)**. 

- If no data errors occur, the program will convert the data into packet form, with each packet represented as a byte string, and write each packet to a separate line in the output DAT file.

Each byte string consists of 15 bytes, with each byte separated by a space. The bytes are represented in hexadecimal format as follows:
- **Start byte:** Always `0x00`.
- **Packet length:** Represents the length of the packet, including the start byte and stop byte, with a value of `0x0F`.
- **Sensor ID:** An identifier greater than 0.
- **Time:**  4 bytes representing the measurement timestamp in Unix time format.
- **PM2.5 concentration:** A 4-byte floating-point value according to IEEE754 standard.
- **AQI:** A 2-byte integer representing the Air Quality Index.
- **Checksum:** A byte for verifying the accuracy of the packet data, calculated as the 2's complement of the least significant byte of the sum of packet length, sensor ID, time, PM2.5 concentration, and AQI.
- **Stop byte:** Always `0xFF`.

### [Task 4: Automatic Data Conversion and Sorting Program](/task_4)

This program involves a versatile program capable of recognizing the format of input files and performing the corresponding data conversion. The program can handle both CSV and DAT files, converting them to the appropriate format based on the input file extension.

Additionally, the program includes a sorting feature for data contained in CSV files.

**1. From Hex to Text Conversion (without sorting)**

#### Command-line Usage

The user needs to run the program on the command-line with the following format:

```console
dust_convert [hex_filename.dat] [data_filename.csv]
```
Where:
- **`dust_convert`** is the compiled program file name.
- **`[data_filename.csv]`** is the output CSV file that will contain the sensor data stored in plain text format.
- **`[hex_filename.dat]`** is the input DAT file containing sensor data stored in byte string packets.
- **If the user enters an incorrect syntax, provides insufficient parameters, or uses an invalid command, the program will report an error and log the error code into the "**dust_convert_error.log**" file as `Error 01: invalid command`.**
- **If the input or output files do not exist, or if the file exists but is not accessible, the program will report an error and log the error code into the `dust_convert_error.log` file as `Error 02: denied access FILENAME`, where FILENAME is replaced with the inaccessible file name.**
- **If the input or output files do not adhere to the specified format, meaning they do not have the required extensions (.csv or .dat), the program will not execute and will report an error into the `dust_convert_error.log` file as `Error 03: invalid file format`.**

#### Data Conversion

- If the user provides a valid command and the input and output files are valid, the program will run and read each line of the input file. 

- **If there are errors in the data packet within the input DAT file (such as missing fields, invalid fields, incorrect Start byte, Stop byte, Packet length, future timestamps compared to the program execution time, incorrect checksum, inconsistent dust concentration values, and AQI index), the program will skip that line and continue execution, logging the error into the `dust_convert_error.log` file with error code `Error 06: invalid data packet at line X`, where X is the line number in the input file (starting from 0).**

- If no data errors are detected in any line, the program will proceed to convert the data into plain text format and save it into the output CSV file.
- **In case duplicate data is found in two lines, the program will only output one line and log the error into the `dust_convert_error.log` file with error code `Error 05: duplicated data at lines X1, X2`, where X1 and X2 are the line numbers in the input file.**

After conversion, the program will also save the runtime parameters into the `dust_convert_run.log` file. The information to be saved includes:
- **Total number of rows:** the total number of rows in the input file, excluding empty rows.
- **Successfully converted rows:** the number of rows containing valid data that were successfully converted (including duplicate rows).
- **Error rows:** the number of rows containing invalid data (excluding duplicate rows).

**2. From Hex to Text Conversion (with sorting)**

#### Command-line Usage

The user needs to run the program on the command-line with the following format:

```console
dust_convert [hex_filename.dat] [data_filename.csv] -s [params] [-asc/-dsc]
```

Where:
- **`-s`** is the parameter to specify the sorting requirement.
- **`[params]`** is a list of parameters used for sorting. The order of parameters from left to right represents the priority order for sorting. There are 3 parameters that can be used: `id`, `time`, and `values`. If multiple parameters are used, they are separated by commas. If a parameter is not mentioned, it will maintain the original order as read from the input file. If [params] is not provided, the default sorting order will be `id`, `time`, and `values`.
- **`[-asc/-dsc]`** is a parameter with a value of -asc or -dsc to determine whether to sort in ascending or descending order, respectively.

After processing, the program will also save the runtime parameters into the `dust_convert_run.log` file. The information to be saved includes:
- **Total number of rows:** the total number of rows in the input file, excluding empty rows.
- **Successfully converted rows:** the number of rows containing valid data that were successfully converted (including duplicate rows).
- **Error rows:** the number of rows containing invalid data (excluding duplicate rows).
- **Sorting algorithm bubble [ms]:** the time taken to perform the Bubble Sort Algorithm, measured in milliseconds.
- **Sorting algorithm selection [ms]:** the time taken to perform the Selection Sort Algorithm, measured in milliseconds.

