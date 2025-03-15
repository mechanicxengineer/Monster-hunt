
#pragma once

#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"

#define draw_sphere(location, radius, sagment) if(GetWorld()) DrawDebugSphere(GetWorld(), location, radius, sagment, FColor::White, true);
#define draw_line_persis_true(start_point, end_point_with_length) if(GetWorld()) DrawDebugLine(GetWorld(), start_point, end_point_with_length, FColor::White, true, -1.0f, 0, 1.0f);
#define draw_line_persis_false(start_point, end_point_with_length) if(GetWorld()) DrawDebugLine(GetWorld(), start_point, end_point_with_length, FColor::White, false, 1.0f);
#define draw_box(location, box_size) if(GetWorld()) DrawDebugBox(GetWorld(), location, box_size, FColor::White, true);
#define draw_point(location) if(GetWorld()) DrawDebugPoint(GetWorld(), location, 20.0f, FColor::White, true);
#define draw_vector(start_point, end_point_with_length) if(GetWorld())                          \
    {                                                                                           \
        DrawDebugLine(GetWorld(), start_point, end_point_with_length, FColor::White, true, -1.0f, 0, 1.0f);     \
        DrawDebugPoint(GetWorld(), end_point_with_length, 20.0f, FColor::White, true);          \
    }

//  Single Frames
#define draw_sphere_singleframe(location, radius, sagment) if(GetWorld()) DrawDebugSphere(GetWorld(), location, radius, sagment, FColor::White, false, -1.0f);
#define draw_line_singleframe(start_point, end_point_with_length) if(GetWorld()) DrawDebugLine(GetWorld(), start_point, end_point_with_length, FColor::White, false, -1.0f, 0, 1.0f);
#define draw_box_singleframe(location, box_size) if(GetWorld()) DrawDebugBox(GetWorld(), location, box_size, FColor::White, false, -1.0f);
#define draw_point_singleframe(location) if(GetWorld()) DrawDebugPoint(GetWorld(), location, 20.0f, FColor::White, false, -1.0f);
#define draw_vector_singleframe(start_point, end_point_with_length) if(GetWorld())              \
    {                                                                                           \
        DrawDebugLine(GetWorld(), start_point, end_point_with_length, FColor::White, false, -1.0f, 0, 1.0f);    \
        DrawDebugPoint(GetWorld(), end_point_with_length, 20.0f, FColor::White, false, -1.0f);         \
    }

//  timed
#define draw_sphere_timed(location, radius, sagment) if(GetWorld()) DrawDebugSphere(GetWorld(), location, radius, sagment, FColor::White, false, 5.0f);

//  messages
#define showargplus(msg, ...) if(GEngine) GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::White, FString::Printf(TEXT(msg), __VA_ARGS__));
#define showargminus(msg, ...) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::Printf(TEXT(msg), __VA_ARGS__));
#define show(msg) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, msg);
