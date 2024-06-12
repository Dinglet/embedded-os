# Embedded Operating Systems

This repository collects most of the code that I developed during the 2024 Spring Semester Embedded Operating Systems[^syllabus], instructed by Prof. HUANG, YU-LUN. It includes lab and homework assignments. Lab assignments correspond to a single course topic and are given after each practical session. Homework assignments encompass multiple course topics and are given once the relevant topics have been covered.

[^syllabus]: Course syllabus: <https://timetable.nycu.edu.tw/?r=main/crsoutline&Acy=112&Sem=2&CrsNo=535314&lang=en-us>

Lab and homework assignments in this repository are:

- Lab 3-1: LED Banner
- Lab 4: LED Banner Using a Sixteen-Segment Display
- Lab 5: Steam Locomotives
- Lab 6: Web ATM
- Lab 7: Number Guessing Game
- Homework 1: Delivery Platform (Standalone Version)
- Homework 2: Delivery Platform (Online Version)
- Homework 3: Delivery Platform (Multi-User Online Version)

## Lab 3: Driver (Part I) - Using GPIO in Raspberry Pi

Deadline: 2024-04-04

In this lab 3-1, we follow "Device Driver 35 - GPIO Driver Basic"[^1] to be familiar with using GPIO and its driver. The original sources of the device driver and makefile are from the following subdirectory of their GitHub repository: https://github.com/Embetronicx/Tutorials/tree/master/Linux/Device_Driver/GPIO-in-Linux-Device-Driver.

[^1]: https://embetronicx.com/tutorials/linux/device-drivers/gpio-driver-basic-using-raspberry-pi/

## Lab4: Banner Using Sixteen-Segment Display

Deadline: 2024-04-11

## Homework 1: Delivery Platform

Deadline: 2024-04-21

## Lab 5: Steam Locomotive

Deadline: 2024-04-25

## Homework 2: Delivery Platform (Online Version)

Deadline: 2024-05-23

In Homework 2, we make Homework 1 an online version. The hw2 program will start running as a server program waiting for connection of a single client.

## Lab 6: Inter-Process Communication

## License

### Modified MIT License

Copyright (c) 2024 HUANG, SHIEN-TING

This license applies to the entire repository except for subdirectories that have their own license file or files that have their own license notice. In such cases, the license file in the subdirectory or the license notice in the file takes precedence.

The subdirectories or files excluded from the original MIT license are:

- [lab3-1/lab3-1_driver.c](./lab3-1/lab3-1_driver.c)
- [lab4/demo.sh](./lab4/demo.sh)
- [lab4/reader.c](./lab4/reader.c)
- [lab4/seg.py](./lab4/seg.py)
- [lab5/demo.sh](./lab5/demo.sh)
- [lab6/examples/](./lab6/examples/)
- [lab7/examples/](./lab7/examples/)
- [lab7/demo.sh](./lab7/demo.sh)

The original MIT License text follows:

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
