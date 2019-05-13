pacman = {
    AnimationSprite = {
        fps = 24.0,
        tint = { 1.0, 1.0 , 1.0 },
        starting = "right",
        sprites = {
            up = {
                file = "pacman.png",
                startX = 0,
                startY = 1,
                size = 70,
                cols = 4,
                length = 4
            },
            down = {
                file = "pacman.png",
                startX = 0,
                startY = 0,
                size = 70,
                cols = 4,
                length = 4
            },
            left = {
                file = "pacman.png",
                startX = 0,
                startY = 2,
                size = 70,
                cols = 4,
                length = 4
            },
            right = {
                file = "pacman.png",
                startX = 0,
                startY = 3,
                size = 70,
                cols = 4,
                length = 4
            }
        }
    },
    Position = {
        x = 0,
        y = 0
    },
    Movement = {
        speed = 3.0        
    },
    Player = {
        lives = 3
    },
    Input = {
        [Action.MOVE_NORTH] = function() print("Hello North") end,
        [Action.MOVE_EAST] = function() print("Hello East") end,
        [Action.MOVE_SOUTH] = function() print("Hello South") end,
        [Action.MOVE_WEST] = function() print("Hello West") end
    }
}