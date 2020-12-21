#include <Easel.h>

class Sandbox : public Easel::Application {
public:
	Sandbox() {

	}

	~Sandbox() {

	}
};

Easel::Application* Easel::CreateApplication() {
	return new Sandbox();
}
