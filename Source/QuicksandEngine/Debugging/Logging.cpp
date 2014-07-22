#include "../Stdafx.hpp"
#include "Logging.hpp"
#include "../Config/ConfigManager.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <chrono>

namespace AwLogging
{
	CriticalSection gCritSec;

	std::ofstream gLog;
	LogInfo gLogInfo;

	string LogToString(LogType logType)
	{
		switch (logType)
		{
		case STDOUT:
			return "STDOUT";
		case ERROR_:
			return "ERROR";
		case WARNING:
			return "WARNING";
		case ASSERTION:
			return "ASSERT";
		case MEMLEAK:
			return "MEMLEAK";
		}
	}

	void WriteLog(LogType logType, string logMessage, const char* funcName, const char* sourceFile, unsigned int lineNum)
	{
		stringstream ss;

		//get the time for the log
		time_t t = time(0);
		struct tm * now = localtime(&t);
		ss << now->tm_mon + 1 << "/" << now->tm_mday << "/" << now->tm_year + 1900 << " " << now->tm_hour + 1 << "-" << now->tm_min << "-" << now->tm_sec;


		//now for the prefix of the message
		ss << "[" << LogToString(logType) << "]";


		//now for the location of the message
		//TODO: THREAD ID
		ss << " --" << funcName << "-- ";

		//if it is debug, add some extra stuff
		if (GET_CONFIG_ELEMENT_STR("APPLICATION_MODE") == "DEBUG");
		{
			ss << "--" << sourceFile << "-- --" << lineNum << "-- ";
		}

		//now output the actual message
		ss << "\'" << LogToString(logType) << "\'\n";

		//always output everything to the console until I impliment variable verbosity
		std::cout << ss.str();

		//now depending on the type, increment the correct value
		
		switch (logType)
		{
		case STDOUT:
			gLogInfo.IncrementLogEntryCount();
			break;
		case ERROR:
			gLogInfo.IncrementErrorCount();
			gLogInfo.IncrementLogEntryCount();
			break;
		case WARNING:
			gLogInfo.IncrementWarningCount();
			gLogInfo.IncrementLogEntryCount();
			break;
		case MEMLEAK:
			gLogInfo.IncrementMemLeakCount();
			gLogInfo.IncrementLogEntryCount();
			break;
		}
		
		//finally write it to the log stream
		SCOPED_CS(gCritSec);
		gLog << ss.str();
	}


	void Init()
	{
		//the name of the file will be the data added to the time
		stringstream ss;

		time_t t = time(0);
		struct tm * now = localtime(&t);
		ss << now->tm_mon + 1 << "/" << now->tm_mday << "/" << now->tm_year + 1900 << " " << now->tm_hour + 1 << "-" << now->tm_min << "-" << now->tm_sec;

		gLog.open(ss.str().c_str());
		gLog << "=====================\tHeader\t=====================\n";
	}

	void Destroy()
	{
		//add the footer of the logfile, then save it
		gLog << "===============LOGGING ENDED: Errors: " << gLogInfo.GetErrorCount() << "; Warnings: " << gLogInfo.GetWarningCount() << "; Memory Leaks: " << gLogInfo.GetMemLeakCount() << "; Total Log Entries: " << gLogInfo.GetLogEntryCount() << "; ========";


		//finally close the file once it is done
		gLog.close();
	}

	void IncrementProperLogTypes(LogType logType)
	{
		switch (logType)
		{
		case STDOUT:
			gLogInfo.IncrementLogEntryCount();
			break;
		case ERROR:
			gLogInfo.IncrementErrorCount();
			gLogInfo.IncrementLogEntryCount();
			break;
		case WARNING:
			gLogInfo.IncrementWarningCount();
			gLogInfo.IncrementLogEntryCount();
			break;
		case MEMLEAK:
			gLogInfo.IncrementMemLeakCount();
			gLogInfo.IncrementLogEntryCount();
			break;
		}
	}


	LogType StringToLogType(string logType)
	{
		//first make it all upercase
		std::transform(logType.begin(), logType.end(), logType.begin(), std::ptr_fun<int, int>(std::toupper));

		//now just compare, possibly make this more sophisticated
		if (logType == "STDOUT")
			return STDOUT;
		if (logType == "ERROR")
			return ERROR;
		if (logType == "WARNING")
			return WARNING;
		if (logType == "MEMLEAK")
			return MEMLEAK;


		//if it is none of them, than return the standard std::out becuase that is non-destructive
		return STDOUT;
	}
}