import os
from Game import (
    GameConfig,
    GamePlayer,
    PathCategory,
)

def file_exists(path: str) -> bool:
    return os.path.isfile(path)

def main() -> int:
    # 1) Create and initialize a new GameConfig
    try:
        config = GameConfig()
    except RuntimeError as e:
        print(f"Failed to create GameConfig: {e}")
        return -1

    if not config.init():
        print("Failed to initialize GameConfig!")
        config.destroy()
        return -1

    # 2) Attempt to load config from file or save a default
    config_file = config.get_path(PathCategory.CONFIG)
    if not config_file:
        config_file = "Player.json"  # Fallback if empty

    if file_exists(config_file):
        if not config.load(config_file):
            print(f"Warning: Failed to load config from '{config_file}'")
    else:
        if not config.save(config_file):
            print(f"Warning: Failed to save default config to '{config_file}'")

    # 3) Create and initialize the GamePlayer
    try:
        player = GamePlayer()
    except RuntimeError as e:
        print(f"Failed to create GamePlayer: {e}")
        config.destroy()
        return -1

    if not player.init(config, None):
        print("Failed to initialize GamePlayer!")
        player.destroy()
        config.destroy()
        return -1

    # 4) Load the actual game composition (pass None or a real path)
    if not player.load(None):
        print("Failed to load game composition!")
        player.shutdown()
        player.destroy()
        config.destroy()
        return -1

    # 6) Start and run the game
    player.play()
    player.run()

    # 7) Shutdown and cleanup
    player.shutdown()
    player.destroy()

    # Save config changes
    config.save(config_file)
    config.destroy()

    return 0

if __name__ == "__main__":
    exit_code = main()
    os._exit(exit_code)
