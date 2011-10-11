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
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=g77
AS=as

# Macros
CND_PLATFORM=Cygwin-Windows
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/io.o \
	${OBJECTDIR}/fitness.o \
	${OBJECTDIR}/populacao.o \
	${OBJECTDIR}/sa.o \
	${OBJECTDIR}/agenda.o \
	${OBJECTDIR}/problema.o \
	${OBJECTDIR}/buscalocal.o \
	${OBJECTDIR}/auxiliar.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/util.o \
	${OBJECTDIR}/genetico.o \
	${OBJECTDIR}/individuo.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Release.mk dist/Release/Cygwin-Windows/alggenetico.exe

dist/Release/Cygwin-Windows/alggenetico.exe: ${OBJECTFILES}
	${MKDIR} -p dist/Release/Cygwin-Windows
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/alggenetico ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/io.o: nbproject/Makefile-${CND_CONF}.mk io.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/io.o io.c

${OBJECTDIR}/fitness.o: nbproject/Makefile-${CND_CONF}.mk fitness.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/fitness.o fitness.c

${OBJECTDIR}/populacao.o: nbproject/Makefile-${CND_CONF}.mk populacao.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/populacao.o populacao.c

${OBJECTDIR}/sa.o: nbproject/Makefile-${CND_CONF}.mk sa.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/sa.o sa.c

${OBJECTDIR}/agenda.o: nbproject/Makefile-${CND_CONF}.mk agenda.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/agenda.o agenda.c

${OBJECTDIR}/problema.o: nbproject/Makefile-${CND_CONF}.mk problema.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/problema.o problema.c

${OBJECTDIR}/buscalocal.o: nbproject/Makefile-${CND_CONF}.mk buscalocal.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/buscalocal.o buscalocal.c

${OBJECTDIR}/auxiliar.o: nbproject/Makefile-${CND_CONF}.mk auxiliar.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/auxiliar.o auxiliar.c

${OBJECTDIR}/main.o: nbproject/Makefile-${CND_CONF}.mk main.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.c

${OBJECTDIR}/util.o: nbproject/Makefile-${CND_CONF}.mk util.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/util.o util.c

${OBJECTDIR}/genetico.o: nbproject/Makefile-${CND_CONF}.mk genetico.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/genetico.o genetico.c

${OBJECTDIR}/individuo.o: nbproject/Makefile-${CND_CONF}.mk individuo.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/individuo.o individuo.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} dist/Release/Cygwin-Windows/alggenetico.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
