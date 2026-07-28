# Redis Clone in C

A lightweight Redis-inspired key-value database written in C.

This project implements a simple command-line database engine with support for strings, lists, key expiration, and basic Redis-like commands.

The goal of this project is to practice:

* Data structures in C
* Memory management
* Hash maps
* Dynamic arrays
* Command parsing
* Modular project architecture
* Working with pointers and heap allocation

## Features

* Key-value storage
* String values
* List values
* Key expiration system
* Interactive CLI
* Command dispatcher system
* Dynamic memory management

## Project Structure

Example layout:

```
redis/
│
├── source/
│   ├── main.c
│   ├── commands.c
│   ├── operations.c
│   ├── hashmap.c
│   └── parser.c
│
├── header/
│   ├── commands.h
│   ├── hashmap.h
│   ├── operations.h
│   └── parser.h
│
├── PUT_NAME_HERE
└── README.md
```

## Build

```bash
make
# then open the file called PUT_NAME_HERE and put the database name inside it before you run the program
```

--- 
## Run:

```bash
make run
```

--- 

## Remove executables
```bash
make clean
```
---

## Screenshot
![alt text](https://github.com/skinwalker3654/redis-clone/blob/main/.github/screenshot.png?raw=true)
