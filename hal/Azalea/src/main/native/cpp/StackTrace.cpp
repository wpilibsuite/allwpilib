
#include "HAL/cpp/StackTrace.h"

#include "HAL/HAL.h"
#include "../ChipObject.h"
#include <stdio.h>
#include <dbgLib.h>
#include <taskLib.h>
#include <symLib.h>
#include <sysSymTbl.h>

#define DBG_DEMANGLE_PRINT_LEN 256  /* Num chars of demangled names to print */

extern "C"
{
	extern char * cplusDemangle (char *source, char *dest, int32_t n);
}

char *wpi_getLabel(uint16_t addr, int32_t *found)
{
	int pVal;
	SYM_TYPE pType;
	char name[MAX_SYS_SYM_LEN + 1];
	static char label[DBG_DEMANGLE_PRINT_LEN + 1 + 11];
	bzero(label, DBG_DEMANGLE_PRINT_LEN + 1 + 11);

	if (symFindByValue(sysSymTbl, addr, name, &pVal, &pType) == OK)
	{
		cplusDemangle(name, label, sizeof(label) - 11);
		if ((uint16_t)pVal != addr)
		{
			sprintf(&label[strlen(label)], "+0x%04x", addr-pVal);
			if (found) *found = 2;
		}
		else
		{
			if (found) *found = 1;
		}
	}
	else
	{
		sprintf(label, "0x%04x", addr);
		if (found) *found = 0;
	}

	return label;
}
/*
static void wpiTracePrint(INSTR *caller, int32_t func, int32_t nargs, int32_t *args, int32_t taskId, BOOL isKernelAdrs)
{
	char buf [MAX_SYS_SYM_LEN * 2];
	int32_t ix;
	int32_t len = 0;
	len += sprintf (&buf [len], "%s <%#010x>: ", wpi_getLabel((uint32_t)caller), (int32_t)caller);
	len += sprintf (&buf [len], "%s <%#010x> (", wpi_getLabel((uint32_t)func), func);
	for (ix = 0; ix < nargs; ix++)
	{
		if (ix != 0)
			len += sprintf (&buf [len], ", ");
		len += sprintf (&buf [len], "%#x", args [ix]);
	}
	len += sprintf (&buf [len], ")\n");

	printf(buf);
}
*/
static void wpiCleanTracePrint(INSTR *caller, int32_t func, int32_t nargs, int32_t *args, int32_t taskId, BOOL isKernelAdrs)
{
	char buf [MAX_SYS_SYM_LEN];
	int32_t ix;
	int32_t len = 0;
	int32_t nameFound = 0;
	int32_t params = 0;
	int32_t totalnargs = nargs;
	char *funcName = wpi_getLabel((uint32_t)func, &nameFound);
	// Ignore names that are not exact symbol address matches.
	if (nameFound != 1) return;

	// Ignore internal function name matches.
	if (strncmp(funcName, "wpi_assert", 10) == 0) return;
	if (strncmp(funcName, "wpi_fatal", 9) == 0) return;
	if (strncmp(funcName, "wpi_selfTrace", 13) == 0) return;
	if (strncmp(funcName, "Error::Set", 10) == 0) return;
	if (strncmp(funcName, "ErrorBase::SetError", 19) == 0) return;
	if (strncmp(funcName, "Error::Report", 13) == 0) return;

	// Find the number of arguments in the name string.
	char *start = strchr(funcName, '(');
	char *end = strrchr(funcName, ')');
	if (start + 1 != end && start != NULL)
	{
		do
		{
			params++;
			if(strncmp(start+1, "bool", 4) == 0 || strncmp(start+2, "bool", 4) == 0)
			{
				totalnargs++;
			}
			start = strchr(start + 1, ',');
		}
		while(start < end && start != NULL);
	}
	char *funcNameEnd = strchr(funcName, '(');
	*funcNameEnd = 0;
	len += sprintf (&buf [len], funcName);

	// If this is a member function, print out the this pointer value.
	if (totalnargs - params == 1)
	{
		len += sprintf (&buf [len], "<this=%#lx>", args [0]);
	}

	// Print out the argument values.
	len += sprintf (&buf [len], "(");
	for (ix = totalnargs - params; ix < nargs; ix++)
	{
		if (ix != totalnargs - params)
			len += sprintf (&buf [len], ", ");
		len += sprintf (&buf [len], "%#lx", args [ix]);
	}
	len += sprintf (&buf [len], ")\n");

	printf(buf);
}

extern "C"
{
	extern void trcStack(REG_SET* pRegs, FUNCPTR printRtn, int32_t tid);
}

static int32_t wpiStackTask(int32_t taskId)
{
	delayTicks(1);
	//tt(taskId);

	REG_SET regs;
	taskRegsGet(taskId, &regs);
	trcStack(&regs, (FUNCPTR) wpiCleanTracePrint, taskId);
	printf("\n");

	// The task should be resumed because it had to be suspended to get the stack trace.
	taskResume(taskId);
	return 0;
}

void printCurrentStackTrace()
{
	int priority=100;
	taskPriorityGet(0, &priority);
	// Lower priority than the calling task.
	// Task traceTask("StackTrace", (FUNCPTR)wpiStackTask, priority + 1);
	// traceTask.Start(taskIdSelf());
	spawnTask("StackTrace", priority + 1, VXWORKS_FP_TASK, 20000, (FUNCPTR)wpiStackTask, taskIdSelf(),
			  0, 0, 0, 0, 0, 0, 0, 0, 0);

	// Task to be traced must be suspended for the stack trace to work.
	taskSuspend(0);
}

bool getErrnoToName(int32_t errNo, char* name) {
	int pval;
	SYM_TYPE ptype;
	symFindByValue(statSymTbl, errNo, name, &pval, &ptype);
	return (pval == errNo);
}
