#include <Easel.h>

class Sandbox : public Easel::Application {
public:
	explicit Sandbox()
		: Application(std::string("/Sandbox/"), std::string("Sandbox")) {

	}

	~Sandbox() {

	}
};

Easel::Application* Easel::CreateApplication() {
	return new Sandbox();
}
