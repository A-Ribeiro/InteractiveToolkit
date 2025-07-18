#pragma once

#include "platform_common.h"

#include "Core/ObjectBuffer.h"
#include "Core/ObjectPool.h"
#include "Core/ObjectQueue.h"
#include "Core/SmartVector.h"

#include "AutoLock.h"
#include "Mutex.h"
#include "Process.h"
#include "Semaphore.h"
#include "Signal.h"
#include "Sleep.h"
#include "SocketTCP.h"
#include "SocketUDP.h"
#include "Thread.h"
#include "ThreadPool.h"
#include "ThreadWithParameters.h"
#include "Time.h"

#include "AutoLockSemaphore.h"

#include "Condition.h"

//
// IPC
//
#include "IPC/BufferIPC.h"
#include "IPC/LowLatencyQueueIPC.h"
#include "IPC/QueueIPC.h"
#include "IPC/SemaphoreIPC.h"

#include "IPC/AutoLockSemaphoreIPC.h"

#include "IPC/ConditionIPC.h"

//
// Tool
//
#include "Tool/DebugConsoleIPC.h"
#include "Tool/GetFirstMacAddress.h"

