-- Pickup Sound Effect
function psound(data)
    if data.pickup_name ~= "food" then
        play_sound("powerup_pickup")
    else
        play_sound("food_pickup")
    end
end

-- Ghost Death
function gdsound(data)
    if data.new_state == AIState.DEAD then
        play_sound("ghost_die")
    end
end

-- Pacman Sounds
function plsound(data)
    -- If we lost a life then play hurt sound
    if data.delta < 0 then
        play_sound("ghost_die")
    end

    -- If we died, then game over
    if data.new_life == 0 then
        play_sound("game_over")
    end
end

-- Subscribe to Events with functions
event_sub = {
    pickup_sound = connect("Pickup", psound),
    ghost_sound = connect("GhostStateChanged", gdsound),
    pac_sound = connect("PacLifeChanged", plsound)
}
