class Cursor
{
	Cursor()
	{
		self.image = Assets.loadImage("assets/images/cursor.png");
	}

	def draw()
	{
		Canvas.drawImage(self.image, Input.getMouseX() - 3.5, Input.getMouseY() - 3.5);
	}
}

class Shooter extends GameObject
{
	Shooter()
	{
		self.GameObject("assets/images/shooter_right.png");
		self.leftImage = Assets.loadImage("assets/images/shooter_left.png");
		self.rightImage = Assets.loadImage("assets/images/shooter_right.png");
		self.rect = new Rect(0, 0, 13, 19);
		self.moveSpeed = 60;
	}

	def update(dt)
	{
		var p = self.pos;

		if(Input.isKeyDown(Keys.A))
		{
			p.x = p.x-50*dt;
			//self.image = self.leftImage;
		}

		if(Input.isKeyDown(Keys.D))
		{
			p.x = p.x+50*dt;
			//self.image = self.rightImage;
		}

		if(Input.isKeyDown(Keys.W))
		{
			p.y = p.y-50*dt;
		}

		if(Input.isKeyDown(Keys.S))
		{
			p.y = p.y+50*dt;
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
		while(i<200)
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