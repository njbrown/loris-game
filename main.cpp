#include "SDL.h"
#include "SDL_image.h"
#include "loris/loris.hpp"
#include "loris/libs/math.hpp"
#include "loris/libs/utils.hpp"

#include <fstream>
#include <sstream>

loris::Class* imageClass;
SDL_Surface* screen;
SDL_Renderer *renderer;

class Input
{
public:
	static bool* prevKeys;
	static bool* curKeys;

	static void Init()
	{
		prevKeys = new bool[323];
		curKeys = new bool[323];
		for (int i = 0; i<323; i++)
		{
			prevKeys[i] = false;
			curKeys[i] = false;
		}
	}

	static void PreUpdate()
	{
		for (int i = 0; i<323; i++)
		{
			prevKeys[i] = curKeys[i];
			//curKeys[i]=false;
		}
	}

	static bool IsKeyDown(int index)
	{
		return curKeys[index];
	}

	static bool IsKeyUp(int index)
	{
		return !curKeys[index];
	}

	static bool IsKeyPressed(int index)
	{
		return !prevKeys[index] && curKeys[index];
	}

	static bool IsKeyReleased(int index)
	{
		return prevKeys[index] && !curKeys[index];
	}
};

bool* Input::prevKeys = 0;
bool* Input::curKeys = 0;


struct Image
{
	SDL_Texture* sprite;
	int refCount;
};

#define EXPECT_STRING(vm,index) if(vm->GetArg(index).type!=ValueType::String){vm->RaiseError("expected a string argument");return Value::CreateNull();}
#define EXPECT_OBJECT(vm,index) if(vm->GetArg(index).type!=ValueType::Object){vm->RaiseError("expected an object");return Value::CreateNull();}
#define EXPECT_NUMBER(vm,index) if(vm->GetArg(index).type!=ValueType::Number){vm->RaiseError("expected a number");return Value::CreateNull();}

Value Assets_LoadImage(VirtualMachine* vm, Object* self)
{
	//first arg should be string
	EXPECT_STRING(vm, 0);


	Value val = vm->GetArg(0);
	const char* filename = val.AsString();

	//load actual asset
	Image* img = Assets::LoadImage(filename);

	//create image Object and assign img to data
	Object* imgObject = vm->CreateObject(imageClass);
	imgObject->data = img;

	//return obj
	return Value::CreateObject(imgObject);
}

Value Image_Destruct(VirtualMachine* vm, Object* self)
{
	Image* img = (Image*)self->data;

	//free surface
	SDL_DestroyTexture(img->sprite);

	//delete!
	delete img;

	return Value::CreateNull();
}

Value Canvas_DrawImage(VirtualMachine* vm, Object* self)
{
	//get object
	EXPECT_OBJECT(vm, 0);
	EXPECT_NUMBER(vm, 1);
	EXPECT_NUMBER(vm, 2);

	Object* obj = vm->GetArg(0).AsObject();
	if (obj->typeName != "Image")
	{
		vm->RaiseError("you can only render an image");
		return Value::CreateNull();
	}
	//todo: validate obj->data is not null

	int x = (int)vm->GetArg(1).AsNumber();
	int y = (int)vm->GetArg(2).AsNumber();

	//blit!
	Image* img = (Image*)obj->data;
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	//rect.w = img->sprite->w;
	//rect.h = img->sprite->h;

	//SDL_BlitSurface(img->sprite, NULL, screen, &rect);
	SDL_RenderCopy(renderer, img->sprite, nullptr, &rect);

	return Value::CreateNull();
}

Value Canvas_DrawTestRect(VirtualMachine* vm, Object* self)
{
	//get object
	EXPECT_OBJECT(vm, 0);

	Object* obj = vm->GetArg(0).AsObject();
	;
	SDL_Rect rect;
	rect.x = obj->GetAttrib("x").AsNumber();
	rect.y = obj->GetAttrib("y").AsNumber();
	rect.w = obj->GetAttrib("width").AsNumber();
	rect.h = obj->GetAttrib("height").AsNumber();
	SDL_FillRect(screen, &rect, 0xFF0000);

	return Value::CreateNull();
}

Value Input_IsKeyDown(VirtualMachine* vm, Object* self)
{
	EXPECT_NUMBER(vm, 0);

	int index = (int)vm->GetArg(0).AsNumber();

	//todo: check if index is within the bounds

	bool res = Input::IsKeyDown(index);

	return Value::CreateBool(res);
}

Value Input_IsKeyUp(VirtualMachine* vm, Object* self)
{
	EXPECT_NUMBER(vm, 0);

	int index = (int)vm->GetArg(0).AsNumber();

	//todo: check if index is within the bounds	
	bool res = Input::IsKeyUp(index);

	return Value::CreateBool(res);
}

class Assets
{
	unordered_map<string, Image*> imageMap;
public:
	static Image* LoadImage(string imgName)
	{
		Image* image = new Image;

		//The final texture
		SDL_Texture* newTexture = NULL;

		//Load image at specified path
		SDL_Surface* loadedSurface = IMG_Load(imgName.c_str());
		if (loadedSurface == NULL)
		{
			printf("Unable to load image %s! SDL_image Error: %s\n", imgName.c_str(), IMG_GetError());
		}
		else
		{
			//Create texture from surface pixels
			newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
			if (newTexture == NULL)
			{
				printf("Unable to create texture from %s! SDL Error: %s\n", imgName.c_str(), SDL_GetError());
			}

			//Get rid of old loaded surface
			SDL_FreeSurface(loadedSurface);
		}

		image->sprite = newTexture;
		assert(image->sprite != NULL);

		return image;
	}
};


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
	//script man
	ScriptManager scriptMan;

	//create classes
	ClassBuilder* builder = new ClassBuilder;

	Class* gfxClass = loris::CreateClass("Canvas")
		.StaticMethod("drawTestRect", Canvas_DrawTestRect)/* void drawImage(img,x,y); */
		.StaticMethod("drawImage", Canvas_DrawImage)/* void drawImage(img,x,y); */
		.Build();

	Class* assetsClass = loris::CreateClass("Assets")
		.StaticMethod("loadImage", Assets_LoadImage)/* Image loadImage(filename) */
		.Build();

	Class* inputClass = loris::CreateClass("Input")
		.StaticMethod("isKeyDown", Input_IsKeyDown)/* bool isKeyDown(index) */
		.StaticMethod("isKeyUp", Input_IsKeyUp)/* bool isKeyUp(index) */
		.Build();

	//global
	imageClass = loris::CreateClass("Image")
		.Attrib("width")
		.Attrib("height")
		.Destructor(Image_Destruct)//release resources
		.Build();

	//add scripts
	scriptMan.AddSourceFile("scripts/game.ds");

	//add classes
	scriptMan.AddClass(assetsClass);
	scriptMan.AddClass(gfxClass);
	scriptMan.AddClass(imageClass);
	scriptMan.AddClass(inputClass);

	//add classes and sources before this function is called
	scriptMan.Start();

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

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

	Input::Init();
	scriptMan.RunInit();

	int lastTime = SDL_GetTicks();
	bool running = true;
	SDL_Event e;
	while (running) {

		Input::PreUpdate();
		while (SDL_PollEvent(&e) != 0) {
			switch (e.type)
			{
			case SDL_QUIT:
				running = false;
				return 0;
				break;
			case SDL_KEYDOWN:
				Input::curKeys[e.key.keysym.sym] = true;
				break;
			case SDL_KEYUP:
				Input::curKeys[e.key.keysym.sym] = false;
				break;
			default:
				break;
			}
		}

		int curTime = SDL_GetTicks();
		float dt = (curTime - lastTime) / 1000.0f;
		lastTime = curTime;

		scriptMan.RunUpdate(dt);

		SDL_RenderClear(renderer);
		scriptMan.RunDraw();
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}