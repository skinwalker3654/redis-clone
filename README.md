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

## Supported Commands

### String Commands

```
set key value
```

Creates or updates a key with a value.

Example:

```
> set name john
OK
```

---

```
get key
```

Returns the value stored at a key.

Example:

```
> get name
"john"
```

---

```
del key
```

Deletes a key from the database.

Example:

```
> del name
(integer): 1
```

---

```
exists key
```

Checks if a key exists.

Example:

```
> exists name
(integer): 1
```

---

### List Commands

```
rpush key value [value...]
```

Creates a list or appends a value to an existing list.

Example:

```
> rpush users john alex
(integer): 2
```

---

```
lrange key start end
```

Returns values inside a list range.

`-1` can be used to get the entire list.

Example:

```
> lrange users 0 -1
1) "john"
2) "alex"
```

---

```
rpop key
```

Removes and returns the last value from a list.

Example:

```
> rpop users
"alex"
```

---

```
lset key index value
```

Changes a value inside a list.

Example:

```
> lset users 0 mike
(integer): 1
```

---

```
lindex key index
```

Returns a value at a specific list index.

Example:

```
> lindex users 0
"mike"
```

---

### Expiration Commands

```
expire key seconds
```

Sets an expiration timer for a key.

Example:

```
> expire session 60
(integer): 1
```

The key will be deleted after 60 seconds.

---

```
ttl key
```

Shows the remaining lifetime of a key.

Example:

```
> ttl session
(integer): 45
```

---

### Database Commands

```
flushall
```

Deletes all keys from the database.

Example:

```
> flushall
OK
```

---

### Utility Commands

```
clear
```

Clears the terminal screen.

---

```
help
```

Displays available commands.

---

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
└── README.md
```

## Build

```bash
make
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
# redis-clone
