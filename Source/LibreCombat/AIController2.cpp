// Fill out your copyright notice in the Description page of Project Settings.


#include "AIController2.h"

AAIController2::AAIController2() {
	GetPathFollowingComponent()->SetBlockDetection(10.f, 0.5f, 2);
}
