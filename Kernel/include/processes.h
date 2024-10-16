#ifndef PROCESSES_H
#define PROCESSES_H

#include "commonHeader.h"
#include "scheduler.h"

pid createProcess(createProcessInfo * info);

int kill(pid pid);

int sendToBackground(pid pid);

int bringToForeground(pid pid);

int isForeground(pid pid);

int listProcessesInfo(ProcessInfo * processes, int max_processes);

#endif