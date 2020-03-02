#!/bin/bash
rm -rf src/library/*
touch src/library/lib_findGate.cpp
touch src/library/lib_gates.cpp
touch src/library/lib_gates.h
touch src/library/lib_gateType.cpp
touch src/library/lib_path.cpp
echo "Cleared library logic gate content; Rebuild and run to update library"
