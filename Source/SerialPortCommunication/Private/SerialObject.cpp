// Fill out your copyright notice in the Description page of Project Settings.


#include "SerialObject.h"
#include <Windows.h>

USerialObject::USerialObject() : PortHandle(nullptr), m_Port(-1), m_BaudRate(-1)
{
}

USerialObject::~USerialObject()
{
	Close();
}

USerialObject* USerialObject::OpenSerialPort(bool& bOpened, int32 Port, int32 BaudRate)
{
	USerialObject* SerialObject = NewObject<USerialObject>();

	bOpened = SerialObject->OpenPort(Port, BaudRate);

	return SerialObject;
}


bool USerialObject::OpenPort(int32 Port, int32 BaudRate)
{
	if (Port < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("InValid Port Number %d"), Port);
		return false;
	}

	FString PortName = FString::Printf(TEXT("COMD%d"), Port);

	PortHandle = CreateFile(*PortName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
	                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	if (PortHandle == INVALID_HANDLE_VALUE)
	{
		unsigned long dwError = GetLastError();
		UE_LOG(LogTemp, Error, TEXT("Failed to open Port COM%d. Error : %08X"), Port, dwError);
		return false;
	}

	COMMTIMEOUTS Commtimeouts;
	Commtimeouts.ReadIntervalTimeout = 50;
	Commtimeouts.ReadTotalTimeoutConstant = 0;
	Commtimeouts.ReadTotalTimeoutMultiplier = 0;
	Commtimeouts.WriteTotalTimeoutConstant = 0;
	Commtimeouts.WriteTotalTimeoutMultiplier = 10;
	SetCommTimeouts(PortHandle, &Commtimeouts);

	DCB dcbSerialParams;
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(PortHandle, &dcbSerialParams))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed To Get Serial State"));
		return false;
	}

	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.BaudRate = BaudRate;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if (!SetCommState(PortHandle, &dcbSerialParams))
	{
		CloseHandle(PortHandle);
		PortHandle = nullptr;
		UE_LOG(LogTemp, Error, TEXT("Failed To Setup Port COM%d State. Error : %08x"), Port, GetLastError());
		return false;
	}

	AddToRoot();
	m_Port = Port;
	m_BaudRate = BaudRate;

	return true;
}

void USerialObject::Close()
{
	if (!PortHandle) return;

	CloseHandle(PortHandle);
	PortHandle = nullptr;

	RemoveFromRoot();
}


float USerialObject::ReadFloat(bool& bSuccess)
{
	bSuccess = false;

	TArray<uint8> Bytes = ReadBytes(4);
	if (Bytes.Num() == 0) return 0;

	bSuccess = true;
	return *reinterpret_cast<float*>(Bytes.GetData());
}

int32 USerialObject::ReadInt(bool& bSuccess)
{
	bSuccess = false;

	TArray<uint8> Bytes = ReadBytes(4);
	if (Bytes.Num() == 0) return 0;

	bSuccess = true;
	return *reinterpret_cast<int32*>(Bytes.GetData());
}

uint8 USerialObject::ReadByte(bool& bSuccess)
{
	bSuccess = false;
	if (!PortHandle) return 0x0;

	uint8 Byte = 0x0;
	unsigned long dwBytesRead;

	if (!ReadFile(PortHandle, &Byte, 1, &dwBytesRead, nullptr))
	{
		UE_LOG(LogTemp, Error, TEXT("Error On Read Byte.Error : %08x"), GetLastError());
		return Byte;
	}

	bSuccess = dwBytesRead > 0;
	return Byte;
}

TArray<uint8> USerialObject::ReadBytes(int32 NumBytes)
{
	TArray<uint8> Data;

	if (!PortHandle || NumBytes <= 0) return Data;

	Data.Empty(NumBytes);
	uint8* Buffer = new uint8[NumBytes];
	unsigned long dwBytesRead;

	if (!ReadFile(PortHandle, Buffer, NumBytes, &dwBytesRead, nullptr))
	{
		UE_LOG(LogTemp, Error, TEXT("Error On Read Bytes.Error : %08x"), GetLastError());
		return Data;
	}

	Data.Append(Buffer, dwBytesRead);
	return Data;
}

FString USerialObject::ReadString(bool& bSuccess)
{
	return TEXT("");
}

bool USerialObject::WriteInt(int32 Value)
{
	TArray<uint8> Buffer;
	Buffer.Append(reinterpret_cast<uint8*>(&Value), 4);
	return WriteBytes(Buffer);
}

bool USerialObject::WriteFloat(float Value)
{
	TArray<uint8> Buffer;
	Buffer.Append(reinterpret_cast<uint8*>(&Value), 4);
	return WriteBytes(Buffer);
}

bool USerialObject::WriteByte(uint8 Value)
{
	TArray<uint8> Buffer({Value});
	return WriteBytes(Buffer);
}

bool USerialObject::WriteBytes(TArray<uint8> Buffer)
{
	if (!PortHandle) return false;

	unsigned long dwBytesWritten;
	if (!WriteFile(PortHandle, Buffer.GetData(), Buffer.Num(), &dwBytesWritten, nullptr))
	{
		UE_LOG(LogTemp, Error, TEXT("Error on WriteBytes. Error : %08x"), GetLastError());
		return false;
	}

	return true;
}
