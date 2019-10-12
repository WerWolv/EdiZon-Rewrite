# How to create a EdiZon save file editor

## Introduction

EdiZon provides a unified interface for developers to build save editors in and for users to use them later on. This allows developers to not worry about writing a fancy GUI for their save editor so they can concentrate on the important thing, the save editing. For this to work EdiZon uses a config file, which describes what properties the save editing interface should display, and a Lua or Python script which does the editing based on the users input.

## Requirements

- A save file of any game
- Experience in either Lua or Python
- Knowledge about file formats and how to analyze an unknown file type

## Config files

For EdiZon to know how to build its interface, a config file is required. A config file is a JSON formatted text file named `<titleID>.json` which gets placed in the `/switch/EdiZon/configs` folder where EdiZon looks for it.

**A typical config file may look like this:**

```json
{
    "author" : "WerWolv",
    "description" : "A sample save editor config to test the editor",
    "beta" : true,
    "all" : [
        {
            "pathRegex" : [ "path", "\\d+", "[a-zA-Z0-9]+_\\d+" ],
            "fileNameRegex" : "filename\\.sav",
            "scriptLanguage" : "python",
            "script" : "test.py",
            "items" : [
                {
                    "name" : "Integer Value A",
                    "description" : "This is a sample integer value",
                    "category" : "Category 1",
                    "arguments" : {
                        "numItems" : 555,
                        "enabled" : true,
                        "item" : "sword",
                        "durability" : 0.125
                    },
                    "widget" : {
                        "type" : "int",
                        "minValue" : 0,
                        "maxValue" : 1000
                    }
                },
                {
                    "name" : "Integer Value B",
                    "description" : "This is a second sample integer value",
                    "category" : "Category 2",
                    "arguments" : {
                        "test" : "Hello World"
                    },
                    "widget" : {
                        "type" : "int",
                        "minValue" : 0,
                        "maxValue" : 1000
                    }
                },
                {
                    "name" : "Boolean Value",
                    "description" : "This is a sample boolean value",
                    "category" : "Category 1",
                    "arguments" : {
                        "pi" : 3.141592654
                    },
                    "widget" : {
                        "type" : "bool",
                        "onValue" : 69,
                        "offValue" : 420
                    }
                },
                {
                    "name" : "String Value",
                    "description" : "This is a sample string value",
                    "category" : "Category 1",
                    "arguments" : {
                        "boolA" : true,
                        "boolB" : false
                    },
                    "widget" : {
                        "type" : "string",
                        "minLength" : 5,
                        "maxLength" : 32
                    }
                },
                {
                    "name" : "List Value",
                    "description" : "This is a sample list value",
                    "category" : "Category 1",
                    "arguments" : {
                        "address" : "0x12345",
                        "size" : 8
                    },
                    "widget" : {
                        "type" : "list",
                        "keys" : [ "Value 1", "Value 2", "Value 3", "Value 4"],
                        "values" : [ 555, 1337, 69, 420 ]
                    }
                }
            ]
        }
    ]
}
```

### Config file metadata

| Key name        | Description | Example values |
|-----------------|-------------|----------------|
| `author`        | The author of the save file editor. | `WerWolv` |
| `description`   | A short description that will be presented to the user when opening the save editor menu. | `Can edit Gold, Health points and items` |
| `beta`          | Weather this save editor has been tested by multiple people and it's save to use for everybody | `false` |
| `all` / `X.Y.Z` | Here the key name can vary and there can be multiple different of these keys. If the `X.Y.Z` format is used, this portion of the config file will only be loaded if this specific version of the game is installed on the Switch. Using `all` here instead will make this part of the config be used for every game version. | `A list of version specific config metadata that may be used for different files` |

### Version and file-specific metadata

The value of the in `Config file metadata` described `all` / `X.Y.Z` key is a list of these Version and file-specific metadata items. Each entry of this list may have different configurations and therefor may target a different file in the save filesystem. This is used for if a game stores different information in separate files (ex. inventory data goes into inventory.bin, player data goes into player.bin).

| Key name        | Description | Example values |
|-----------------|-------------|----------------|
| `pathRegex`     | A list of strings containing one part of the path to the save file each. These strings are RegEx strings so every directory in the path can be matched by RegEx. | `[ "path", "\\d+", "[a-zA-Z0-9]+_\\d+" ]` |
| `fileNameRegex` | A regex string to match the save file which will be edited by EdiZon | `filename\\.sav` |
| `scriptLanguage` | The script language that will be used to edit the save file. | `python` or `lua` |
| `script`        | The filename of the script that will be used to edit the save file | `json.lua` |
| `items`        | This is a list of the config items that will be displayed inside the editor interface. Each item will be used to edit one property in the save file | See `Widget metadata` |

### Widget metadata

Widgets are the list items in the editor window that will communicate with the script and ultimately edit the save file depending on the user's input.

| Key name        | Description | Example values |
|-----------------|-------------|----------------|
| `name`          | The name of the property | `Coin count` |
| `description`   | A small description about what this widget edits | `The amount of golden coins the player has.` |
| `category`     | The name of the category this widget will be sorted in. | `Collectibles` |
| `args`          | Arguments sent to the script file when this widget gets used | See `Arguments` |
| `widget`        | The widget dependant metadata | See `Widget dependant metadata` |

### Arguments

These key-value-pairs will be sent to the script whenever the user interacts with a widget. These can be used to tell the script what address to modify, what property to change or the size or type of the value that will get edited. The names of the argument keys can be any string that fits the script's needs. Values can be `numbers`, `floats`, `strings` or `booleans`. Lists, json objects and null will cause the arguments to not be passed to the script and therefor being treated as undefined.

**Both the keys and the values inside the arguments object are user defined and can be set to whatever is useful for the script.**

### Widget dependent metadata

There are multiple different types of widgets that can be used to modify data. The available types are `int`, `bool`, `string`, `list`, `slider`, `progress` and `comment` which are the only valid values the `type` value can be set to. Any other one will cause the widget to not load. Dependent on the value `type` was set to, EdiZon will expect different parameters to be set.

#### Integer Widget

A widget that will open the keyboard when selected and expects a number to be entered by the user. The script won't be notified when the user just closes the keyboard.

| Key name        | Description | Example values |
|-----------------|-------------|----------------|
| `minValue`      | The minimum value this widget can be set to. Values lower than it will be capped to `minValue`. | `0` |
| `maxValue`      | The maximum value this widget can be set to. Values higher than it will be capped to `maxValue` | `9999` |

#### Boolean Widget

A widget that can have a ON and a OFF state which gets toggled when selected. If the value reported by the script is neither `onValue` nor `offValue`, EdiZon will interpret this as the property being turned off and will set the value to `onValue` the next time the user selects the widget.

| Key name        | Description | Example values |
|-----------------|-------------|----------------|
| `onValue`       | The value that will be sent to the script when the on-state is selected  | `1234` |
| `offValue`      | The value that will be sent to the script when the off-state is selected | `1000` |

#### String Widget

A widget that will open the keyboard when selected and expects a number to be entered by the user. The script won't be notified when the user just closes the keyboard.

| Key name        | Description | Example values |
|-----------------|-------------|----------------|
| `minLength`      | The minimum length of the string this widget can be set to. If a shorter string is entered, the input will be ignored and the script won't get notified. | `0` |
| `maxLength`      | The maximum length of the string this widget can be set to. The software keyboard will prevent the user to enter a string longer than this | `32` |

#### List Widget

A widget that will display a dropdown menu when selected. This allows for naming an unlimited amount of values and displaying them in a list to present to the user. Every string in the keys list will be mapped to the value at the same index in the values list therefor the keys list and the values list have to be the same length. Values can be `numbers`, `floats`, `booleans` and `strings` but they should all be of the same type.

| Key name        | Description | Example values |
|-----------------|-------------|----------------|
| `keys`          | The names displayed to the user. | `[ "Value 1", "Value 2", "Value 3", "Value 4"]` |
| `values`        | The values sent to the script. | `[ 555, 1337, 69, 420 ]` |

**TODO: slider**</br>
**TODO: progressbar**

#### Comment Widget

A widget that will insert some text in the widget list used to give additional information about anything in the editor screen.

| Key name        | Description | Example values |
|-----------------|-------------|----------------|
| `comment`       | The text that will be displayed. | `Make sure that the tutorial has been completed before enabling this option.`  |

## Script Files

Save editor scripts can be either written in Lua or Python. EdiZon bundles Lua version 5.3.5 and Python 3.8 together with the entirety of it's standard library. If needed, extra libraries can be added and imported in the save editing script.

### Hello World script

#### Lua

!!! Note
    The `edizon` module does **not** have to be imported in lua. It's a publicly registered module that will be loaded automatically and will always be called `edizon`.

```lua
-- Load the current save data in as a byte array --
saveFileBuffer = edizon.getDataAsBuffer()

-- Get a value from the save data --
function getValue()
    -- Get the address and size argument from the config file for this widget --
    address = edizon.getArgument("address")
    size = edizon.getArgument("size")
    value = ...
    -- ... use address and size to get the requested value from the save file ... --

    return value
end

-- Set a value in the save data --
function setValue(value)
    -- Get the address and size argument from the config file for this widget --
    address = edizon.getArgument("address")
    size = edizon.getArgument("size")

    -- ... use address and size to set the requested part of the save file to value ... --

    saveFileBuffer[...] = value
end

-- Return the modified save data to EdiZon --
function getModifiedSaveFileData()
    -- Recalculate checksums / fixup save data to be ready for injecting --
    return saveFileBuffer
end
```

#### Python

!!! Note
    The `edizon` module does have to be imported manually on Python.

```python
# Import edizon module
import edizon

# Load the current save data in as a byte array
saveFileBuffer = edizon.getDataAsBuffer()

# Get a value from the save data
def getValue():
    # Get the address and size argument from the config file for this widget
    address = edizon.getArgument("address")
    size = edizon.getArgument("size")

    # ... use address and size to get the requested value from the save file ...

    return value

# Set a value in the save data
def setValue(value):
    # Get the address and size argument from the config file for this widget
    address = edizon.getArgument("address")
    size = edizon.getArgument("size")

    # ... use address and size to set the requested part of the save file to value ...
    saveFileBuffer[...] = value

# Return the modified save data to EdiZon
def getModifiedSaveFileData():
    # Recalculate checksums / fixup save data to be ready for injecting
    return saveFileBuffer
```

### _**Functions called by EdiZon**_

#### `Global space`

Before EdiZon calls any functions in the script, the code in the global space will be executed. This is where potential save file parsing should be done as the global space will only be executed once on startup.

!!! Warning
    Do **not** parse the whole save data in the `getValue` or `setValue` function! These functions will get executed whenever the user interacts with the editor interface. Parsing the save data every time this happens will lag out the interface and should not be done.

#### `getValue( ) -> int | float | boolean | string`

This function gets called when EdiZon wants to display the current value of a property in the editor interface. It should load the current arguments from the config file and use them to load the correct value from the save data. This value then shall be sent to EdiZon by returning it from the function (`return value`). This value can be of type `int`, `float`, `boolean` or `string` dependant on the config file and the widget used.

#### `setValue( int | float | boolean | string )`

This function gets called when the user interacts with any widget and therefor a value in the save data should be updated. This value will be passed in as the first and only argument of the function and can be of type `int`, `float`, `boolean` or `string` dependant on the config file and the widget used.

#### `getModifiedSaveFileData( ) -> byte[]`

This function will get called as soon as the user decides that they are happy with the modifications and hit `Apply` in the interface. It should re-encode the parsed save data, potentially recalculate checksums and do anything necessary for the save data to be readable again by the game. Once this is done it should return the data from the function back to EdiZon (`return saveDataBuffer`). Only byte arrays can be returned from this function.

### _**EdiZon functions callable by the script**_

#### `edizon.getDataAsBuffer( ) -> byte[]`

This function returns the currently loaded save data as a byte array. Useful for binary save formats.

#### `edizon.getDataAsString( ) -> string`

This function returns the currently loaded save data as a string. Useful for plain text save formats.

#### `edizon.getArgument( string ) -> int | float | boolean | string`

Every time the user interacts with a widget, EdiZon loads the arguments of that widget from the config and puts them into the script engine. This function then can be used to access these argument values based on their names. Like this the script can know what value should get edited and react accordingly.
