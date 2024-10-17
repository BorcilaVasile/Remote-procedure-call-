#!/bin/bash
g++ -g /home/vasile/Desktop/Remote-procedure-call-/Server/main_server.cpp \
    /home/vasile/Desktop/Remote-procedure-call-/Shared/*.cpp \
    -I /home/vasile/Desktop/Remote-procedure-call-/Shared \
    -o /home/vasile/Desktop/Remote-procedure-call-/Server/main_server
