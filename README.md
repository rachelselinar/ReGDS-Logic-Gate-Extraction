# ReGDS: Logic Gate Extraction routine

The Logic Gate Extraction (LGE) is a custom C++ routine, part of the layout reverse engineering framework *ReGDS*, for digital circuits, that converts layout netlist (SPICE) to HDL netlist (Verilog) in the presence of the logic gate definitions from the standard cell library (SPICE).
The digital connectivity index (DCI) coding scheme is employed to represent connectivity between transistors and [subgraph isomorphism algorithm](https://ieeexplore.ieee.org/abstract/document/1323804) is employed to identify logic gates in the layout netlist (sea of transistors).
For more details, please refer to the ['paper'](#citation).

The SPICE netlist is parsed and the connectivity information is stored as a hypergraph.
Tie off in the layout netlist is handled by renaming to avoid mismatch during graph-based pattern matching to identify logic gates.
LGE has two modes of operation:
* Library Build:
    - SPICE netlist with logic gate definitions from the technology library is read and library digital connectivity graphs (DCGs) are constructed.
* Logic Gate Identification:
    - The layout SPICE netlist is read and logic gates are identified based on the existing library DCGs.
As LGE reads in only one SPICE netlist at a time, a [rebuild](#rebuild) is required after *Library Build* mode to incorporate the constructed library DCGs in *Logic Gate Identification*.

## Acknowlegement

Thanks to *Wuxi Li* and *Keren Zhu* from [UTDA](https://www.cerc.utexas.edu/utda), for all the help in the development of this routine!

## Developers

- Rachel Selina Rajarathnam, [UTDA](https://www.cerc.utexas.edu/utda), ECE Department, The University of Texas at Austin

## External Dependencies

The external dependencies are listed below:

* [CMake](https://cmake.org) version 2.8.11 or later is required. 
* [cmdline](https://github.com/tanakh/cmdline) is a Command line parser for C++.
* [Flex](http://flex.sourceforge.net) lexical analyzer is employed in the SPICE parser.
* [Bison](https://www.gnu.org/software/bison) parser generator is employed in the SPICE parser.
* [Boost](https://www.boost.org) library (version 1.61.0) is employed for graph based operations.
* [gTest](https://github.com/google/googletest) is the Google C++ testing framework.

## Cloning the repository

To clone the repository, 

```
git clone https://github.com/rachelselinar/ReGDS-Logic-Gate-Extraction.git
```

## Build Instructions

### Parser build

The SPICE netlist parser is found within *\<root\>/src/parser* directory and needs to built to generate static library **libbookshelfparser.a**.
Please refer to parser [README](./src/parser/README.md) for information on files to make any changes. It is case-sensitive.

```
cd src/parser
make clean
make
```

Repeat this step when changes are made to the parser. By default the parser supports the format in [example](./examples/lib.sp).

### LGE build

To build LGE, run the following commands at the *\<root\>* directory. 
```
mkdir bin build
cd build
cmake ..
make 
```
GCC 4.9 or later is preferred. 
Export CC and CXX environment variables for custom gcc and g++ path, respectively. 

To clean, run make clean in the cpp directory. 
```
make clean
```

### Rebuild

A rebuild is required after running LGE in *Library Build* mode and before *Logic Gate Identification*.
At the *\<root\>/build* directory, do
```
make clean
make
```

## Running LGE

After successful build, LGE can be run at *\<root\>/bin* directory.

```
usage: ./LGE --sp=string --lib=bool [options] ... 
options:
  -i, --sp      Provide SPICE netlis for Library or Layout netlist as per mode of operation (string)
  -l, --lib     Specify mode of operation (True = Library Build; False = Logic Gate Extraction) (bool)
      --dot     Write out DOT file for graphical visualization (optional, default=false) (bool [=0])
      --sOut    Write out SPICE file (optional, default=false) (bool [=0])
      --log     log file (string [=])
  -h, --help    Lists available options
```

### Library Build mode

To construct library DCGs, run LGE in library mode with *lib* flag set to **1**, at *\<root\>/bin* directory. For example:

```
./LGE --lib=1 --sp=../examples/lib.sp
```

After construction of library DCGs, do a [rebuild](#rebuild) before proceeding to *Logic Gate Identification* mode.

### Logic Gate Identification mode

At *\<root\>/bin* directory, run LGE with *lib* flat set to **0**. For example:

```
./LGE --lib=0 --sp=../examples/c17_flat.sp
```

c17 is a combinational circuit that is part of the [ISCAS'85 benchmarks](http://www.pld.ttu.ee/~maksim/benchmarks/iscas85/).
LGE can be used on flat as well as hierarchical spice netlists. The large runtime of the graph isomorphism algorithm restricts the usage to reasonably sized flattened spice netlists. For very large designs, please use hierarchical input spice netlist.

### Running LGE with different library netlist

To run LGE with different library netlist, clear existing library DCGs contents at *\<root\>* directory,

```
./clearLibrary.sh
```

Once existing contents are cleared, a [rebuild](#rebuild) is required before running LGE in [Library Build mode](#library-build-mode).

## Bug Report

To report a bug, please file an [issue](https://github.com/rachelselinar/ReGDS-Logic-Gate-Extraction/issues).

## Citation

If you use LGE routine in your work, please [cite](https://ieeexplore.ieee.org/document/9300272): 

```
R. S. Rajarathnam, Y. Lin, Y. Jin and D. Z. Pan, "ReGDS: A Reverse Engineering Framework from GDSII to Gate-level Netlist," IEEE International Symposium on Hardware Oriented Security and Trust (HOST), CA, Dec 6-9, 2020.
```

Bibtex:
```
@inproceedings{Rajarathnam2020ReGDS,
  title={ReGDS: A Reverse Engineering Framework from GDSII to Gate-level Netlist},
  author={Rajarathnam, Rachel Selina and Lin, Yibo and Jin, Yier and Pan, David Z.},
  booktitle={IEEE International Symposium on Hardware Oriented Security and Trust (HOST)},
  year={2020}
}
```

## Copyright

This software is released under *GNU General Public License*. Please refer to ['LICENSE'](#license) for details.

- [CMake](https://cmake.org) is released under *BSD 3-clause* license.
- [cmdline](https://github.com/tanakh/cmdline) is released under *BSD 3-clause* license.
- [Flex](http://flex.sourceforge.net) is released as a *Free and Open-Source Software*.
- [Bison](https://www.gnu.org/software/bison) is released under *GNU General Public License*.
- [Boost](https://www.boost.org) is released under *Boost Software License*.
- [gTest](https://github.com/google/googletest) is released under *BSD 3-clause* license.

## LICENSE

GNU General Public License v3.0 or later.

Refer to [LICENSE](./LICENSE) for complete information.

