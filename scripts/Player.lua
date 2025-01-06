-- Player.lua
-- Demonstration of using LuaJIT FFI to call into the refactored Game module's OO API.
-- Replicates logic of the C++ WinMain-based Player.exe.

local ffi  = require("ffi")
local Game = require("Game")  -- Your refactored module with Logger, GameConfig, and GamePlayer classes

----------------------------------------------------------------
-- Check if a file or directory exists (simple Lua-based approach):
local function fileExists(path)
    local f = io.open(path, "r")
    if f then
        f:close()
        return true
    end
    return false
end

local function main()
    ----------------------------------------------------------------
    -- 1) Acquire the default logger
    ----------------------------------------------------------------
    local logger = Game.Logger.getDefault()

    ----------------------------------------------------------------
    -- 2) Create and initialize a new game config
    ----------------------------------------------------------------
    local config
    local success, err = pcall(function()
        config = Game.GameConfig:new("Player")
    end)
    if not success or not config then
        logger:error("Failed to create GameConfig! Error: " .. (err or ""))
        return -1
    end

    if not config:init() then
        logger:error("Failed to initialize GameConfig!")
        config:destroy()
        return -1
    end
    
    ----------------------------------------------------------------
    -- 3) Attempt to load the config from file; otherwise save it
    ----------------------------------------------------------------
    local configFile = config:getPath(Game.PathCategory.CONFIG)  -- Typically 0 => BP_PATH_CONFIG
    if not configFile or configFile == "" then
        -- Fallback to a local file, e.g. "Player.json"
        configFile = "Player.json"
    end

    if fileExists(configFile) then
        if not config:load(configFile) then
            logger:warn("Failed to load game configuration from file: " .. configFile)
        end
    else
        if not config:save(configFile) then
            logger:warn("Failed to save default game configuration to file: " .. configFile)
        end
    end

    ----------------------------------------------------------------
    -- 4) Create and initialize the game player
    ----------------------------------------------------------------
    local player
    success, err = pcall(function()
        player = Game.GamePlayer:new("Player")
    end)
    if not success or not player then
        logger:error("Failed to create GamePlayer! Error: " .. (err or ""))
        config:destroy()
        return -1
    end

    -- Pass `nil` for hInstance (not used in this Lua-based approach)
    if not player:init(config, nil) then
        logger:error("Failed to initialize GamePlayer!")
        player:destroy()
        config:destroy()
        return -1
    end

    ----------------------------------------------------------------
    -- 5) Load the actual game composition (here we pass nil or a real file)
    ----------------------------------------------------------------
    if not player:load(nil) then
        logger:error("Failed to load game composition!")
        player:shutdown()
        player:destroy()
        config:destroy()
        return -1
    end

    ----------------------------------------------------------------
    -- 6) Start and run the game
    ----------------------------------------------------------------
    player:play()
    player:run()

    ----------------------------------------------------------------
    -- 7) Shutdown and cleanup
    ----------------------------------------------------------------
    player:shutdown()
    player:destroy()

    -- Save config changes once more (if needed)
    config:save(configFile)
    config:destroy()

    return 0
end

-- Execute our main function
local exitCode = main()
os.exit(exitCode)
