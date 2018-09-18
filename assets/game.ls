// use scancodes instead of keycodes
class Keys
{
	static var Up = 82;
	static var Down = 81;
	static var Right = 79;
	static var Left = 80;

	static var W = 26;
	static var A = 4;
	static var S = 22;
	static var D = 7;
}

class Mouse
{
	static var Left = 1;
	static var Middle = 2;
	static var Right = 3;
}

class Vector2
{
	Vector2(x,y)
	{
		self.x = x;
		self.y = y;
	}

	def copy()
	{
		return new Vector2(self.x,self.y);
	}

	def add(other)
	{
		self.x = self.x + other.x;
		self.y = self.y + other.y;
	}

	static def add(a, b)
	{
		return new Vector2(a.x + b.x, a.y + b);
	}
}

class Rect
{
	Rect(x,y,w,h)
	{
		self.x = x;
		self.y = y;
		self.width = w;
		self.height = h;
	}

	def right()
	{
		return self.x + self.width;
	}

	def bottom()
	{
		return self.y + self.height;
	}

	def collide(other)
	{
		// eliminate all options
		if (other.right() < self.x)
			return false;

		if (other.x > self.right())
			return false;

		if (other.bottom() < self.y)
			return false;

		if (other.y > self.bottom())
			return false;

		// if we're here then it must be true
		return true;
	}
}

class Sprite
{
	// image paths should be an array of 
	Sprite(imagePaths)
	{
		self.index = 0;
		var images = array();
		var i = 0;
		while(i < imagePaths.size()) {
			images.add(Assets.loadImage(imagePaths.get(i)));
			
			i = i + 1;
		}

		self.images = images;
		self.image = null;
	}

	def update(dt)
	{
		
	}

	def draw(x,y)
	{
		if (self.image != null)
			Canvas.drawImage(self.image, x, y);
	}
}

class GameObject
{
	GameObject(imgName)
	{
		self.pos = new Vector2(0,0);
		self.image = Assets.loadImage(imgName);
		self.debug = false;
		self.rect = null;
	}

	def update(dt)
	{

	}

	def draw()
	{
		Canvas.drawImage(self.image,self.pos.x,self.pos.y);
		if (self.debug){
			if (self.rect != null) {
				Canvas.drawTestRect(self.rect);
			}
		}
	}
}

class Cursor
{
	Cursor()
	{
		self.image = Assets.loadImage("assets/images/cursor.png");
	}

	def draw()
	{
		Canvas.drawImage(self.image, Input.getMouseX() - 14, Input.getMouseY() - 14);
	}
}

class Shooter extends GameObject
{
	Shooter()
	{
		self.GameObject("assets/images/shooter_right.png");
		self.leftImage = Assets.loadImage("assets/images/shooter_left.png");
		self.rightImage = Assets.loadImage("assets/images/shooter_right.png");
		self.rect = new Rect(0, 0, 52, 76);
	}

	def update(dt)
	{
		var p = self.pos;

		if(Input.isKeyDown(Keys.A))
		{
			p.x = p.x-200*dt;
			//self.image = self.leftImage;
		}

		if(Input.isKeyDown(Keys.D))
		{
			p.x = p.x+200*dt;
			//self.image = self.rightImage;
		}

		if(Input.isKeyDown(Keys.W))
		{
			p.y = p.y-200*dt;
		}

		if(Input.isKeyDown(Keys.S))
		{
			p.y = p.y+200*dt;
		}

		// update rect
		self.rect.x = p.x;
		self.rect.y = p.y;

		// update looking dir
		if (Input.getMouseX() < p.x) {
			self.image = self.leftImage;
		} else {
			self.image = self.rightImage;
		}
	}
}

class Slime extends GameObject
{
	def Slime()
	{
		self.speed = 50;
		self.GameObject("assets/images/slime_left.png");
		self.rect = new Rect(0, 0, 52, 44);
	}

	def update(dt)
	{
		var p = self.pos;

		p.x = p.x-self.speed*dt;
		self.rect.x = p.x;
		self.rect.y = p.y;
	}
}

class Bullet extends GameObject
{
	Bullet()
	{
		self.GameObject("assets/images/bullet.png");
		self.dir = new Vector2(0,0);
	}

	def update(dt)
	{
		var p = self.pos;
		p.x = p.x + self.dir.x * dt;
		p.y = p.y + self.dir.y * dt;
	}
}

class Game
{
	def init()
	{
		Input.hideCursor();

		self.cursor = new Cursor();
		self.obj = new Shooter();
		self.mobs = array();
		self.bullets = array();

		var i = 0;
		while(i<5)
		{
			var slime = new Slime();
			slime.pos.x = 600;
			slime.pos.y = i*80;
			self.mobs.add(slime);

			i = i+1;
		}
	}

	def update(dt)
	{
		self.obj.update(dt);

		if (Input.isMousePressed(Mouse.Left)) {
			var bt = new Bullet();
			bt.pos = self.obj.pos.copy();
			bt.dir = new Vector2(10,0);

			self.bullets.add(bt);
			print("firing bullet", bt);
		}

		var mobs = self.mobs;

		var i = 0;
		while(i<mobs.size())
		{
			var b = mobs.get(i);
			b.update(dt);

			if (b.rect.collide(self.obj.rect)) {
				print("collision");
				mobs.remove_at(i);
				i = i - 1;
			}

			i = i+1;
		}

		i = 0;
		var bullets = self.bullets;
		while(i<bullets.size())
		{
			bullets.get(i).update(dt);
			i = i+1;
		}
	}

	def draw()
	{
		self.obj.draw();
		var mobs = self.mobs;
		var i = 0;
		while(i<mobs.size())
		{
			mobs.get(i).draw();
			i = i+1;
		}

		i = 0;
		var bullets = self.bullets;
		while(i<bullets.size())
		{
			bullets.get(i).draw();
			i = i+1;
		}

		self.cursor.draw();
	}

	def doSomething(bullet)
	{
		var i = 100/150;

	}
}