// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SerialObject.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Category = "SerialPortCommunication")
class SERIALPORTCOMMUNICATION_API USerialObject : public UObject
{
	GENERATED_BODY()

public:
	USerialObject();
	~USerialObject();

	//打开一个串口
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Open Serial Port"), Category = "SerialPortCommunication")
	static USerialObject* OpenSerialPort(bool& bOpened, int32 Port = 1, int32 BaudRate = 9600);

	//关闭串口
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Close Port"), Category = "SerialPortCommunication")
	void Close();

	//检查端口是否打开
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Is Port Open"), Category = "SerialPortCommunication")
	bool IsOpened() const { return PortHandle != nullptr; }

	//获取串口号
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Port Number"), Category = "SerialPortCommunication")
	int32 GetPort() { return m_Port; }

	//获取波特率
	UFUNCTION(BlueprintCallable, Category = "SerialPortCommunication")
	int32 GetBaudRate() { return m_BaudRate; }

private:
	//打开串口
	bool OpenPort(int32 Port = 1, int32 BaudRate = 9600);

public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read a Float"), Category = "SerialPortCommunication")
	float ReadFloat(bool& bSuccess);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read a Int"), Category = "SerialPortCommunication")
	int32 ReadInt(bool& bSuccess);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read a Byte"), Category = "SerialPortCommunication")
	uint8 ReadByte(bool& bSuccess);

	UFUNCTION(BlueprintCallable, Category = "SerialPortCommunication")
	TArray<uint8> ReadBytes(int32 NumBytes = 256);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Read String"), Category = "SerialPortCommunication")
	FString ReadString(bool& bSuccess);

public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Write a Int"), Category = "SerialPortCommunication")
	bool WriteInt(int32 Value);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Write a Float"), Category = "SerialPortCommunication")
	bool WriteFloat(float Value);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Write a Byte"), Category = "SerialPortCommunication")
	bool WriteByte(uint8 Value);

	UFUNCTION(BlueprintCallable, Category = "SerialPortCommunication")
	bool WriteBytes(TArray<uint8> Buffer);

protected:
	//串口句柄
	Windows::HANDLE PortHandle;

	int32 m_Port;
	int32 m_BaudRate;
};
