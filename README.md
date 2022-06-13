# Time-Log-Parsing
Program to parse through a Kronos Time Clock log file finding system startups and verifying if the booting process completed using regular expressions. Created regexes for the startup message/timestamp, completion message/timestamp, and getting the date and time from each of those timestamps. If a boot was completed, the elapsed time would be displayed. All of the device's time log boot info is stored and printed on the report file. 

Command:
```
make
./ps7 device5_intouch.log
```
