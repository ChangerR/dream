
#ifndef __CANDROID_LOGGER_H_INCLUDED__
#define __CANDROID_LOGGER_H_INCLUDED__

#include <android/log.h>
#include "ILogger.h"
#include <stdio.h>
#include <wchar.h>
#include <dstring.h>
class CAndroidLogger : public ILogger {
public:
	CAndroidLogger() {
		m_log = fopen("mnt/sdcard/dream/log.txt","w");
		level = ELL_INFORMATION;
	}

	virtual ~CAndroidLogger() {
		fflush(m_log);
		fclose(m_log);
	}
	
	void append_begine(ELOG_LEVEL ll) {
		switch(ll) {
			case ELL_DEBUG:
				fputs("DEBUG: ",m_log);
				break;
			case ELL_INFORMATION:
				fputs("INFO: ",m_log);
				break;
			case ELL_WARNING:
				fputs("WARNING: ",m_log);
				break;
			case ELL_ERROR:
				fputs("****ERROR: ",m_log);
				break;
			default:
				break;
		}
	}
	//! Returns the current set log level.
	virtual ELOG_LEVEL getLogLevel() const {
		return level;
	}

	virtual void setLogLevel(ELOG_LEVEL ll) {
		level = ll;
	}

	virtual void log(const c8* text, ELOG_LEVEL ll=ELL_INFORMATION) {
		__android_log_print(ANDROID_LOG_INFO, "Dream", "log: %s", text);   
		append_begine(ll);
		fputs(text,m_log);
		fputc('\n',m_log);
	}
	
	virtual void log(const c8* text, const c8* hint, ELOG_LEVEL ll=ELL_INFORMATION) {
		__android_log_print(ANDROID_LOG_INFO, "Dream", "log: %s", text);
		append_begine(ll);
		fputs(text,m_log);	
		fputc('\n',m_log);
	}

	virtual void log(const wchar_t* text, ELOG_LEVEL ll=ELL_INFORMATION) {
		__android_log_print(ANDROID_LOG_INFO, "Dream", "log: %s",stringc(text).c_str()); 
		append_begine(ll);
		fputws(text,m_log);
		fputc('\n',m_log);
	}
	virtual void log(const wchar_t* text, const wchar_t* hint, ELOG_LEVEL ll=ELL_INFORMATION) {
		__android_log_print(ANDROID_LOG_INFO, "Dream", "log: %s", stringc(text).c_str());   
		append_begine(ll);
		fputws(text,m_log);
		fputc('\n',m_log);
	}
private:
	ELOG_LEVEL level;
	FILE* m_log;
};

#endif // __CANDROID_LOGGER_H_INCLUDED__
