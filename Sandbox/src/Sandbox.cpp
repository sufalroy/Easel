#include <Easel.h>
#include <Easel/Core/EntryPoint.h>

class Game : public Easel::Application {
public:
	explicit Game()
		: Application(std::string("/Sandbox/"), std::string("Sandbox")) {

		Application::Get().GetWindow()->SetWindowTitle("Sandbox");
	}

	~Game() {

	}

	void Init() override {

		Application::Init();
	}
};

Easel::Application* Easel::CreateApplication() {
	return new Game();
}
