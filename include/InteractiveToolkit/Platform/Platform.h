#pragma once

#include "platform_common.h"

#include "Core/ObjectBuffer.h"
#include "Core/ObjectPool.h"
#include "Core/ObjectQueue.h"

#include "AutoLock.h"
#include "Mutex.h"
#include "Process.h"
#include "Semaphore.h"
#include "Signal.h"
#include "Sleep.h"
#include "SocketTCP.h"
#include "SocketUDP.h"
#include "Thread.h"
#include "ThreadWithParameters.h"
#include "Time.h"

//
// IPC
//
#include "IPC/BufferIPC.h"
#include "IPC/LowLatencyQueueIPC.h"
#include "IPC/QueueIPC.h"
#include "IPC/SemaphoreIPC.h"

//
// Tool
//
#include "Tool/DebugConsoleIPC.h"
#include "Tool/GetFirstMacAddress.h"

