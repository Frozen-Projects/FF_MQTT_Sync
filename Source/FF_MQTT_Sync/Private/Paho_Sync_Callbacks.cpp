#include "Paho_Sync_Manager.h"

#pragma region Internals

bool APaho_Manager_Sync::SetSSLParams(FString In_Protocol, FPahoSslOptions In_Options)
{
	if (In_Protocol == "wss" || In_Protocol == "mqtts" || In_Protocol == "ssl" || In_Protocol == "WSS" || In_Protocol == "MQTTS" || In_Protocol == "SSL")
	{
		this->SSL_Options = MQTTClient_SSLOptions_initializer;
		this->SSL_Options.enableServerCertAuth = 0;
		this->SSL_Options.verify = 1;

		if (!In_Options.CAPath.IsEmpty() && FPaths::FileExists(In_Options.CAPath))
		{
			this->SSL_Options.CApath = (const char*)StringCast<UTF8CHAR>(*In_Options.CAPath).Get();
		}

		if (!In_Options.Path_KeyStore.IsEmpty() && FPaths::FileExists(In_Options.Path_KeyStore))
		{
			this->SSL_Options.keyStore = (const char*)StringCast<UTF8CHAR>(*In_Options.Path_KeyStore).Get();
		}

		if (!In_Options.Path_TrustStore.IsEmpty() && FPaths::FileExists(In_Options.Path_TrustStore))
		{
			this->SSL_Options.trustStore = (const char*)StringCast<UTF8CHAR>(*In_Options.Path_TrustStore).Get();
		}

		if (!In_Options.Path_PrivateKey.IsEmpty() && FPaths::FileExists(In_Options.Path_PrivateKey))
		{
			this->SSL_Options.privateKey = (const char*)StringCast<UTF8CHAR>(*In_Options.Path_PrivateKey).Get();
		}

		if (!In_Options.PrivateKeyPass.IsEmpty())
		{
			this->SSL_Options.privateKeyPassword = (const char*)StringCast<UTF8CHAR>(*In_Options.PrivateKeyPass).Get();
		}

		if (!In_Options.CipherSuites.IsEmpty())
		{
			this->SSL_Options.enabledCipherSuites = (const char*)StringCast<UTF8CHAR>(*In_Options.CipherSuites).Get();
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
	const FString TopicNameStr = StringCast<UTF8CHAR>(TopicName).Get();
	const FString PayloadStr = StringCast<UTF8CHAR>((const char*)Message->payload).Get();

	FJsonObjectWrapper Arrived;
	Arrived.JsonObject->SetStringField("TopicName", TopicNameStr);
	Arrived.JsonObject->SetNumberField("TopicLength", TopicLenght);
	Arrived.JsonObject->SetStringField("Message", PayloadStr);

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
	const FString CauseStr = StringCast<UTF8CHAR>(Cause).Get();

	AsyncTask(ENamedThreads::GameThread, [CallbackContext, CauseStr]()
	{
		APaho_Manager_Sync* Owner = Cast<APaho_Manager_Sync>((APaho_Manager_Sync*)CallbackContext);

		if (!Owner)
		{
			return;
		}

		Owner->Delegate_Connection_Lost.Broadcast(CauseStr);
	});
}

#pragma endregion Callbacks