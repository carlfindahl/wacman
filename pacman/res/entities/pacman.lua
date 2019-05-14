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
        x = 3,
        y = 3
    },
    Movement = {
        speed = 3.0        
    },
    Player = {
        lives = 3
    },
    Input = {
        [Action.MOVE_NORTH] = function(e) move(e, 0, -1) print("Hello North") end,
        [Action.MOVE_EAST] = function(e)  move(e, 1, 0) print("Hello East") end,
        [Action.MOVE_SOUTH] = function(e) move(e, 0, 1) print("Hello South") end,
        [Action.MOVE_WEST] = function(e)  move(e, -1, 0) print("Hello West") end
    }
}