pacman = {
    Collision = {},
    Player = {
        lives = 3,
        icon = 20
    },
    Position = {
        x = 0,
        y = 0
    },
    Movement = {
        speed = 4.0        
    },
    AnimationSprite = {
        fps = 24.0,
        tint = { 1.0, 1.0, 1.0 },
        starting = "right",
        sprites = {
            up = {
                file = "pacman.png",
                startX = 0,
                startY = 70,
                width = 70,
                height = 70,
                cols = 4,
                length = 4
            },
            down = {
                file = "pacman.png",
                startX = 0,
                startY = 0,
                width = 70,
                height = 70,
                cols = 4,
                length = 4
            },
            left = {
                file = "pacman.png",
                startX = 0,
                startY = 140,
                width = 70,
                height = 70,
                cols = 4,
                length = 4
            },
            right = {
                file = "pacman.png",
                startX = 0,
                startY = 210,
                width = 70,
                height = 70,
                cols = 4,
                length = 4
            }
        }
    },
    Input = {
        [Action.MOVE_NORTH] = function(e) 
            move(e, 0, -1)
        end,
        [Action.MOVE_EAST] = function(e)  
            move(e, 1, 0) 
        end,
        [Action.MOVE_SOUTH] = function(e) 
            move(e, 0, 1) 
        end,
        [Action.MOVE_WEST] = function(e)  
            move(e, -1, 0)
        end
    }
}