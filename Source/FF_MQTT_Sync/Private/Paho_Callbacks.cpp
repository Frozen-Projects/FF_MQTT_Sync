#include "Paho_Manager_Sync.h"

#pragma region Internals

const char* APaho_Manager_Sync::FStringToUTF8(const FString& In_String)
{
	const char* UTF8 = (const char*)StringCast<UTF8CHAR>(*In_String).Get();
	return UTF8;
}

FString APaho_Manager_Sync::UTF8ToFString(const char* In_Char, size_t Length)
{
	if (Length == 0)
	{
		return StringCast<TCHAR>(In_Char).Get();
	}

	else if (Length > 0)
	{
		return StringCast<TCHAR>(In_Char, Length).Get();
	}

	else
	{
		return FString();
	}
}

bool APaho_Manager_Sync::SetSSLParams(FString In_Protocol, FPahoSslOptions In_Options)
{
	if (In_Protocol == "wss" || In_Protocol == "mqtts" || In_Protocol == "ssl" || In_Protocol == "WSS" || In_Protocol == "MQTTS" || In_Protocol == "SSL")
	{
		this->SSL_Options = MQTTClient_SSLOptions_initializer;
		this->SSL_Options.enableServerCertAuth = 0;
		this->SSL_Options.verify = 1;

		if (!In_Options.CAPath.IsEmpty() && FPaths::FileExists(In_Options.CAPath))
		{
			this->SSL_Options.CApath = APaho_Manager_Sync::FStringToUTF8(*In_Options.CAPath);
		}

		if (!In_Options.Path_KeyStore.IsEmpty() && FPaths::FileExists(In_Options.Path_KeyStore))
		{
			this->SSL_Options.keyStore = APaho_Manager_Sync::FStringToUTF8(*In_Options.Path_KeyStore);
		}

		if (!In_Options.Path_TrustStore.IsEmpty() && FPaths::FileExists(In_Options.Path_TrustStore))
		{
			this->SSL_Options.trustStore = APaho_Manager_Sync::FStringToUTF8(*In_Options.Path_TrustStore);
		}

		if (!In_Options.Path_PrivateKey.IsEmpty() && FPaths::FileExists(In_Options.Path_PrivateKey))
		{
			this->SSL_Options.privateKey = APaho_Manager_Sync::FStringToUTF8(*In_Options.Path_PrivateKey);
		}

		if (!In_Options.PrivateKeyPass.IsEmpty())
		{
			this->SSL_Options.privateKeyPassword = APaho_Manager_Sync::FStringToUTF8(*In_Options.PrivateKeyPass);
		}

		if (!In_Options.CipherSuites.IsEmpty())
		{
			this->SSL_Options.enabledCipherSuites = APaho_Manager_Sync::FStringToUTF8(*In_Options.CipherSuites);
		}

		return true;
	}

	else
	{
		return false;
	}
}

#pragma endregion Internals

#pragma region Callbacks

void APaho_Manager_Sync::MessageDelivered(void* CallbackContext, MQTTClient_deliveryToken In_DeliveryToken)
{
	AsyncTask(ENamedThreads::GameThread, [CallbackContext, In_DeliveryToken]()
	{
		APaho_Manager_Sync* Owner = Cast<APaho_Manager_Sync>((APaho_Manager_Sync*)CallbackContext);
		
		if (!Owner)
		{
			return;
		}

		Owner->Delegate_Message_Delivered.Broadcast(In_DeliveryToken);
	});
}

int APaho_Manager_Sync::MessageArrived(void* CallbackContext, char* TopicName, int TopicLenght, MQTTClient_message* Message)
{
	FJsonObjectWrapper Arrived;
	Arrived.JsonObject->SetStringField("TopicName", APaho_Manager_Sync::FStringToUTF8(TopicName));
	Arrived.JsonObject->SetNumberField("TopicLength", TopicLenght);
	Arrived.JsonObject->SetStringField("Message", APaho_Manager_Sync::UTF8ToFString((char*)Message->payload));

	MQTTClient_freeMessage(&Message);
	MQTTClient_free(TopicName);

	AsyncTask(ENamedThreads::GameThread, [CallbackContext, Arrived]()
	{
		APaho_Manager_Sync* Owner = Cast<APaho_Manager_Sync>((APaho_Manager_Sync*)CallbackContext);

		if (!Owner)
		{
			return;
		}

		Owner->Delegate_Message_Arrived.Broadcast(Arrived);
	});

	return 1;
}

void APaho_Manager_Sync::ConnectionLost(void* CallbackContext, char* Cause)
{
	AsyncTask(ENamedThreads::GameThread, [CallbackContext, Cause]()
	{
		APaho_Manager_Sync* Owner = Cast<APaho_Manager_Sync>((APaho_Manager_Sync*)CallbackContext);

		if (!Owner)
		{
			return;
		}

		Owner->Delegate_Connection_Lost.Broadcast(APaho_Manager_Sync::UTF8ToFString(Cause));
	});
}

#pragma endregion Callbacks