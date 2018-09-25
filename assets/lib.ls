class Vector2
{
	Vector2(x,y)
	{
		self.x = x;
		self.y = y;

		if(self.x == null)
			self.x = 0;
		if(self.y == null)
			self.y = 0;
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

    def normalize()
    {
        var scale = sqrt((self.x * self.x) + (self.y * self.y));
        self.x = self.x / scale;
        self.y = self.y / scale;
    }

    def scale(sx, sy)
    {
        self.x = self.x * sx;
        self.y = self.y * sy;
    }

	static def add(a, b)
	{
		return new Vector2(a.x + b.x, a.y + b);
	}

    static def sub(a, b)
	{
		return new Vector2(a.x - b.x, a.y - b.y);
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

class SpriteFlipMode
{
	static var None = 0;
	static var Vertical = 1;
	static var Horizontal = 2;
	static var Both = 3;
}

class Sprite
{
	// image paths should be an array of strings
	Sprite(imagePaths)
	{
		self.centerX = 0;
		self.centerY = 0;
		self.index = 0;
		var images = array();
		var i = 0;
		
		while(i < imagePaths.size()) {
			var img = Assets.loadImage(imagePaths.get(i));
			images.add(img);
			
			i = i + 1;
			self.centerX = img.width / 2;
			self.centerY = img.height / 2;
		}

		self.speed = 1;
		self.images = images;
		self.image = null;
        self.frameTime = 1;
        self.time = 0;
		self.rot = 0;
		
		self.flipMode = SpriteFlipMode.None;
	}

	def update(dt)
	{
		self.time = self.time + dt * self.speed;

        var i = floor(mod(self.time / self.frameTime, self.images.size()));
        self.image = self.images.get(i);
	}

	def draw(x,y)
	{
		if (self.image != null)
			//Canvas.drawImage(self.image, x, y);
			Canvas.drawImageEx(self.image, x, y, 1, 1, self.rot, self.centerX, self.centerY, self.flipMode);
	}
}

class GameObject
{
	GameObject(imgName)
	{
		self.pos = new Vector2(0,0);
        self.rot = 0;
        self.scale = 0;

        self.destroyed = false;

		self.image = Assets.loadImage(imgName);
		self.debug = false;
		self.rect = null;
	}

    def setSprite(sprite)
    {
        self.sprite = sprite;
    }

    def destroy()
    {
        self.destroyed = true;
    }

	def update(dt)
	{

	}

	def draw()
	{
		Canvas.drawImage(self.image,self.pos.x,self.pos.y);
		if (self.debug){
			if (self.rect != null) {
				//Canvas.drawTestRect(self.rect);
			}
		}
	}
}

class Group
{
    Group()
    {
        self.objects = array();
    }

    def add(obj)
    {
        self.objects.add(obj);
    }

    def update(dt)
    {

    }

    def draw()
    {
        
    }

    // returns if objects collides with obj
    def collides(obj)
    {

    }

    // returns list of objects collides with obj
    def getCollisions(obj)
    {

    }
}