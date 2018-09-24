class Game
{
	def init()
	{
        var images = array();
        self.sprite = new Sprite(array("assets/images/player_idle1.png",
                                        "assets/images/player_idle2.png"));
        self.sprite.speed = 3;
    }

    def update(dt)
    {
        self.sprite.update(dt);
    }

    def draw()
    {
        self.sprite.draw(60, 50);
    }
}