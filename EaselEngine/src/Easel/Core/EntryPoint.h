#pragma once

#include "Easel/Core/Log.h"

#include <iostream>

int main(int argc, char** argv) {

	Easel::Debug::Log::OnInit();
	EASEL_CORE_INFO("Welcome To Easel.");
	std::cin.get();
}
