# SetTeamColor
This plugin is for **Star Wars: Battle Front (2004)** game.
It allow the mini-map to have faction colors rather than being limited to only Allies, Enemies and Rebels.
The plugin can be used by injecting the DLL directly to the game process.

This plugin requires [BasePlugin](https://github.com/abkarino/SWBF-BasePlugin) in order to work.

This plugin is intented to be used with EGM launcher by Antstafer.

## Usage
The plugin creates a LUA function `SetTeamColor` that can be used in map scripts.

```lua
SetTeamColor(OwnerTeam, TargetTeam, R, G, B)
```
### Arguments
- `OwnerTeam`: This is the team, that you are in.
- `TargetTeam`: This is the team, that you will be seeing.
- `R`: This is the Red component of the RGB color specified as integer from 0 to 255 inclusive.
- `G`: This is the Green component of the RGB color specified as integer from 0 to 255 inclusive.
- `B`: This is the Blue component of the RGB color specified as integer from 0 to 255 inclusive.

**NOTE:** The function is not bi-directional. i.e. you will need to set how team A sees team B and how team B sees team A. If not specified, the default mechanism relies on whether the team is Ally, Enemy or Neutral.

### Example
```lua
SetTeamColor(1, 2, 0, 0, 255)
```
This tells the game that if the selected character belongs to `Team 1`, it would see `Team 2` on the map as BLUE `#0000FF`.

Most maps would be using the constants `ATT` or `DEF` for teams which is equivelent to `0` and `1` respectively.