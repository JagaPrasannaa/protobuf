// ClientApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<Windows.h>
#include<stdio.h>
#include<fstream>
#include <set>
#include <map>
#include "data.pb.h"
using namespace std;

class RestoreStatusSet {
	int r_id;
	std::string FilePath;
	int ErrorCode;
	std::string operation;//[Restore/delete]
public:
	RestoreStatusSet(int id, std::string fPath, int Error_Code, std::string opr) {
		r_id = id;
		FilePath = fPath;
		ErrorCode = Error_Code;
		operation = opr;
	};
	std::string GetFilePath() {
		return this->FilePath;
	};
	int GetErrorCode() {
		return this->ErrorCode;
	};
	std::string GetOperation() {
		return this->operation;
	};
	bool operator<(const RestoreStatusSet& t) const
	{
		return (this->r_id < t.r_id);
	}
};
int main()
{

	std::map<int, std::set<RestoreStatusSet>>* ResultMap;
	std::set<RestoreStatusSet>* success_set;
	std::set<RestoreStatusSet> failure_set;
	std::cout << "Hello World!\n";
	cout << "\t\t...........Client  PIPE........." << endl;
	HANDLE hCreateNamedPipe;
	//Local var defenition
	BOOL bReadFile;
	DWORD dwNoBytesRead;
	char szReadFileBuffer[5000];
	DWORD dwszReadFileBuffersize = sizeof(szReadFileBuffer);
	//WriteFile Local Var
	BOOL bWriteFile;
	DWORD dwNoBytesWrite;
	char szWriteFileBuffer[1023] = "96569846541356854";
	DWORD dwszWriteFileBuffersize = sizeof(szWriteFileBuffer);

	hCreateNamedPipe = CreateFile(
		L"\\\\.\\pipe\\MYNAMEDPIPE",
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (hCreateNamedPipe == INVALID_HANDLE_VALUE) {
		cout << "Failed to create pipe GLE= " << GetLastError() << endl;
	}
	else {
		cout << "Named pipe creation success" << endl;
	}
	//Write Operation
	bWriteFile = WriteFile(
		hCreateNamedPipe,
		szWriteFileBuffer,
		dwszWriteFileBuffersize,
		&dwNoBytesWrite,
		NULL);
	if (bWriteFile == FALSE) {
		cout << "Write File failed -> GLE = " << GetLastError() << endl;
	}
	else {
		cout << "Write file success" << endl;
	}
	//ReadFile
	bool bFlushFileBuffer;
	bFlushFileBuffer = FlushFileBuffers(hCreateNamedPipe);
	if (bFlushFileBuffer == FALSE) {
		std::cout << "Flush File Buffer failed GLE = " << GetLastError() << std::endl;
	}
	std::cout << "FlushFileBuffer Success" << std::endl;

	cout << "Reading" << endl;
	//cout << "Date Reading -> " << (szReadFileBuffer) << endl;
	bReadFile = ReadFile(
		hCreateNamedPipe,
		szReadFileBuffer,
		dwszReadFileBuffersize,
		&dwNoBytesRead,
		NULL);
	if (bReadFile == FALSE) {
		cout << "ReadFile failed GLE = " << GetLastError() << endl;
	}

	Recovery_Response recovery;
	string message;
	recovery.ParseFromArray(&szReadFileBuffer, sizeof(szReadFileBuffer));
	cout << "Status Success" << endl;
	for (int i = 0; i < recovery.successreport_size(); i++) {
		const SuccessReport* sr = &recovery.successreport(i);
		cout << "FilePath: " << sr->filepath() << " Operation: " << sr->operation() << " ErrorCode: " << sr->errorcode() << endl;
	}
	cout << "Status Failed" << endl;
	for (int i = 0; i < recovery.failurereport_size(); i++) {
		const FailueReport* fr = &recovery.failurereport(i);
		cout << "FilePath: " << fr->filepath() << " Operation: " << fr->operation() << " ErrorCode: " << fr->errorcode() << endl;
	}
	google::protobuf::ShutdownProtobufLibrary();
	DisconnectNamedPipe(hCreateNamedPipe);

	//Close Handle
	CloseHandle(hCreateNamedPipe);
}