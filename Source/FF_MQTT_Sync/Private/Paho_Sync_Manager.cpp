//Fill out your copyright notice in the Description page of Project Settings.

#include "Paho_Sync_Manager.h"

// Sets default values.
APaho_Manager_Sync::APaho_Manager_Sync()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned.
void APaho_Manager_Sync::BeginPlay()
{
	Super::BeginPlay();
}

// Called when the game end or when destroyed.
void APaho_Manager_Sync::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (this->Client)
	{
		this->MQTT_Sync_Destroy();
	}

	Super::EndPlay(EndPlayReason);
}

// Called every frame.
void APaho_Manager_Sync::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FPahoClientParams APaho_Manager_Sync::GetClientParams()
{
	return this->Client_Params;
}

bool APaho_Manager_Sync::Init_Internal(FJsonObjectWrapper& Out_Code, FPahoClientParams In_Params)
{
	Out_Code.JsonObject->SetStringField("PluginName", "FF_MQTT_Sync");
	Out_Code.JsonObject->SetStringField("FunctionName", TEXT(__FUNCTION__));
	TArray<TSharedPtr<FJsonValue>> Details;

	if (this->Client)
	{
		Details.Add(MakeShared<FJsonValueString>("Client already initialized !"));
		Out_Code.JsonObject->SetArrayField("Details", Details);
		return false;
	}

	FString ParameterReason;
	if (!In_Params.IsParamsValid(ParameterReason))
	{
		Details.Add(MakeShared<FJsonValueString>(ParameterReason));
		Out_Code.JsonObject->SetArrayField("Details", Details);
		return false;
	}

	FString Protocol = In_Params.GetProtocol();
	
	MQTTClient TempClient = nullptr;

	MQTTClient_createOptions Create_Options;

	switch (In_Params.Version)
	{
		case EMQTTVERSION::V3_1:
			
			Create_Options = MQTTClient_createOptions_initializer;
			Create_Options.MQTTVersion = MQTTVERSION_3_1;

			this->Connection_Options.cleansession = 1;

			if (Protocol == "wss" || Protocol == "ws")
			{
				this->Connection_Options = MQTTClient_connectOptions_initializer_ws;
			}

			else
			{
				this->Connection_Options = MQTTClient_connectOptions_initializer;
			}

			break;

		case EMQTTVERSION::V3_1_1:
			
			Create_Options = MQTTClient_createOptions_initializer;
			Create_Options.MQTTVersion = MQTTVERSION_3_1_1;
			
			this->Connection_Options.cleansession = 1;
			
			if (Protocol == "wss" || Protocol == "ws")
			{
				this->Connection_Options = MQTTClient_connectOptions_initializer_ws;
			}

			else
			{
				this->Connection_Options = MQTTClient_connectOptions_initializer;
			}

			break;

		case EMQTTVERSION::V_5:

			Create_Options = MQTTClient_createOptions_initializer;
			Create_Options.MQTTVersion = MQTTVERSION_5;

			this->Connection_Options.cleanstart = 1;

			if (Protocol == "wss" || Protocol == "ws")
			{
				this->Connection_Options = MQTTClient_connectOptions_initializer5_ws;
			}

			else
			{
				this->Connection_Options = MQTTClient_connectOptions_initializer5;
			}

			break;

		default:
			
			Create_Options = MQTTClient_createOptions_initializer;
			Create_Options.MQTTVersion = MQTTVERSION_3_1_1;
			
			this->Connection_Options.cleansession = 1;
			
			if (Protocol == "wss" || Protocol == "ws")
			{
				this->Connection_Options = MQTTClient_connectOptions_initializer5_ws;
			}
			
			else
			{
				this->Connection_Options = MQTTClient_connectOptions_initializer5;
			}
			
			break;
	}

	int RetVal = MQTTClient_createWithOptions(&TempClient, (const char*)StringCast<UTF8CHAR>(*In_Params.Address).Get(), (const char*)StringCast<UTF8CHAR>(*In_Params.ClientId).Get(), MQTTCLIENT_PERSISTENCE_NONE, NULL, &Create_Options);

	if (RetVal != MQTTCLIENT_SUCCESS)
	{
		MQTTClient_destroy(&TempClient);

		Details.Add(MakeShared<FJsonValueString>(FString::Printf(TEXT("MQTTClient_createWithOptions failed with error code %d"), RetVal)));
		Out_Code.JsonObject->SetArrayField("Details", Details);
		return false;
	}

	this->Connection_Options.keepAliveInterval = In_Params.KeepAliveInterval;
	this->Connection_Options.username = (const char*)StringCast<UTF8CHAR>(*In_Params.UserName).Get();
	this->Connection_Options.password = (const char*)StringCast<UTF8CHAR>(*In_Params.Password).Get();
	this->Connection_Options.MQTTVersion = (int32)In_Params.Version;

	if (this->SetSSLParams(Protocol, In_Params.SSL_Options))
	{
		this->Connection_Options.ssl = &this->SSL_Options;
		Details.Add(MakeShared<FJsonValueString>("SSL parameters set."));
	}

	else
	{
		Details.Add(MakeShared<FJsonValueString>("SSL parameters couldn't be set."));
	}

	RetVal = MQTTClient_setCallbacks(TempClient, this, APaho_Manager_Sync::ConnectionLost, APaho_Manager_Sync::MessageArrived, APaho_Manager_Sync::MessageDelivered);

	if (RetVal != MQTTCLIENT_SUCCESS)
	{
		MQTTClient_destroy(&TempClient);

		Details.Add(MakeShared<FJsonValueString>(FString::Printf(TEXT("MQTTClient_setCallbacks failed with error code %d"), RetVal)));
		Out_Code.JsonObject->SetArrayField("Details", Details);
		return false;
	}

	if (In_Params.Version == EMQTTVERSION::V_5)
	{
		MQTTProperties PropertiesConnection = MQTTProperties_initializer;
		MQTTProperties PropertiesWill = MQTTProperties_initializer;
		const MQTTResponse Response = MQTTClient_connect5(TempClient, &this->Connection_Options, &PropertiesConnection, &PropertiesWill);
		RetVal = Response.reasonCode;
	}

	else
	{
		RetVal = MQTTClient_connect(TempClient, &this->Connection_Options);
	}

	if (RetVal != MQTTCLIENT_SUCCESS)
	{
		MQTTClient_destroy(&TempClient);

		Details.Add(MakeShared<FJsonValueString>(FString::Printf(TEXT("MQTTClient_connect failed with error code %d"), RetVal)));
		Out_Code.JsonObject->SetArrayField("Details", Details);
		return false;
	}

	this->Client = TempClient;
	this->Client_Params = In_Params;

	Details.Add(MakeShared<FJsonValueString>("Client initialized successfully."));
	Out_Code.JsonObject->SetArrayField("Details", Details);
	return true;
}

void APaho_Manager_Sync::MQTT_Sync_Destroy()
{
	if (!this->Client)
	{
		return;
	}

	if (MQTTClient_isConnected(this->Client))
	{
		if (this->Connection_Options.MQTTVersion == MQTTVERSION_5)
		{
			MQTTClient_disconnect5(this->Client, 10000, MQTTREASONCODE_NORMAL_DISCONNECTION, NULL);
		}

		else
		{
			MQTTClient_disconnect(this->Client, 10000);
		}
	}

	MQTTClient_destroy(&this->Client);
}

void APaho_Manager_Sync::MQTT_Sync_Init(FDelegate_Paho_Connection DelegateConnection, FPahoClientParams In_Params)
{
	AsyncTask(ENamedThreads::AnyNormalThreadNormalTask, [this, DelegateConnection, In_Params]()
		{
			FJsonObjectWrapper Out_Code;
			const bool InitResult = this->Init_Internal(Out_Code, In_Params);

			AsyncTask(ENamedThreads::GameThread, [DelegateConnection, InitResult, Out_Code]()
				{
					DelegateConnection.ExecuteIfBound(InitResult, Out_Code);
				}
			);
		}
	);
}

bool APaho_Manager_Sync::MQTT_Sync_Publish(FJsonObjectWrapper& Out_Code, FString In_Topic, FString In_Payload, EMQTTQOS In_QoS, bool bIsRetained)
{
	Out_Code.JsonObject->SetStringField("PluginName", "FF_MQTT_Sync");
	Out_Code.JsonObject->SetStringField("FunctionName", TEXT(__FUNCTION__));
	TArray<TSharedPtr<FJsonValue>> Details;

	if (!this->Client)
	{
		Details.Add(MakeShared<FJsonValueString>("Client is not valid !"));
		Out_Code.JsonObject->SetArrayField("Details", Details);
		return false;
	}

	if (!MQTTClient_isConnected(this->Client))
	{
		Details.Add(MakeShared<FJsonValueString>("Client is not connected !"));
		Out_Code.JsonObject->SetArrayField("Details", Details);
		return false;
	}

	int RetVal = -1; 
	MQTTClient_deliveryToken DeliveryToken;

	if (this->Connection_Options.MQTTVersion == MQTTVERSION_5)
	{
		MQTTProperties Properties_Publish = MQTTProperties_initializer;
		const MQTTResponse Response = MQTTClient_publish5(this->Client, (const char*)StringCast<UTF8CHAR>(*In_Topic).Get(), In_Payload.Len(), (const char*)StringCast<UTF8CHAR>(*In_Payload).Get(), (int32)In_QoS, bIsRetained ? 1 : 0, &Properties_Publish, &DeliveryToken);
		RetVal = Response.reasonCode;
	}

	else
	{
		RetVal = MQTTClient_publish(this->Client, (const char*)StringCast<UTF8CHAR>(*In_Topic).Get(), In_Payload.Len(), (const char*)StringCast<UTF8CHAR>(*In_Payload).Get(), (int32)In_QoS, bIsRetained ? 1 : 0, &DeliveryToken);
	}

	const FString ResultString = RetVal == MQTTCLIENT_SUCCESS ? "Payload successfully published." : FString::Printf(TEXT("There was a problem while publishing payload with these configurations : %d"), RetVal);
	Details.Add(MakeShared<FJsonValueString>(ResultString));
	Out_Code.JsonObject->SetArrayField("Details", Details);
	
	return RetVal == MQTTCLIENT_SUCCESS ? true : false;
}

bool APaho_Manager_Sync::MQTT_Sync_Subscribe(FJsonObjectWrapper& Out_Code, FString In_Topic, EMQTTQOS In_QoS)
{
	Out_Code.JsonObject->SetStringField("PluginName", "FF_MQTT_Sync");
	Out_Code.JsonObject->SetStringField("FunctionName", TEXT(__FUNCTION__));
	TArray<TSharedPtr<FJsonValue>> Details;

	if (!this->Client)
	{
		Details.Add(MakeShared<FJsonValueString>("Client is not valid !"));
		Out_Code.JsonObject->SetArrayField("Details", Details);
		return false;
	}

	if (!MQTTClient_isConnected(this->Client))
	{
		Details.Add(MakeShared<FJsonValueString>("Client is not connected !"));
		Out_Code.JsonObject->SetArrayField("Details", Details);
		return false;
	}

	int RetVal = -1;
	
	if (this->Connection_Options.MQTTVersion == MQTTVERSION_5)
	{
		const MQTTResponse Response = MQTTClient_subscribe5(this->Client, (const char*)StringCast<UTF8CHAR>(*In_Topic).Get(), (int32)In_QoS, NULL, NULL);
		RetVal = Response.reasonCode;
	}

	else
	{
		RetVal = MQTTClient_subscribe(this->Client, (const char*)StringCast<UTF8CHAR>(*In_Topic).Get(), (int32)In_QoS);
	}

	const FString ResultString = RetVal == MQTTCLIENT_SUCCESS ? "Topic successfully subscribed." : FString::Printf(TEXT("There was a problem while subscribing topic with these configurations. : %d"), RetVal);
	Details.Add(MakeShared<FJsonValueString>(ResultString));
	Out_Code.JsonObject->SetArrayField("Details", Details);
	
	return RetVal == MQTTCLIENT_SUCCESS ? true : false;
}

bool APaho_Manager_Sync::MQTT_Sync_Unsubscribe(FJsonObjectWrapper& Out_Code, FString In_Topic)
{
	Out_Code.JsonObject->SetStringField("PluginName", "FF_MQTT_Sync");
	Out_Code.JsonObject->SetStringField("FunctionName", TEXT(__FUNCTION__));
	TArray<TSharedPtr<FJsonValue>> Details;

	if (!this->Client)
	{
		Details.Add(MakeShared<FJsonValueString>("Client is not valid !"));
		Out_Code.JsonObject->SetArrayField("Details", Details);
		return false;
	}

	if (!MQTTClient_isConnected(this->Client))
	{
		Details.Add(MakeShared<FJsonValueString>("Client is not connected !"));
		Out_Code.JsonObject->SetArrayField("Details", Details);
		return false;
	}

	const int RetVal = MQTTClient_unsubscribe(this->Client, (const char*)StringCast<UTF8CHAR>(*In_Topic).Get());

	const FString ResultString = RetVal == MQTTCLIENT_SUCCESS ? "Topic successfully unsubscribed." : FString::Printf(TEXT("There was a problem while unsubscribing topic with these configurations. : %d"), RetVal);
	Details.Add(MakeShared<FJsonValueString>(ResultString));
	Out_Code.JsonObject->SetArrayField("Details", Details);

	return RetVal == MQTTCLIENT_SUCCESS ? true : false;
}