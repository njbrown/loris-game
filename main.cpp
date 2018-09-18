#include "SDL.h"
#include "SDL_image.h"
#include "loris/loris.hpp"
#include "loris/libs/math.hpp"
#include "loris/libs/utils.hpp"

#include <fstream>
#include <sstream>
#include <unordered_map>

loris::Class* imageClass;
SDL_Surface* screen;
SDL_Renderer *renderer;

struct Image
{
	SDL_Texture* sprite;
	int width;
	int height;
	int refCount;
};

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
			image->width = loadedSurface->w;
			image->height = loadedSurface->h;
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


class Input
{
public:
	static bool* prevKeys;
	static bool* curKeys;

	static bool* prevMouseButtons;
	static bool* curMouseButtons;

	static int prevMouseX;
	static int prevMouseY;
	static int curMouseX;
	static int curMouseY;

	static void Init()
	{
		prevKeys = new bool[SDL_NUM_SCANCODES];
		curKeys = new bool[SDL_NUM_SCANCODES];
		for (int i = 0; i<SDL_NUM_SCANCODES; i++)
		{
			prevKeys[i] = false;
			curKeys[i] = false;
		}

		prevMouseButtons = new bool[5];
		curMouseButtons = new bool[5];
		for (int i = 0; i<5; i++)
		{
			prevMouseButtons[i] = false;
			curMouseButtons[i] = false;
		}

		prevMouseX = 0;
		prevMouseY = 0;
		curMouseX = 0;
		curMouseY = 0;
	}

	static void PreUpdate()
	{
		for (int i = 0; i<SDL_NUM_SCANCODES; i++)
		{
			prevKeys[i] = curKeys[i];
			//curKeys[i]=false;
		}

		for (int i = 0; i<5; i++)
		{
			prevMouseButtons[i] = curMouseButtons[i];
			//curKeys[i]=false;
		}

		prevMouseX = curMouseX;
		prevMouseY = curMouseY;
		SDL_GetMouseState(&curMouseX, &curMouseY);
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

	static bool IsMouseButtonDown(int index)
	{
		return curMouseButtons[index];
	}

	static bool IsMouseButtonUp(int index)
	{
		return !curMouseButtons[index];
	}

	static bool IsMouseButtonPressed(int index)
	{
		return !prevMouseButtons[index] && curMouseButtons[index];
	}

	static bool IsMouseButtonReleased(int index)
	{
		return prevMouseButtons[index] && !curMouseButtons[index];
	}
};

bool* Input::prevKeys = 0;
bool* Input::curKeys = 0;

bool* Input::prevMouseButtons = 0;
bool* Input::curMouseButtons = 0;

int Input::prevMouseX = 0;
int Input::prevMouseY = 0;
int Input::curMouseX = 0;
int Input::curMouseY = 0;

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
	rect.w = img->width;
	rect.h = img->height;

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


bool Input_IsMouseButtonDown(int key)
{
	return Input::IsMouseButtonDown(key);
}

bool Input_IsMouseButtonUp(int key)
{
	return Input::IsMouseButtonUp(key);
}

bool Input_IsMouseButtonPressed(int key)
{
	return Input::IsMouseButtonPressed(key);
}

bool Input_IsMouseButtonReleased(int key)
{
	return Input::IsMouseButtonReleased(key);
}

int Input_GetMouseX(int key)
{
	return Input::curMouseX;
}

int Input_GetMouseY(int key)
{
	return Input::curMouseY;
}

// autobound functions cannot return void for now
int Input_ShowCursor()
{
	SDL_ShowCursor(true);
	return 0;
}

int Input_HideCursor()
{
	SDL_ShowCursor(false);
	return 0;
}

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
		if (compiler.Compile(true))// true gives line numbers in error outputs
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
		.StaticMethod("isMouseDown", loris::Def(Input_IsMouseButtonDown))/* bool isMouseDown(index) */
		.StaticMethod("isMouseUp", loris::Def(Input_IsMouseButtonUp))/* bool isMouseUp(index) */
		.StaticMethod("isMousePressed", loris::Def(Input_IsMouseButtonPressed))/* bool isMousePressed(index) */
		.StaticMethod("isMouseReleased", loris::Def(Input_IsMouseButtonReleased))/* bool isMouseReleased(index) */
		.StaticMethod("getMouseX", loris::Def(Input_GetMouseX))/* number getMouseX() */
		.StaticMethod("getMouseY", loris::Def(Input_GetMouseY))/* number getMouseY() */
		.StaticMethod("showCursor", loris::Def(Input_ShowCursor))/* void showCursor() */
		.StaticMethod("hideCursor", loris::Def(Input_HideCursor))/* void hideCursor() */
		.Build();


	//global
	imageClass = loris::CreateClass("Image")
		.Attrib("width")
		.Attrib("height")
		.Destructor(Image_Destruct)//release resources
		.Build();

	//add scripts
	scriptMan.AddSourceFile("assets/game.ls");

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
	SDL_SetRenderDrawColor(renderer,255, 255, 255, SDL_ALPHA_OPAQUE);

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
				//std::cout << e.key.keysym.scancode;
				Input::curKeys[e.key.keysym.scancode] = true;
				break;
			case SDL_KEYUP:
				Input::curKeys[e.key.keysym.scancode] = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				Input::curMouseButtons[e.button.button] = true;
				break;
			case SDL_MOUSEBUTTONUP:
				Input::curMouseButtons[e.button.button] = false;
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