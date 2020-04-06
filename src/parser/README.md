## SPICE parser

This directory consists of SPICE parser based on C libraries - Flex/Bison. Any changes made in this directory needs to be rebuilt to update the static library used in LGE.

### File Information

Please find description of all files, based on which changes can be done.

| File                | Description                                                                                                                        |
|---------------------|------------------------------------------------------------------------------------------------------------------------------------|
| BookshelfDriver.h   | List the user call back functions                                                                                                  |
| BookshelfDatabase.h | List of virtual instantiations of user call back functions                                                                         |
| BookshelfScanner.ll | Based on Flex library and consists of keyword/data to be captured.                                                                 |
| BookshelfDriver.cc  | Map user call back functions to corresponding db functions (Mapping to C++)                                                        |
| BookshelfParser.yy  | Based on Bison library and consists of Grammar rules (or) Format of lines, in the file to be parsed as well as call back functions |

### To build 

To build the static library,

```
make clean
make
```

