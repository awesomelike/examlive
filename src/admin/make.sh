#!/bin/bash
gcc -o admin $(mysql_config --cflags) admin.c $(mysql_config --libs) -Wall `pkg-config --cflags --libs gtk+-3.0` -export-dynamic -w -lpthread
