// RestoreHandlerFin.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <sstream> 
#include <set>
#include <Windows.h>
#include "BaseRecoverySubService.cpp"

#include "data.pb.h"
#pragma warning(disable:4996)
int main()
{

	std::set<RestoreStatus::RestoreStatusSet> success_set;
	std::set<RestoreStatus::RestoreStatusSet> failure_set;
	std::set<RestoreStatus::RestoreStatusSet> my_set;
	success_set.insert(RestoreStatus::RestoreStatusSet(1, "sample\\filepath1", 0, "delete"));
	success_set.insert(RestoreStatus::RestoreStatusSet(2, "sample\\filepate2", 0, "restore"));
	failure_set.insert(RestoreStatus::RestoreStatusSet(3,"sample\\filepath3", 2, "restore"));

	std::map<int, std::set<RestoreStatus::RestoreStatusSet>> ResultMap;
	//std::map<int, std::set<RestoreStatus::RestoreStatusSet>> Map_received;
	ResultMap[1] = success_set;
	ResultMap[2] = failure_set;
	RestoreStatus* PRestoreStaus = new RestoreStatus();
	PRestoreStaus->setMapValue(ResultMap);
	//std::cout << "Printing map" << std::endl;
	//PRestoreStaus->printMap();
	std::cout << "\t\t..........Name pipe server........." << std::endl;
	//Name pipe local var
	HANDLE hCreateNamedpipe;
	char szInputBuffer[1023];
	char szOutputBuffer[1023];
	DWORD dwszInputBuffer = sizeof(szInputBuffer);
	DWORD dwszOutputBuffer = sizeof(szOutputBuffer);
	BOOL bConnectNamedPipe;
	// ConnectNAmedPirpe Local Var


	//Write file local var
	BOOL bWritefile;
	char szWriteFileBuffer[5000] = "hello";
	DWORD dwWrtieBufferSize = sizeof(szWriteFileBuffer);
	DWORD dwNoBytesWrite;

	//FLUSH Local Var
	BOOL bFlushFileBuffer;
	//Read local var
	BOOL bReadfile;
	char szReadFileBuffer[1023];
	DWORD dwReadBufferSize = sizeof(szReadFileBuffer);
	DWORD dwNoBytesRead;

	//proto-declaration
	Recovery_Response recovery;
	//CreateNamedPiep 
	hCreateNamedpipe = CreateNamedPipe(
		L"\\\\.\\pipe\\MYNAMEDPIPE",
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		dwszOutputBuffer,
		dwszInputBuffer,
		0, NULL);
	if (hCreateNamedpipe == INVALID_HANDLE_VALUE) {
		std::cout << "Failed to create pipe GLE= " << GetLastError() << std::endl;
	}
	else {
		std::cout << "Named pipe creation success" << std::endl;
	}

	//Connect pipe
	bConnectNamedPipe = ConnectNamedPipe(hCreateNamedpipe, NULL);
	if (bConnectNamedPipe == FALSE) {
		std::cout << "Connection failed to pipe GLE= " << GetLastError() << std::endl;
	}
	std::cout << "Connection established successfully" << std::endl;

	//get susid
	bReadfile = ReadFile(
		hCreateNamedpipe,
		szReadFileBuffer,
		dwReadBufferSize,
		&dwNoBytesRead,
		NULL);
	if (bReadfile == FALSE) {
		std::cout << "ReadFile failed GLE = " << GetLastError() << std::endl;
	}
	std::cout << "ReadFile Success" << std::endl;
	bFlushFileBuffer = FlushFileBuffers(hCreateNamedpipe);
	if (bFlushFileBuffer == FALSE) {
		std::cout << "Flush File Buffer failed GLE = " << GetLastError() << std::endl;
	}
	std::cout << "FlushFileBuffer Success" << std::endl;


	/*
	add implementation of uuid
	*/

	//successReport
	std::string message;
	my_set = ResultMap[1];
	for (auto itr = my_set.begin(); itr != my_set.end(); ++itr) {
		RestoreStatus::RestoreStatusSet status = *itr;
		SuccessReport* sr = recovery.add_successreport();
		sr->set_filepath(status.GetFilePath());
		sr->set_errorcode(status.GetErrorCode());
		sr->set_operation(status.GetOperation());
		sr->set_r_id(status.GetId());
	}
	//failureReport
	my_set.clear();
	my_set = ResultMap[2];

	for (auto itr = my_set.begin(); itr != my_set.end(); ++itr) {
		RestoreStatus::RestoreStatusSet status = *itr;

		FailueReport* fr = recovery.add_failurereport();
		fr->set_filepath(status.GetFilePath());
		fr->set_errorcode(status.GetErrorCode());
		fr->set_operation(status.GetOperation());
		fr->set_r_id(status.GetId());
	}

	recovery.SerializeToString(&message);
	strncpy(szWriteFileBuffer, message.c_str(), sizeof(szWriteFileBuffer));
	szWriteFileBuffer[sizeof(szWriteFileBuffer) - 1] = 0;
	std::cout << "Wrintg cknter : " << szWriteFileBuffer << std::endl;
	bWritefile = WriteFile(
		hCreateNamedpipe,
		szWriteFileBuffer,
		dwWrtieBufferSize,
		&dwNoBytesWrite,
		NULL);
	if (bWritefile == FALSE) {
		std::cout << "Write File failed -> GLE = " << GetLastError() << std::endl;
	}
	else {
		std::cout << "Write file success" << std::endl;
	}


	google::protobuf::ShutdownProtobufLibrary();

	//Dissconnet
	DisconnectNamedPipe(hCreateNamedpipe);

	//Close Handle
	CloseHandle(hCreateNamedpipe);
	system("PAUSE");
}


/*bFlushFileBuffer = FlushFileBuffers(hCreateNamedpipe);
	if (bFlushFileBuffer == FALSE) {
		std::cout << "Flush File Buffer failed GLE = " << GetLastError() << std::endl;
	}
	std::cout << "FlushFileBuffer Success" << std::endl;
	//ReadFile
	bReadfile = ReadFile(
		hCreateNamedpipe,
		szReadFileBuffer,
		dwReadBufferSize,
		&dwNoBytesRead,
		NULL);
	if (bReadfile == FALSE) {
		std::cout << "ReadFile failed GLE = " << GetLastError() << std::endl;
	}
	std::cout << "ReadFile Success" << std::endl;
	std::cout << "Data read -> " << szReadFileBuffer << std::endl;
	*/

	//cin >> szWriteFileBuffer;
	//std::cin.getline(szWriteFileBuffer, 1023);0c
	//const void * address = static_cast<const void*>(&ResultMap);
	//std::stringstream ss;
	//ss << address;
	//std::string adr = ss.str();
	//string temp = "cat";
	//char tab2[1024];
	//strncpy(szWriteFileBuffer, adr.c_str(), sizeof(szWriteFileBuffer));
	//szWriteFileBuffer[sizeof(szWriteFileBuffer) - 1] = 0;
	//szWriteFileBuffer = tab2;
	//std::cout << szWriteFileBuffer;
	//Write Operation0
	//char szWriteFileBuffer[1024] = "hello";