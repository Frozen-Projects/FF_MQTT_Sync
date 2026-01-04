#pragma once

#include "CoreMinimal.h"
#include "Paho_Tools.generated.h"

USTRUCT(BlueprintType)
struct FF_MQTT_SYNC_API FPahoSslOptions
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FString CAPath;

	UPROPERTY(BlueprintReadWrite)
	FString Path_KeyStore;

	UPROPERTY(BlueprintReadWrite)
	FString Path_TrustStore;

	UPROPERTY(BlueprintReadWrite)
	FString Path_PrivateKey;

	UPROPERTY(BlueprintReadWrite)
	FString PrivateKeyPass;

	UPROPERTY(BlueprintReadWrite)
	FString CipherSuites;

	bool operator == (const FPahoSslOptions& Other) const
	{
		return CAPath == Other.CAPath && Path_KeyStore == Other.Path_KeyStore && Path_TrustStore == Other.Path_TrustStore && Path_PrivateKey == Other.Path_PrivateKey
			&& PrivateKeyPass == Other.PrivateKeyPass && CipherSuites == Other.CipherSuites;
	}

	bool operator != (const FPahoSslOptions& Other) const
	{
		return !(*this == Other);
	}
};

FORCEINLINE uint32 GetTypeHash(const FPahoSslOptions& Key)
{
	uint32 Hash_CAPath = GetTypeHash(Key.CAPath);
	uint32 Hash_KeyStore = GetTypeHash(Key.Path_KeyStore);
	uint32 Hash_TrustStore = GetTypeHash(Key.Path_TrustStore);
	uint32 Hash_PrivateKey = GetTypeHash(Key.Path_PrivateKey);
	uint32 Hash_PrivateKeyPass = GetTypeHash(Key.PrivateKeyPass);
	uint32 Hash_Cipher = GetTypeHash(Key.CipherSuites);

	uint32 GenericHash;
	FMemory::Memset(&GenericHash, 0, sizeof(uint32));
	GenericHash = HashCombine(GenericHash, Hash_CAPath);
	GenericHash = HashCombine(GenericHash, Hash_KeyStore);
	GenericHash = HashCombine(GenericHash, Hash_TrustStore);
	GenericHash = HashCombine(GenericHash, Hash_PrivateKey);
	GenericHash = HashCombine(GenericHash, Hash_PrivateKeyPass);
	GenericHash = HashCombine(GenericHash, Hash_Cipher);

	return GenericHash;
}

USTRUCT(BlueprintType)
struct FF_MQTT_SYNC_API FPahoClientParams
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FString ClientId;

	UPROPERTY(BlueprintReadWrite)
	FString Address;

	UPROPERTY(BlueprintReadWrite)
	FString UserName;

	UPROPERTY(BlueprintReadWrite)
	FString Password;

	UPROPERTY(BlueprintReadWrite)
	int32 KeepAliveInterval = 20;

	UPROPERTY(BlueprintReadWrite)
	EMQTTVERSION Version = EMQTTVERSION::Default;

	UPROPERTY(BlueprintReadWrite)
	FPahoSslOptions SSL_Options;

	bool IsParamsValid(FString& Out_Code) const
	{
		if (Address.IsEmpty())
		{
			Out_Code = "Address is empty.";
			return false;
		}

		if (this->ClientId.IsEmpty())
		{
			Out_Code = "ClientId is empty.";
			return false;
		}

		Out_Code = "Parameters is valid.";
		return true;
	};

	FString GetProtocol() const
	{
		TArray<FString> URL_Sections = UKismetStringLibrary::ParseIntoArray(Address, "://");

		if (URL_Sections.Num() <= 1)
		{
			return "";
		}

		return URL_Sections[0];
	}

	bool operator == (const FPahoClientParams& Other) const
	{
		return ClientId == Other.ClientId && Address == Other.Address && UserName == Other.UserName && Password == Other.Password && KeepAliveInterval == Other.KeepAliveInterval &&
			Version == Other.Version && SSL_Options == Other.SSL_Options;
	}

	bool operator != (const FPahoClientParams& Other) const
	{
		return !(*this == Other);
	}
};

FORCEINLINE uint32 GetTypeHash(const FPahoClientParams& Key)
{
	uint32 Hash_ClientId = GetTypeHash(Key.ClientId);
	uint32 Hash_Address = GetTypeHash(Key.Address);
	uint32 Hash_Username = GetTypeHash(Key.UserName);
	uint32 Hash_Password = GetTypeHash(Key.Password);
	uint32 Hash_Interval = GetTypeHash(Key.KeepAliveInterval);
	uint32 Hash_Version = GetTypeHash(Key.Version);
	uint32 Hash_SSL_Options = GetTypeHash(Key.SSL_Options);

	uint32 GenericHash;
	FMemory::Memset(&GenericHash, 0, sizeof(uint32));
	GenericHash = HashCombine(GenericHash, Hash_ClientId);
	GenericHash = HashCombine(GenericHash, Hash_Address);
	GenericHash = HashCombine(GenericHash, Hash_Username);
	GenericHash = HashCombine(GenericHash, Hash_Password);
	GenericHash = HashCombine(GenericHash, Hash_Interval);
	GenericHash = HashCombine(GenericHash, Hash_Version);
	GenericHash = HashCombine(GenericHash, Hash_SSL_Options);

	return GenericHash;
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegate_Paho_Int, int32, Out_Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegate_Paho_String, FString, Out_Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegate_Paho_Json, FJsonObjectWrapper, Out_Result);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegate_Paho_Connection, bool, bIsSuccessfull, FJsonObjectWrapper, Out_Code);