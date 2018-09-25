class Game
{
	def init()
	{
        var images = array();
        self.sprite = new Sprite(array("assets/images/player_idle1.png",
                                        "assets/images/player_idle2.png"));
        self.sprite.speed = 3;

        self.gun = new Sprite(array("assets/images/basic_gun.png"));
        self.gun.centerX = 4;
    }

    def update(dt)
    {
        self.sprite.update(dt);
        self.gun.update(dt);

        //print(Input.getMouseX());
        if (Input.getMouseX() < 60) {
            self.sprite.flipMode = SpriteFlipMode.Horizontal;
            self.gun.flipMode = SpriteFlipMode.Vertical;
        }
        else {
            self.sprite.flipMode = SpriteFlipMode.None;
            self.gun.flipMode = SpriteFlipMode.None;
        }

        //self.sprite.rot = Input.getMouseX();

        var rot = degrees(atan2(Input.getMouseY() - 60, Input.getMouseX() - 50));
        //print(rot);
        self.gun.rot = rot;
    }

    def draw()
    {
        self.sprite.draw(60, 50);
        self.gun.draw(63, 60);
        
    }
}