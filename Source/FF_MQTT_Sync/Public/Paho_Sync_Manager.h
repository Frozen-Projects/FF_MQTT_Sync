// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Custom Includes.
#include "Generic_Includes.h"
#include "Paho_Sync_Includes.h"

#include "Paho_Sync_Manager.generated.h"

UCLASS()
class FF_MQTT_SYNC_API APaho_Manager_Sync : public AActor
{
	GENERATED_BODY()
	
private:

	MQTTClient Client = nullptr;
	MQTTClient_connectOptions Connection_Options;
	MQTTClient_SSLOptions SSL_Options;
	
	UPROPERTY()
	FPahoClientParams Client_Params;

#pragma region Internals

	virtual bool SetSSLParams(FString In_Protocol, FPahoSslOptions In_Options);
	virtual bool Init_Internal(FJsonObjectWrapper& Out_Code, FPahoClientParams In_Params);

#pragma endregion Internals
	
#pragma region Callbacks

	static void MessageDelivered(void* CallbackContext, MQTTClient_deliveryToken In_DeliveryToken);
	static int MessageArrived(void* CallbackContext, char* TopicName, int TopicLenght, MQTTClient_message* Message);
	static void ConnectionLost(void* CallbackContext, char* Cause);

#pragma endregion Callbacks

protected:
	
	// Called when the game starts or when spawned.
	virtual void BeginPlay() override;

	// Called when the game end or when destroyed.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	

	// Sets default values for this actor's properties.
	APaho_Manager_Sync();

	// Called every frame.
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable, Category = "Frozen Forest|MQTT|Client|Paho C")
	FDelegate_Paho_String Delegate_Connection_Lost;

	UPROPERTY(BlueprintAssignable, Category = "Frozen Forest|MQTT|Client|Paho C")
	FDelegate_Paho_Int Delegate_Message_Delivered;

	UPROPERTY(BlueprintAssignable, Category = "Frozen Forest|MQTT|Client|Paho C")
	FDelegate_Paho_Json Delegate_Message_Arrived;

	UFUNCTION(BlueprintPure)
	virtual FPahoClientParams GetClientParams();

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|MQTT|Client|Paho C", meta = (DisplayName = "Paho Sync - Destroy", ToolTip = "Don't use it immediately after \"MQTT Async Init\" give some delay or better use it after \"Delegate OnConnect\"", KeyWords = "mqtt, sync, paho, client, destroy, close, disconnect"))
	virtual void MQTT_Sync_Destroy();

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|MQTT|Client|Paho C", meta = (DisplayName = "Paho Sync - Init", ToolTip = "Don't use it immediately after \"MQTT Async Init\" give some delay or better use it after \"Delegate OnConnect\"", KeyWords = "mqtt, sync, paho, client, init, initialize, start, connect"))
	virtual void MQTT_Sync_Init(FDelegate_Paho_Connection DelegateConnection, FPahoClientParams In_Params);

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|MQTT|Client|Paho C", meta = (DisplayName = "Paho Sync - Publish", ToolTip = "Don't use it immediately after \"MQTT Async Init\" give some delay or better use it after \"Delegate OnConnect\"", KeyWords = "mqtt, sync, paho, client, publish, publisher"))
	virtual bool MQTT_Sync_Publish(FJsonObjectWrapper& Out_Code, FString In_Topic, FString In_Payload, EMQTTQOS In_QoS = EMQTTQOS::QoS_0, bool bIsRetained = false);

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|MQTT|Client|Paho C", meta = (DisplayName = "Paho Sync - Subscribe", ToolTip = "Don't use it immediately after \"MQTT Async Init\" give some delay or better use it after \"Delegate OnConnect\"", KeyWords = "mqtt, sync, paho, client, subscribe, subscriber"))
	virtual bool MQTT_Sync_Subscribe(FJsonObjectWrapper& Out_Code, FString In_Topic, EMQTTQOS In_QoS = EMQTTQOS::QoS_0);

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|MQTT|Client|Paho C", meta = (DisplayName = "Paho Sync - Unsubscribe", ToolTip = "Don't use it immediately after \"MQTT Async Init\" give some delay or better use it after \"Delegate OnConnect\"", KeyWords = "mqtt, sync, paho, client, unsubscribe, subscriber"))
	virtual bool MQTT_Sync_Unsubscribe(FJsonObjectWrapper& Out_Code, FString In_Topic);

};
