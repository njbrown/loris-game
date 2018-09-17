#include "SDL.h"
#include "SDL_image.h"
#include "loris/loris.hpp"
#include "loris/libs/math.hpp"
#include "loris/libs/utils.hpp"

#include <fstream>
#include <sstream>


void PrintError(loris::Error e)
{
	cout << "error: " << e.filename << ":" << e.line << ": " << e.message << endl;
}

string ReadFile(const char *filename)
{
	ifstream file(filename);
	stringstream stream;
	string line;

	while (file.good())
	{
		getline(file, line);
		stream << line << endl;
	}

	return stream.str();
}

loris::Value print(loris::VirtualMachine* vm, loris::Object* self)
{
	for (int i = 0; i<vm->NumArgs(); i++)
		vm->GetArg(i).Print();

	return loris::Value::CreateNull();
}


class ScriptManager
{
	loris::Compiler compiler;
	loris::Assembly* lib;
	loris::VirtualMachine vm;
	loris::Object* game;

	vector<loris::Class*> classes;

public:
	ScriptManager()
	{
	}

	void AddClass(loris::Class* cls)
	{
		classes.push_back(cls);
	}

	void AddSourceFile(string filename)
	{
		compiler.AddSource(filename, ReadFile(filename.c_str()));
	}

	bool CompileScripts()
	{
		if (compiler.Compile())
		{
			return true;
		}
		return false;
	}

	//compiles scripts, creates instance of class derived from Game and stores it 
	//add all classes and sources before this method is called
	void Start()
	{
		//compile scripts
		bool res = CompileScripts();
		if (!res)
		{
			PrintError(compiler.GetError());
			return;
		}

		//add classes to assembly
		lib = compiler.GetAssembly();
		for (auto i : classes)
			lib->AddClass(i);

		//add few native helper functions
		lib->AddFunction("print", print);

		DSMathLib::Install(lib);
		DSUtilsLib::Install(lib);

		vm.SetAssembly(lib);

		loris::Class* gameClass = lib->GetClass("Game");
		if (!gameClass)
		{
			cout << "couldnt find a Game class " << endl;
			return;
		}

		game = vm.CreateObject(gameClass);
		if (!game)
		{
			cout << "couldnt not create game instance" << endl;
			return;
		}

		game->managed = false;
	}

	void RunInit()
	{
		if (!game)return;

		vm.ExecuteMemberFunction(game, "init");

		if (vm.HasError())
		{
			PrintError(vm.GetError());
			vm.ClearError();
		}
	}

	void RunUpdate(float dt)
	{
		if (!game)return;

		vm.AddArg(loris::Value::CreateNumber(dt));
		vm.ExecuteMemberFunction(game, "update");

		if (vm.HasError())
		{
			PrintError(vm.GetError());
			vm.ClearError();
		}

	}

	void RunDraw()
	{
		if (!game)return;

		vm.ExecuteMemberFunction(game, "draw");

		if (vm.HasError())
		{
			PrintError(vm.GetError());
			vm.ClearError();
		}
	}

	~ScriptManager()
	{
		vm.DestroyObject(game);
	}
};

int main(int argc, char *argv[])
{

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window = SDL_CreateWindow(
	"Game",
	SDL_WINDOWPOS_UNDEFINED,
	SDL_WINDOWPOS_UNDEFINED,
	640,
	480,
	0
	);

	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF | IMG_INIT_WEBP);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

	bool running = true;
	SDL_Event e;
	while (running) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				running = false;
			}
		}

		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}