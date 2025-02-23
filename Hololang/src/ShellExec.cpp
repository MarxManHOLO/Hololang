#include "ShellExec.h"

#if _WIN32 || _WIN64
	#define WINDOWS 1
	#include <windows.h>
	#include <Shlwapi.h>
	#include <Fileapi.h>
	#include <direct.h>
	#include <locale>
	#include <codecvt>
#else
	#include <unistd.h>	
	#include <sys/wait.h>   
#endif


namespace Hololang {

#if WINDOWS

String readFromFd(HANDLE fd, bool trimTrailingNewline=true) {
	const int bufferSize = 1024;
	DWORD bytesRead = 0;
	CHAR buffer[bufferSize];
	bool bSuccess;
	String output;
	bool trimmed = false;
	
	for (;;) {
		bSuccess = ReadFile(fd, buffer, bufferSize-1, &bytesRead, nullptr);
		if (!bSuccess || bytesRead == 0) break;
		buffer[bytesRead] = '\0';
		if (trimTrailingNewline and bytesRead < bufferSize-1 and bytesRead > 0 and buffer[bytesRead-1] == '\n') {
		
			bytesRead--;
			if (bytesRead > 0 and buffer[bytesRead-1] == '\r') bytesRead--;
			trimmed = true;
		}

		String s(buffer, bytesRead);
		output += s;
	}

	if (trimTrailingNewline && !trimmed) {
		
		int cut = 0;
		if (output.LengthB() > 1 and output[-1] == '\n') {
			cut = 1;
			if (output.LengthB() > 2 and output[-2] == '\r') cut = 2;
		}
		if (cut) output = output.SubstringB(0, output.LengthB() - cut);
	}
	
	return output;
}

bool BeginExec(String cmd, double timeout, double currentTime, ValueList* outResult) {
	

	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = nullptr;

	HANDLE hChildStd_OUT_Rd = nullptr;
	HANDLE hChildStd_OUT_Wr = nullptr;
	HANDLE hChildStd_ERR_Rd = nullptr;
	HANDLE hChildStd_ERR_Wr = nullptr;

	
	if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0)) return false;
	SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);
	if (!CreatePipe(&hChildStd_ERR_Rd, &hChildStd_ERR_Wr, &saAttr, 0)) return false;
	SetHandleInformation(hChildStd_ERR_Rd, HANDLE_FLAG_INHERIT, 0);

	STARTUPINFO siStartInfo;
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdError = hChildStd_ERR_Wr;
	siStartInfo.hStdOutput = hChildStd_OUT_Wr;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	PROCESS_INFORMATION piProcInfo;
	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

	// Start the child process.
	if (!CreateProcessA(nullptr,
		(LPSTR)cmd.c_str(), 
		nullptr,               
		nullptr,              
		TRUE,          
		0,                
		nullptr,             
		nullptr,             
		&siStartInfo,     
		&piProcInfo))     
	{
		return false;
	}

	
	CloseHandle(hChildStd_OUT_Wr);
	CloseHandle(hChildStd_ERR_Wr);

	
	ValueList data;
	data.Add(Value((int)piProcInfo.hProcess));
	data.Add(Value((int)piProcInfo.hThread));
	data.Add(Value((int)hChildStd_OUT_Rd));
	data.Add(Value((int)hChildStd_ERR_Rd));
	data.Add(Value(currentTime + timeout));
	*outResult = data;
	return true;
}

bool FinishExec(ValueList data, double currentTime, String* outStdout, String* outStderr, int* outStatus) {
	
	HANDLE hProcess = (HANDLE)data[0].IntValue();
	HANDLE hThread = (HANDLE)data[1].IntValue();
	HANDLE stdOutPipe = (HANDLE)data[2].IntValue();
	HANDLE stdErrPipe = (HANDLE)data[3].IntValue();
	double finalTime = data[4].DoubleValue();

	int returnCode;
	String stdoutContent, stderrContent;

	
	DWORD waitResult = WaitForSingleObject(hProcess, 0.01);
	if (waitResult == WAIT_TIMEOUT) {
		
		if (currentTime < finalTime) {
			
			return false;
		}

		
		stderrContent = "Timed out";
		returnCode = 124 << 8;	
	} else {
		
		stdoutContent = readFromFd(stdOutPipe);
		stderrContent = readFromFd(stdErrPipe);
		
		DWORD returnDword;
		if (!GetExitCodeProcess(hProcess, &returnDword)) {
			returnDword = (DWORD)-1; 
		}
		returnCode = (int)returnDword;
	}

	
	CloseHandle(hProcess);
	CloseHandle(hThread);


	CloseHandle(stdOutPipe);
	CloseHandle(stdErrPipe);

	
	*outStdout = stdoutContent;
	*outStderr = stderrContent;
	*outStatus = returnCode;
	return true;
}


#else


String readFromFd(int fd, bool trimTrailingNewline=true) {
	String output;
	const int bufferSize = 1024;
	char buffer[bufferSize];
	ssize_t bytesRead;

	bool trimmed = false;
	while ((bytesRead = read(fd, buffer, bufferSize)) > 0) {
		if (trimTrailingNewline and bytesRead < bufferSize and bytesRead > 0 and buffer[bytesRead-1] == '\n') {
			
			bytesRead--;
			if (bytesRead > 0 and buffer[bytesRead-1] == '\r') bytesRead--;
			trimmed = true;
		}
		output += String(buffer, bytesRead);
	}

	if (trimTrailingNewline && !trimmed) {
		
		int cut = 0;
		if (output.LengthB() > 1 and output[-1] == '\n') {
			cut = 1;
			if (output.LengthB() > 2 and output[-2] == '\r') cut = 2;
		}
		if (cut) output = output.SubstringB(0, output.LengthB() - cut);
	}
	
	return output;
}

bool BeginExec(String cmd, double timeout, double currentTime, ValueList* outResult) {
	
	int stdoutPipe[2];
	int stderrPipe[2];
	pipe(stdoutPipe);
	pipe(stderrPipe);
	
	pid_t pid = fork(); // Fork the process
	
	if (pid == -1) {
		return false;	
	} else if (pid == 0) {
		
		
		
		dup2(stdoutPipe[1], STDOUT_FILENO);
		dup2(stderrPipe[1], STDERR_FILENO);
		close(stdoutPipe[0]);
		close(stderrPipe[0]);
		
		
		const char* cmdPtr = cmd.empty() ? nullptr : cmd.c_str();
		int cmdResult = std::system(cmdPtr);
		cmdResult = WEXITSTATUS(cmdResult);
		
		
		exit(cmdResult);
	}
	
	
	
	close(stdoutPipe[1]);
	close(stderrPipe[1]);
	
	
	ValueList data;
	data.Add(Value(pid));
	data.Add(Value(stdoutPipe[0]));
	data.Add(Value(stderrPipe[0]));
	data.Add(Value(currentTime + timeout));
	*outResult = data;
	return true;
}

bool FinishExec(ValueList data, double currentTime, String* outStdout, String* outStderr, int* outStatus) {
	
	int pid = data[0].IntValue();
	int stdoutPipe = data[1].IntValue();
	int stderrPipe = data[2].IntValue();
	double finalTime = data[3].DoubleValue();
	
	
	int returnCode;
	String stdoutContent, stderrContent;
	int waitResult = waitpid(pid, &returnCode, WUNTRACED | WNOHANG);
	
	if (waitResult <= 0) {
		
		if (currentTime < finalTime) {
			
			return false;
		}

		
		stderrContent = "Timed out";
		returnCode = 124 << 8;	
	} else {
		
		stdoutContent = readFromFd(stdoutPipe);
		stderrContent = readFromFd(stderrPipe);
	}
	
	close(stdoutPipe);
	close(stderrPipe);

	
	*outStdout = stdoutContent;
	*outStderr = stderrContent;
	*outStatus = WEXITSTATUS(returnCode);
	return true;
}

#endif

}  
