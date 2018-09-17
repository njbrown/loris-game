
class Keys
{
	static var Up = 82;
	static var Down = 81;
	static var Right = 79;
	static var Left = 80;
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

		if(Input.isKeyDown(Keys.Left))
		{
			p.x = p.x-200*dt;
			self.image = self.leftImage;
		}

		if(Input.isKeyDown(Keys.Right))
		{
			p.x = p.x+200*dt;
			self.image = self.rightImage;
		}

		if(Input.isKeyDown(Keys.Up))
		{
			p.y = p.y-200*dt;
		}

		if(Input.isKeyDown(Keys.Down))
		{
			p.y = p.y+200*dt;
		}

		// update rect
		self.rect.x = p.x;
		self.rect.y = p.y;
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
	}

	def update(dt)
	{
		var p = self.pos;
		p.x = p.x+50*dt;
	}
}

class Game
{
	def init()
	{
		self.obj = new Shooter();
		self.bullets = array();

		var i = 0;
		while(i<200)
		{
			var slime = new Slime();
			slime.pos.x = 600;
			slime.pos.y = i*80;
			self.bullets.add(slime);

			i = i+1;
		}
	}

	def update(dt)
	{
		self.obj.update(dt);
		var bullets = self.bullets;

		var i = 0;
		while(i<bullets.size())
		{
			var b = bullets.get(i);
			b.update(dt);

			if (b.rect.collide(self.obj.rect)) {
				print("collision");
				bullets.remove_at(i);
				i = i - 1;
			}

			i = i+1;
		}
	}

	def draw()
	{
		self.obj.draw();
		var bullets = self.bullets;

		var i = 0;
		while(i<bullets.size())
		{
			bullets.get(i).draw();
			i = i+1;
		}
	}

	def doSomething(bullet)
	{
		var i = 100/150;

	}
}