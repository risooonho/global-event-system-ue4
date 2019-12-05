// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GESHandler.h"
#include "GlobalEventSystemBPLibrary.generated.h"

/* 
* Core Global Event System functions. Call anywhere.
*/
UCLASS()
class UGlobalEventSystemBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** 
	* Remove this listener from the specified GESEvent.
	*/
	UFUNCTION(BlueprintCallable, meta = (Keywords = "ges sever stoplisten", WorldContext = "WorldContextObject"), Category = "GlobalEventSystem")
	static void GESUnbindEvent(UObject* WorldContextObject, const FString& Domain = TEXT("global.default"), const FString& Event = TEXT(""), const FString& ReceivingFunction = TEXT(""));

	/**
	* Bind a function (to current caller) to GES event. Make sure to match your receiving function parameters to the GESEvent ones.
	*/
	UFUNCTION(BlueprintCallable, meta = (Keywords = "ges create listen", WorldContext = "WorldContextObject"), Category = "GlobalEventSystem")
	static void GESBindEvent(UObject* WorldContextObject, const FString& Domain = TEXT("global.default"), const FString& Event = TEXT(""), const FString& ReceivingFunction = TEXT(""));

	/**
	* Bind an event delegate to GES event. Use blueprint utility to decode UProperty.
	*/
	UFUNCTION(BlueprintCallable, meta = (Keywords = "ges create listen", WorldContext = "WorldContextObject"), Category = "GlobalEventSystem")
	static void GESBindEventToWildcardDelegate(UObject* WorldContextObject, const FGESOnePropertySignature& ReceivingFunction, const FString& Domain = TEXT("global.default"), const FString& Event = TEXT(""));

	/** 
	* Emit desired event with data. Data can be any property (just not UObjects at this time). 
	* Pinning an event means it will emit to future listeners even if the event has already been
	* emitted.
	*/
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "GlobalEventSystem", meta = (CustomStructureParam = "ParameterData", WorldContext = "WorldContextObject"))
	static void GESEmitEventOneParam(UObject* WorldContextObject, bool bPinned = false, const FString& Domain = TEXT("global.default"), const FString& Event = TEXT(""), UProperty* ParameterData = nullptr);

	/** 
	* Just emits the event with no additional data
	*/
	UFUNCTION(BlueprintCallable, meta=(WorldContext = "WorldContextObject"), Category = "GlobalEventSystem")
	static void GESEmitEvent(UObject* WorldContextObject, bool bPinned = false, const FString& Domain = TEXT("global.default"), const FString& Event = TEXT(""));

	/** 
	* If an event was pinned, this will unpin it. If you wish to re-pin a different event you need to unpin the old event first.
	*/
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "GlobalEventSystem")
	static void GESUnpinEvent(UObject* WorldContextObject, const FString& Domain = TEXT("global.default"), const FString& Event = TEXT(""));

	/**
	* GES Options are global and affect things like logging and param verification (performance options)
	*/
	UFUNCTION(BlueprintCallable, Category = "GlobalEventSystemOptions")
	static void SetGESOptions(const FGESGlobalOptions& InOptions);

	//Wildcard conversions, used in wildcard event delegates

	/** Convert wildcard property into a literal int */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To Integer (Wildcard Property)", BlueprintAutocast), Category = "Utilities|SocketIO")
	static bool Conv_PropToInt(const FGESWildcardProperty& InProp, int32& OutInt);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "To Float (Wildcard Property)", BlueprintAutocast), Category = "Utilities|SocketIO")
	static bool Conv_PropToFloat(const FGESWildcardProperty& InProp, float& OutFloat);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "To Bool (Wildcard Property)", BlueprintAutocast), Category = "Utilities|SocketIO")
	static bool Conv_PropToBool(const FGESWildcardProperty& InProp, bool& OutBool);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "To String (Wildcard Property)", BlueprintAutocast), Category = "Utilities|SocketIO")
	static bool Conv_PropToString(const FGESWildcardProperty& InProp, FString& OutString);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "To Name (Wildcard Property)", BlueprintAutocast), Category = "Utilities|SocketIO")
	static bool Conv_PropToName(const FGESWildcardProperty& InProp, FName& OutName);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "To Struct (Wildcard Property)", CustomStructureParam = "OutStruct", BlueprintAutocast), Category = "Utilities|SocketIO")
	static bool Conv_PropToStruct(const FGESWildcardProperty& InProp, UProperty*& OutStruct);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "To Object (Wildcard Property)", BlueprintAutocast), Category = "Utilities|SocketIO")
	static bool Conv_PropToObject(const FGESWildcardProperty& InProp, UObject*& OutObject);

	//Convert property into c++ accessible form
	DECLARE_FUNCTION(execGESEmitEventOneParam)
	{
		Stack.MostRecentProperty = nullptr;
		FGESEmitData EmitData;

		Stack.StepCompiledIn<UObjectProperty>(&EmitData.WorldContext);
		Stack.StepCompiledIn<UBoolProperty>(&EmitData.bPinned);
		Stack.StepCompiledIn<UStrProperty>(&EmitData.Domain);
		Stack.StepCompiledIn<UStrProperty>(&EmitData.Event);

		//Determine wildcard property
		Stack.Step(Stack.Object, NULL);
		UProperty* ParameterProp = Cast<UProperty>(Stack.MostRecentProperty);
		void* PropPtr = Stack.MostRecentPropertyAddress;

		EmitData.Property = ParameterProp;
		EmitData.PropertyPtr = PropPtr;

		P_FINISH;
		P_NATIVE_BEGIN;
		HandleEmit(EmitData);
		P_NATIVE_END;
	}


private:
	static void HandleEmit(const FGESEmitData& EmitData);
	//todo add support for array type props
};
