#ifndef AW_LOGGING_HPP
#define AW_LOGGING_HPP

#include <fstream>
#include "../Multicore/CriticalSection.hpp"

namespace AwLogging
{

	enum AppPhase
	{
		INIT = 0,
		MAIN = 1,
		TERM = 2
	};

	enum LogType
	{
		UNKNOWN = -1,
		STDOUT = 0,
		ERROR_,
		WARNING,
		ASSERTION,
		MEMLEAK //TODO: custom memory manager
	};

	struct QSE_API LogInfo
	{
	private:

		CriticalSection mcSec;

		unsigned long mNumErrors = 0;
		unsigned long mNumWarnings = 0;
		unsigned long mNumMemLeaks = 0;
		unsigned long mNumLogEntries = 0;

	public:

		void IncrementErrorCount()		{ SCOPED_CS(mcSec); mNumErrors++;		}
		void IncrementWarningCount()	{ SCOPED_CS(mcSec); mNumWarnings++;		}
		void IncrementMemLeakCount()	{ SCOPED_CS(mcSec); mNumMemLeaks++;		}
		void IncrementLogEntryCount()	{ SCOPED_CS(mcSec); mNumLogEntries++;	}


		unsigned long GetErrorCount()    const	{ return mNumErrors;		}
		unsigned long GetWarningCount()  const	{ return mNumWarnings;		}
		unsigned long GetMemLeakCount()  const	{ return mNumMemLeaks;		}
		unsigned long GetLogEntryCount() const	{ return mNumLogEntries;	}

	};
	extern LogInfo gLogInfo;



	//the last three paramters can be disabled in the config (Debug/Release) 
	//NEVER call this directly
	QSE_API void WriteLog(LogType logType, string logText, const char* funcName, const char* sourceFile, unsigned int lineNum);

	//some utilities for WriteLog, but can be used in other places
	string LogTypeToString(LogType logType);
	LogType StringToLogType(string logType);
	void IncrementProperLogTypes(LogType logType);


	//to initialize the system
	bool Init();

	//this saves the file
	void Destroy();
}


//some preprocessors for easier logging

#define LOG_ERROR(logMessage) \
	AwLogging::WriteLog(AwLogging::LogType::ERROR_, logMessage, __FUNCTION__, __FILE__, __LINE__);

#define LOG_WARNING(logMessage) \
	AwLogging::WriteLog(AwLogging::LogType::WARNING , logMessage, __FUNCTION__, __FILE__, __LINE__);

#define LOG_WRITE(logMessage) \
	AwLogging::WriteLog(AwLogging::LogType::STDOUT, logMessage, __FUNCTION__, __FILE__, __LINE__);

#define LOG_MEMLEAK(logMessage) \
	AwLogging::WriteLog(AwLogging::LogType::MEMLEAK, logMessage, __FUNCTION__, __FILE__, __LINE__);

//if this fails, it pulls up a detal box, and then pulls up a debuggible dialog that the OS  makes
#define LOG_ASSERT(expr) \
		do \
				{ \
			if (!(expr)) \
						{ \
				AwLogging::WriteLog(AwLogging::LogType::ASSERTION, #expr, __FUNCTION__, __FILE__, __LINE__); \
				assert(false);\
			} \
		} \
			while (0) \



#endif