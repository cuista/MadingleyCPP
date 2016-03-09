#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/Input/DataGrid.o \
	${OBJECTDIR}/src/Input/FileReader.o \
	${OBJECTDIR}/src/Input/NcGridCell.o \
	${OBJECTDIR}/src/Model/Cohort.o \
	${OBJECTDIR}/src/Model/Environment.o \
	${OBJECTDIR}/src/Model/Stock.o \
	${OBJECTDIR}/src/Output/Logger.o \
	${OBJECTDIR}/src/Program.o \
	${OBJECTDIR}/src/Tools/Convertor.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-g -std=c++11
CXXFLAGS=-g -std=c++11

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=/home/philju/Libraries/netcdfcxx-4.2.1/lib/libnetcdf_c++4.a /home/philju/Libraries/netcdfc-4.3.3.1/lib/libnetcdf.a /home/philju/Libraries/hdf5-1.8.15/lib/libhdf5_hl.a /home/philju/Libraries/hdf5-1.8.15/lib/libhdf5.a /home/philju/Libraries/zlib-1.2.8/lib/libz.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/madingleyc__

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/madingleyc__: /home/philju/Libraries/netcdfcxx-4.2.1/lib/libnetcdf_c++4.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/madingleyc__: /home/philju/Libraries/netcdfc-4.3.3.1/lib/libnetcdf.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/madingleyc__: /home/philju/Libraries/hdf5-1.8.15/lib/libhdf5_hl.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/madingleyc__: /home/philju/Libraries/hdf5-1.8.15/lib/libhdf5.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/madingleyc__: /home/philju/Libraries/zlib-1.2.8/lib/libz.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/madingleyc__: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/madingleyc__ ${OBJECTFILES} ${LDLIBSOPTIONS} -ldl

${OBJECTDIR}/src/Input/DataGrid.o: src/Input/DataGrid.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Input
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc -Isrc/Model -Isrc/Tools -Isrc/Input -Isrc/Output -I/home/philju/Libraries/netcdfcxx-4.2.1/include -I/home/philju/Libraries/netcdfc-4.3.3.1/include -I/home/philju/Libraries/hdf5-1.8.15/include -I/home/philju/Libraries/zlib-1.2.8/include/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/DataGrid.o src/Input/DataGrid.cpp

${OBJECTDIR}/src/Input/FileReader.o: src/Input/FileReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Input
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc -Isrc/Model -Isrc/Tools -Isrc/Input -Isrc/Output -I/home/philju/Libraries/netcdfcxx-4.2.1/include -I/home/philju/Libraries/netcdfc-4.3.3.1/include -I/home/philju/Libraries/hdf5-1.8.15/include -I/home/philju/Libraries/zlib-1.2.8/include/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/FileReader.o src/Input/FileReader.cpp

${OBJECTDIR}/src/Input/NcGridCell.o: src/Input/NcGridCell.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Input
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc -Isrc/Model -Isrc/Tools -Isrc/Input -Isrc/Output -I/home/philju/Libraries/netcdfcxx-4.2.1/include -I/home/philju/Libraries/netcdfc-4.3.3.1/include -I/home/philju/Libraries/hdf5-1.8.15/include -I/home/philju/Libraries/zlib-1.2.8/include/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Input/NcGridCell.o src/Input/NcGridCell.cpp

${OBJECTDIR}/src/Model/Cohort.o: src/Model/Cohort.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Model
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc -Isrc/Model -Isrc/Tools -Isrc/Input -Isrc/Output -I/home/philju/Libraries/netcdfcxx-4.2.1/include -I/home/philju/Libraries/netcdfc-4.3.3.1/include -I/home/philju/Libraries/hdf5-1.8.15/include -I/home/philju/Libraries/zlib-1.2.8/include/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Model/Cohort.o src/Model/Cohort.cpp

${OBJECTDIR}/src/Model/Environment.o: src/Model/Environment.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Model
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc -Isrc/Model -Isrc/Tools -Isrc/Input -Isrc/Output -I/home/philju/Libraries/netcdfcxx-4.2.1/include -I/home/philju/Libraries/netcdfc-4.3.3.1/include -I/home/philju/Libraries/hdf5-1.8.15/include -I/home/philju/Libraries/zlib-1.2.8/include/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Model/Environment.o src/Model/Environment.cpp

${OBJECTDIR}/src/Model/Stock.o: src/Model/Stock.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Model
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc -Isrc/Model -Isrc/Tools -Isrc/Input -Isrc/Output -I/home/philju/Libraries/netcdfcxx-4.2.1/include -I/home/philju/Libraries/netcdfc-4.3.3.1/include -I/home/philju/Libraries/hdf5-1.8.15/include -I/home/philju/Libraries/zlib-1.2.8/include/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Model/Stock.o src/Model/Stock.cpp

${OBJECTDIR}/src/Output/Logger.o: src/Output/Logger.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Output
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc -Isrc/Model -Isrc/Tools -Isrc/Input -Isrc/Output -I/home/philju/Libraries/netcdfcxx-4.2.1/include -I/home/philju/Libraries/netcdfc-4.3.3.1/include -I/home/philju/Libraries/hdf5-1.8.15/include -I/home/philju/Libraries/zlib-1.2.8/include/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Output/Logger.o src/Output/Logger.cpp

${OBJECTDIR}/src/Program.o: src/Program.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc -Isrc/Model -Isrc/Tools -Isrc/Input -Isrc/Output -I/home/philju/Libraries/netcdfcxx-4.2.1/include -I/home/philju/Libraries/netcdfc-4.3.3.1/include -I/home/philju/Libraries/hdf5-1.8.15/include -I/home/philju/Libraries/zlib-1.2.8/include/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Program.o src/Program.cpp

${OBJECTDIR}/src/Tools/Convertor.o: src/Tools/Convertor.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/Tools
	${RM} "$@.d"
	$(COMPILE.cc) -g -Isrc -Isrc/Model -Isrc/Tools -Isrc/Input -Isrc/Output -I/home/philju/Libraries/netcdfcxx-4.2.1/include -I/home/philju/Libraries/netcdfc-4.3.3.1/include -I/home/philju/Libraries/hdf5-1.8.15/include -I/home/philju/Libraries/zlib-1.2.8/include/ -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Tools/Convertor.o src/Tools/Convertor.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/madingleyc__

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
