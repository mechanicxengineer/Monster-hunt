

#include "AdvLog.h"

DECLARE_LOG_CATEGORY_EXTERN(ShowDebug, Log, All);
DEFINE_LOG_CATEGORY(ShowDebug)

void AdvLogger::Output(int32 key, float timeToDisplay, const FString& msg) {
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(key, timeToDisplay, FColor::Green, msg);
	}
	UE_LOG(ShowDebug, Warning, TEXT("%s"), *msg);
}
