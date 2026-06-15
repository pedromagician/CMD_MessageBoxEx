# MessageBoxEx

A customizable command‑line message dialog for Windows

## ✨ Overview

MessageBoxEx is a lightweight Windows command‑line utility that displays a fully customizable dialog window.
You can control the title, message, fonts, colors, icons, buttons, position, timers, and more — all through command‑line parameters.

Perfect for scripts, installers, automation, DevOps workflows, or any scenario where you need a GUI popup from the command line.

## 🚀 Basic Usage

```CMD
MessageBoxEx.exe -t "Title" -m "Message"
```

## 📘 Examples

### ✔️ Simple dialog

```CMD
MessageBoxEx.exe -t "Hello" -m "This is a message"
```

### ✔️ Multiline text

```CMD
MessageBoxEx.exe -m "Line 1\nLine 2\nLine 3" -l 3
```

### ✔️ Unicode support

```CMD
MessageBoxEx.exe -m "Operation completed \u{1F44D}"
```

### ✔️ Custom colors and icon

```CMD
MessageBoxEx.exe -t "Warning" -m "Be careful!" -p FF0000 -b 000000 -i warning.bmp
```

## 🛠️ Command‑Line Options

### 🎨 Appearance

| Switch | Description |
|--------|-------------|
| `-title, -t` | Sets the window title displayed in the dialog’s title bar. |
| `-noTitle, -nt` | Removes the title bar entirely, creating a clean borderless dialog. |
| `-message, -m` | Sets the dialog text. Supports \n for new lines and Unicode via \u\{XXXX}. |
| `-font, -f` | Specifies the font family used for rendering the message text. |
| `-fontsize, -fs` | Sets the font size in points. |
| `-lines, -l` | Defines the number of text lines reserved for the message area. Helps control dialog height. |
| `-width, -w` | Sets the dialog width in pixels. Useful for long messages or custom layouts. |

### 🎨 Colors

| Switch | Description |
|--------|-------------|
| `-pen, -p` | Sets the text (foreground) color in #RRGGBB or RRGGBB hex format. Default: white. |
| `-background, -b` | Sets the background color of the dialog window. Default: black. |
| `-brush, -br` | Sets the brush color used for drawing filled areas (e.g., behind text). Default: black. |

### 🖼️ Icons

| Switch | Description |
|--------|-------------|
| `-icon, -i` | Loads a BMP image displayed inside the dialog. |
| `-iconsize` | Sets the icon size in pixels (width = height). |
| `-iconborder` | Draws a border around the icon for better visual separation. |
| `-iconapp` | Sets the application window icon (ICO or BMP). Affects the title bar and taskbar icon. |

### 🔘 Buttons

| Switch | Description |
|--------|-------------|
| `-button` | Specifies the number of buttons to display. Allowed values: 0\|1\|2\|3 |
| `-widthbuttons` | Adds extra width (in px) to each button. Useful for long labels. |
| `-center` | Centers the button text instead of using default alignment. |
| `-b1`, `-b2`, `-b3` | Sets the label text for button 1, 2, or 3. |
| `-default, -d` | Sets the default button (activated by Enter). Allowed values: 1, 2, 3. |

### 🖥️ Positioning

| Switch | Description |
|--------|-------------|
| `-monitor, -mon` | Selects the monitor where the dialog appears. Options: Primary, Mouse, MousePointer, or monitor index (0, 1, 2, …). |
| `-position, -pos` | Controls dialog placement: Center, Top, Bottom, Left, Right, Pointer, or xy for absolute positioning. |
| `-x`, `-y` | Sets X/Y offset in pixels. Works with -position or absolute xy mode. |
| `-topmost` | Forces the dialog to stay above all non-topmost windows. |

### ⏱️ Timing & Behavior

| Switch | Description |
|--------|-------------|
| `-block` | Blocks the parent process until the dialog is closed. Useful in scripts that must wait for user interaction. |
| `-windowsReturnCode, -wrc` | Enables returning the Windows-style exit code based on the button pressed. |
| `-minimumDisplayTime, -mdt` | Ensures the dialog remains visible for at least xxx milliseconds before it can be closed. |
| `-timeToCompletion, -ttc` | Automatically closes the dialog after xxx milliseconds, even without user interaction. |
| `-fileRequiredForCompletion, -frfc, -file` | The dialog will close automatically once the specified file appears on disk. |
| `-deleteFileRequiredForCompletion, -dfrfc, -deleteFile` | Deletes the file used with -fileRequiredForCompletion after the dialog closes. |

### ❓ Help

| Switch | Description |
|--------|-------------|
| `-help`, `-h`, `-?` | Displays help information. |


## 🧩 Batch Script Example

Store the selected value into a variable:

```CMD
@echo|set /p="@set mytmp=">tmp.bat
@messageboxex -message "Message ..." -button 3 >>tmp.bat
@echo errorlevel=%errorlevel%
@call tmp.bat
@echo output=%mytmp%
```

Or capture the output directly:

```CMD
@set "output="
@for /f "usebackq delims=" %%A in (`messageboxex -message "Message ..." -button 3`) do @set "output=%%A"
@echo output: %output%
```

## 📜 License

Free to use. Amiga Rulez!
